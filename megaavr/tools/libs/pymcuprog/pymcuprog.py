"""
Python MCU programmer Command Line Interface utility
"""
# Python 3 compatibility for Python 2
from __future__ import print_function

# args, logging
import sys
import argparse
import os
import logging
from logging.config import dictConfig
import textwrap
import yaml

try:
    from pathlib import Path
except ImportError:
    from pathlib2 import Path  # python 2 backport

from appdirs import user_log_dir
from yaml.scanner import ScannerError

# pymcuprog main function
from . import pymcuprog_main
from .pymcuprog_main import WRITE_TO_HEX_MEMORIES
from .deviceinfo.memorynames import MemoryNames, MemoryNameAliases


def setup_logging(user_requested_level=logging.WARNING, default_path='logging.yaml',
                  env_key='MICROCHIP_PYTHONTOOLS_CONFIG'):
    """
    Setup logging configuration for pymcuprog CLI
    """
    # Logging config YAML file can be specified via environment variable
    value = os.getenv(env_key, None)
    if value:
        path = value
    else:
        # Otherwise use the one shipped with this application
        path = os.path.join(os.path.dirname(__file__), default_path)
    # Load the YAML if possible
    if os.path.exists(path):
        try:
            with open(path, 'rt') as file:
                # Load logging configfile from yaml
                configfile = yaml.safe_load(file)
                # File logging goes to user log directory under Microchip/modulename
                logdir = user_log_dir(__name__, "Microchip")
                # Look through all handlers, and prepend log directory to redirect all file loggers
                num_file_handlers = 0
                for handler in configfile['handlers'].keys():
                    # A filename key
                    if 'filename' in configfile['handlers'][handler].keys():
                        configfile['handlers'][handler]['filename'] = os.path.join(
                            logdir, configfile['handlers'][handler]['filename'])
                        num_file_handlers += 1
                # If file logging is enabled, it needs a folder
                if num_file_handlers > 0:
                    # Create it if it does not exist
                    Path(logdir).mkdir(exist_ok=True, parents=True)
                # Console logging takes granularity argument from CLI user
                configfile['handlers']['console']['level'] = user_requested_level
                # Root logger must be the most verbose of the ALL YAML configurations and the CLI user argument
                most_verbose_logging = min(user_requested_level, getattr(logging, configfile['root']['level']))
                for handler in configfile['handlers'].keys():
                    # A filename key
                    if 'filename' in configfile['handlers'][handler].keys():
                        level = getattr(logging, configfile['handlers'][handler]['level'])
                        most_verbose_logging = min(most_verbose_logging, level)
                configfile['root']['level'] = most_verbose_logging
            dictConfig(configfile)
            return
        except ScannerError:
            # Error while parsing YAML
            print("Error parsing logging config file '{}'".format(path))
        except KeyError as keyerror:
            # Error looking for custom fields in YAML
            print("Key {} not found in logging config file".format(keyerror))
    else:
        # Config specified by environment variable not found
        print("Unable to open logging config file '{}'".format(path))

    # If all else fails, revert to basic logging at specified level for this application
    print("Reverting to basic logging.")
    logging.basicConfig(level=user_requested_level)


# Helper functions
def _parse_literal(literal):
    """
    Literals can either be integers or float values.  Default is Integer
    """
    try:
        return int(literal, 0)
    except ValueError:
        return float(literal)


