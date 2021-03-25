#!/usr/bin/python3

# -*- coding: utf-8 -*-
import re
import sys
import os
import argparse

# dependencies
toolspath = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(toolspath, "libs"))

import pymcuprog.pymcuprog_main as pymcu
from pymcuprog.pymcuprog import setup_logging

import logging


class PyMcuException(Exception):
    pass


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("-a", "--action",
                        type=str,
                        default="",
                        help="Action to perform {write, read, erase}.")

    parser.add_argument("-d", "--device",
                        type=str,
                        help="Device type (e.g. attiny412, ...).")

    parser.add_argument("--fuses",
                        nargs='+',
                        type=str,
                        default="",
                        help="List of offset:value (0x, 0b or decimal).")

    parser.add_argument("--fuses_print",
                        action="store_true",
                        help="Print fuses values.")

    parser.add_argument("-f", "--filename",
                        type=str,
                        default="",
                        help="Hex file to read/write.")

    parser.add_argument("-t", "--tool",
                        type=str,
                        default="",
                        help="Tool name, 'uart' for serial.")

    parser.add_argument("-u", "--uart",
                        type=str,
                        default="",
                        help="Uart port to use if tool is uart.")

    parser.add_argument("-s", "--serialnumber",
                        type=str,
                        default="",
                        help="Tool USB serial (optional).")

    parser.add_argument("-v", "--verbose",
                        action="store_true",
                        help="Display more info.")

    # Parse args
    args = parser.parse_args()

    if args.action == "" and args.fuses == "" and not args.fuses_print:
        parser.print_help()
        sys.exit(0)

    if args.action != "" and args.action not in ("read", "write", "erase"):
        print("Error: unknown action '{}'".format(args.action))
        sys.exit(1)

    if args.action not in ("read", "write") and args.filename != "":
        print("Error: action '{}' takes no filename".format(args.action))
        sys.exit(1)

    if args.action in ("read", "write") and args.filename == "":
        print("Error: no filename provided")
        sys.exit(1)

    fuses_dict = {}
    for fuse_str in args.fuses:
        fuse_offset, fuse_val = fuse_str.split(":")
        try:
            fuse_offset = int(fuse_offset, 0)
            fuse_val = int(fuse_val, 0)
            fuses_dict[fuse_offset] = fuse_val
        except ValueError as e:
            print("Error: cannot parse fuse, '{}'".format(e))
            sys.exit(1)

    print_report(args)

    try:
        if args.verbose:
            setup_logging(user_requested_level=logging.DEBUG)
        return_code = pymcuprog_basic(args, fuses_dict)
        sys.exit(return_code)
    except PyMcuException as e:
        print("Error: ".format(e))
        sys.exit(1)


def run_pymcu_action(func, backend, *args, **kwargs):
    args_pymcu = argparse.Namespace(**kwargs)
    status = func(backend, *args, args_pymcu)
    if status != pymcu.STATUS_SUCCESS:
        backend.end_session()
        backend.disconnect_from_tool()
        raise PyMcuException("Call to {} failed".format(func.__name__))


def print_report(args):
    print("Arduino <---> pymcuprog bridge by Quentin Bolsee and Spence Konde")
    print("Target: {}".format(args.device))
    if args.fuses != "":
        print("Set fuses: {}".format(args.fuses))
    print("Action: {}".format(args.action))
    if args.filename != "":
        print("File: {}".format(args.filename))


def pymcuprog_basic(args, fuses_dict):
    """
    Main program
    """
    backend = pymcu.Backend()

    # connect to tool
    toolconnection = pymcu._setup_tool_connection(argparse.Namespace(tool=args.tool,
                                                                     uart=args.uart,
                                                                     serialnumber=args.serialnumber))
    try:
        backend.connect_to_tool(toolconnection)
    except pymcu.PymcuprogToolConnectionError as error:
        print("Error: Cannot connect, '{}'".format(error))
        return 1

    # select device
    device_selected = pymcu._select_target_device(backend, argparse.Namespace(device=args.device, tool=args.tool))
    if device_selected is None:
        backend.disconnect_from_tool()
        print("Error: device selection failed")
        return 1

    # start session
    run_pymcu_action(pymcu._start_session, backend, device_selected,
                     interface="updi",
                     clk=False,
                     high_voltage=False,
                     user_row_locked_device=False,
                     chip_erase_locked_device=False,
                     packpath=False)

    try:
        pymcu._action_ping(backend)
    except ValueError:
        print("Device ID mismatch! Stopping.")
        backend.end_session()
        backend.disconnect_from_tool()
        return 1

    # write fuses
    if fuses_dict is not None and len(fuses_dict) > 0:
        for fuse_offset in sorted(fuses_dict.keys()):
            fuse_value = fuses_dict[fuse_offset]
            print("Setting fuse {}={}".format(hex(fuse_offset), hex(fuse_value)))
            run_pymcu_action(pymcu._action_write, backend,
                             offset=fuse_offset,
                             literal=[fuse_value],
                             memory=pymcu.MemoryNames.FUSES,
                             verify=True,
                             filename=None)
        print("Finished writing fuses.")

    # print fuses
    if args.fuses_print:
        run_pymcu_action(pymcu._action_read, backend,
                         memory=pymcu.MemoryNames.FUSES,
                         offset=0,
                         bytes=0,
                         filename=None)

    # actions
    if args.action == "write":
        run_pymcu_action(pymcu._action_erase, backend,
                         memory=pymcu.MemoryNameAliases.ALL,
                         offset=0)

        run_pymcu_action(pymcu._action_write, backend,
                         memory=pymcu.MemoryNameAliases.ALL,
                         offset=0,
                         literal=None,
                         verify=True,
                         filename=args.filename)

        run_pymcu_action(pymcu._action_verify, backend,
                         filename=args.filename,
                         offset=0,
                         literal=None)
    elif args.action == "read":
        run_pymcu_action(pymcu._action_read, backend,
                         memory=pymcu.MemoryNames.FLASH,
                         offset=0,
                         bytes=0,
                         filename=args.filename)
    elif args.action == "erase":
        run_pymcu_action(pymcu._action_erase, backend,
                         memory=pymcu.MemoryNameAliases.ALL,
                         offset=0)

    # close session
    backend.end_session()
    backend.disconnect_from_tool()
    return 0


if __name__ == '__main__':
    main()
