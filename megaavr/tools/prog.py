#!/usr/bin/python3

# -*- coding: utf-8 -*-
import sys
import os
import argparse

# dependencies
toolspath = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(toolspath, "libs"))

import pymcuprog.pymcuprog_main as pymcu
from pymcuprog.pymcuprog import setup_logging

import logging

import datetime


class PyMcuException(Exception):
    pass


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("-a", "--action",
                        type=str,
                        default="",
                        help="Action to perform {write, read, erase}.")

    parser.add_argument("-b", "--baudrate",
                        type=str,
                        default="115200",
                        help="Serial baud rate, if applicable, (default: 115200).")

    parser.add_argument("-wc", "--write_chunk",
                        type=int,
                        default=-1,
                        help="Max number of bytes of serial data to write per usb packet. -1 (whole page) is recommended. (Default: -1 (no write chunking)) Intended as workaround for specific serial adapters. ")

    parser.add_argument("-rc", "--read_chunk",
                        type=int,
                        default=-1,
                        help="Max number of bytes to request from the device at a time when reading or verifying. (Default: -1 (maximum - usually 512b)) Intended as a workaround for specific serial adapters.")

    parser.add_argument("-d", "--device",
                        type=str,
                        help="Part number, lowercase (e.g. attiny412, ...).")

    parser.add_argument("--fuses",
                        nargs='+',
                        type=str,
                        default="",
                        help="List of offset:value (0x, 0b or decimal).")

    parser.add_argument("--fuses_print",
                        action="store_true",
                        help="Print fuse values.")

    parser.add_argument("-f", "--filename",
                        type=str,
                        default="",
                        help="Hex file to read/write.")

    parser.add_argument("-wd", "--writedelay",
                        type=float,
                        default=0,
                        help="Page write delay [ms] for tinyAVR and megaAVR. USB latency is usually sufficient without this. (Default: 0 - this severely impacts write performance)")

    parser.add_argument("-t", "--tool",
                        type=str,
                        default="uart",
                        help="Tool name, defaults to 'uart' (SerialUPDI) mode. The other options are neither tested nor maintained.")

    parser.add_argument("-u", "--uart",
                        type=str,
                        default="",
                        help="Serial port to use if tool is uart.")

    parser.add_argument("-s", "--serialnumber",
                        type=str,
                        default="",
                        help="Tool USB serial (optional, for non-Serial UPDI programmers only. This feature is neither tested nor maintained.).")

    parser.add_argument("-v", "--verbose",
                        action="count",
                        default=0,
                        help="Display more info (can be repeated).")

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

    logging_level = logging.ERROR
    if args.verbose == 1:
        logging_level = logging.INFO
    elif args.verbose > 1:
        logging_level = logging.DEBUG

    try:
        setup_logging(user_requested_level=logging_level)
        return_code = pymcuprog_basic(args, fuses_dict)
        sys.exit(return_code)
    except PyMcuException as e:
        print("Error: ".format(e))
        sys.exit(1)


def run_pymcu_action(func, backend, *args, **kwargs):
    args_pymcu = argparse.Namespace(**kwargs)
    time_start = datetime.datetime.now()
    status = func(backend, *args, args_pymcu)
    time_stop = datetime.datetime.now()
    print("Action took {:.2f}s".format((time_stop - time_start).total_seconds()))
    if status != pymcu.STATUS_SUCCESS:
        backend.end_session()
        backend.disconnect_from_tool()
        raise PyMcuException("Call to {} failed".format(func.__name__))


def print_report(args):
    print("SerialUPDI")
    print("UPDI programming for Arduino using a serial adapter")
    print("Based on pymcuprog, with significant modifications")
    print("By Quentin Bolsee and Spence Konde")
    print("Version 1.2.0 - June 2021")
    print("Using serial port {} at {} baud.".format(args.uart, args.baudrate))
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
    args_start = argparse.Namespace(interface="updi",
                                    clk=args.baudrate,
                                    high_voltage=False,
                                    user_row_locked_device=False,
                                    chip_erase_locked_device=False,
                                    packpath=False)

    status = pymcu._start_session(backend,
                                  device_selected,
                                  args_start)

    if status != pymcu.STATUS_SUCCESS:
        if status == pymcu.STATUS_FAILURE_LOCKED and args.action in ("write", "erase"):
            print("Locked state detected, performing chip erase")
            args_start.chip_erase_locked_device = True
            status = pymcu._start_session(backend,
                                          device_selected,
                                          args_start)
            if status != pymcu.STATUS_SUCCESS:
                raise PyMcuException("Failed to unlock!")
        else:
            raise PyMcuException("Cannot start session!")

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
                         verify=False,
                         filename=args.filename,
                         blocksize=None if args.write_chunk <= 0 else args.write_chunk,
                         pagewrite_delay=args.writedelay)

        run_pymcu_action(pymcu._action_verify, backend,
                         memory=pymcu.MemoryNameAliases.ALL,
                         offset=0,
                         literal=None,
                         filename=args.filename,
                         max_read_chunk=None if args.read_chunk <= 0 else args.read_chunk)

    elif args.action == "read":
        run_pymcu_action(pymcu._action_read, backend,
                         memory=pymcu.MemoryNames.FLASH,
                         offset=0,
                         bytes=0,
                         filename=args.filename,
                         max_read_chunk=None if args.read_chunk <= 0 else args.read_chunk)
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