def main():
    """
    Entrypoint for installable CLI

    Configures the CLI and parses the arguments
    """
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=textwrap.dedent('''\
    Generic programmer of selected AVR, PIC and SAM devices

    Basic actions:
        - ping: reads the device ID or signature
        - read: read NVM
        - write: write NVM
        - erase: erase NVM
            '''),
        epilog=textwrap.dedent('''\
    Usage examples:

        Ping a device on-board a kit:
        - pymcuprog.py ping

        Ping a device using Atmel-ICE
        - pymcuprog.py -t atmelice -d atmega4809 -i updi ping

        Read the some bytes of flash:
        - pymcuprog.py read -m flash -o 0x80 -b 64

        Erase an UPDI device:
        - pymcuprog.py erase

        Erase a locked UPDI device:
        - pymcuprog.py ping --chip-erase-locked-device

        Set target supply voltage on a kit:
        - pymcuprog.py setsupplyvoltage -l 3.3
            '''))

    parser.add_argument("action",
                        help="action to perform",
                        # This makes the action argument optional
                        # only if -V/--version or -R/release_info argument is given
                        nargs="?" if "-V" in sys.argv or "--version" in sys.argv \
                                     or "-R" in sys.argv or "--release-info" in sys.argv else None,
                        default="ping",
                        # nargs='?', # this makes ping the default, and -h the only way to get usage()
                        choices=['ping', 'erase', 'read', 'write', 'verify', 'getvoltage', 'getsupplyvoltage',
                                 'reboot-debugger',
                                 'setsupplyvoltage', 'getusbvoltage', 'reset'])

    # Device to program
    parser.add_argument("-d", "--device",
                        type=str,
                        help="device to program")

    # Pack path
    parser.add_argument("-p", "--packpath",
                        type=str,
                        help="path to pack")

    # Tool to use
    parser.add_argument("-t", "--tool",
                        type=str,
                        help="tool to connect to")

    parser.add_argument("-s", "--serialnumber",
                        type=str,
                        help="USB serial number of the unit to use")

    # Memtype
    memtype_helpstring = "memory area to access: {}".format(MemoryNameAliases.ALL)
    for memtype in MemoryNames.get_all():
        memtype_helpstring += ", '{}'".format(memtype)
    parser.add_argument("-m", "--memory",
                        type=str,
                        default=MemoryNameAliases.ALL,
                        help=memtype_helpstring)

    parser.add_argument("-o", "--offset",
                        type=lambda x: int(x, 0),
                        default="0",
                        help="memory byte offset to access")

    parser.add_argument("-b", "--bytes",
                        type=int,
                        default=0,
                        help="number of bytes to access")

    parser.add_argument("-l", "--literal",
                        type=_parse_literal,
                        nargs='+',
                        help="literal values to write")

    filename_helpstring_extra = "Note that when reading to hex file only "
    filename_helpstring_extra += ", ".join(WRITE_TO_HEX_MEMORIES)
    filename_helpstring_extra += " memories will be written to the hex file"
    parser.add_argument("-f", "--filename",
                        type=str,
                        help="file to write / read. "
                             "{}".format(filename_helpstring_extra))

    parser.add_argument("-c", "--clk",
                        type=str,
                        help="clock frequency in Hz (bps) for programming interface. "
                             "(eg: '-c 32768' or '-c 115k' or '-c 1M')")

    parser.add_argument("-u", "--uart",
                        type=str,
                        help="UART to use for UPDI")

    parser.add_argument("-i", "--interface",
                        type=str,
                        help="Interface to use")

    parser.add_argument("-v", "--verbose",
                        default="warning", choices=['debug', 'info', 'warning', 'error', 'critical'],
                        help="Logging verbosity level")

    parser.add_argument("-V", "--version",
                        help="Print pymcuprog version number and exit",
                        action="store_true")

    parser.add_argument("-R", "--release-info", action="store_true",
                        help="Print pymcuprog release details and exit")

    parser.add_argument("--verify",
                        help="verify after write from file",
                        action="store_true")

    parser.add_argument("-x", "--timing",
                        help="add timing output",
                        action="store_true")

    # Ex-options
    parser.add_argument("-hv", "--high-voltage",
                        choices=['tool-toggle-power', 'user-toggle-power', 'simple-unsafe-pulse'],
                        help="UPDI high-voltage activation mode")

    parser.add_argument("-ul", "--user-row-locked-device",
                        help="Writes the User Row on a locked device (UPDI devices only)",
                        action="store_true")

    parser.add_argument("-cl", "--chip-erase-locked-device",
                        help="Execute a Chip Erase on a locked device (UPDI devices only)",
                        action="store_true")

    # Parse args
    arguments = parser.parse_args()

    # Setup logging
    setup_logging(user_requested_level=getattr(logging, arguments.verbose.upper()))

    # Call main with args
    return pymcuprog_main.pymcuprog(arguments)


if __name__ == "__main__":
    sys.exit(main())
