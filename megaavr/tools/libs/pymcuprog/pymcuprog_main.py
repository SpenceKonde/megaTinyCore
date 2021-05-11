"""
Python MCU programmer, CLI main program
"""
# Python 3 compatibility for Python 2
from __future__ import print_function

# utils
import time
import os
from copy import copy

from .backend import Backend, SessionConfig
from .toolconnection import ToolUsbHidConnection, ToolSerialConnection
from .deviceinfo.memorynames import MemoryNameAliases, MemoryNames
from .deviceinfo.eraseflags import ChiperaseEffect
from .deviceinfo.deviceinfo import get_supported_devices
from .deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys

from .utils import print_tool_info, showdata, verify_flash_from_bin, compare
from .hexfileutils import write_memories_to_hex, write_memory_to_hex, read_memories_from_hex, verify_flash_from_hex
from .pymcuprog_errors import PymcuprogNotSupportedError, PymcuprogSessionConfigError, \
    PymcuprogToolConnectionError, PymcuprogDeviceLockedError

try:
    from .version import VERSION, BUILD_DATE, COMMIT_ID
except ImportError:
    VERSION = "0.0.0"
    COMMIT_ID = "N/A"
    BUILD_DATE = "N/A"

STATUS_SUCCESS = 0
STATUS_FAILURE = 1
STATUS_FAILURE_LOCKED = 2

# Only include memories that can be written when writing memories to hex file
WRITE_TO_HEX_MEMORIES = [MemoryNames.EEPROM, MemoryNames.FLASH, MemoryNames.FUSES, MemoryNames.CONFIG_WORD]


def pymcuprog(args):
    """
    Main program
    """
    if args.version or args.release_info:
        print("pymcuprog version {}".format(VERSION))
        if args.release_info:
            print("Build date: {}".format(BUILD_DATE))
            print("Commit ID:  {}".format(COMMIT_ID))
        return STATUS_SUCCESS

    backend = Backend()

    toolconnection = _setup_tool_connection(args)

    try:
        backend.connect_to_tool(toolconnection)
    except PymcuprogToolConnectionError as error:
        print(error)
        return STATUS_FAILURE

    status = None
    if args.tool not in ['uart']:
        # This section can initialise all features requiring non-UART transports

        # DAP info only available on native CMSIS-DAP
        dap_info = backend.read_tool_info()
        print_tool_info(dap_info)

        # Targetless actions, only available on HID tools
        status = _debugger_actions(backend, args)

        if status is not None:
            backend.disconnect_from_tool()
            return status

    device_selected = _select_target_device(backend, args)
    if device_selected is None:
        backend.disconnect_from_tool()
        return STATUS_FAILURE

    status = _start_session(backend, device_selected, args)
    if status != STATUS_SUCCESS:
        backend.disconnect_from_tool()
        return status

    # -x timer argument
    time_start = None
    if args.timing:
        print("Starting timer")
        time_start = time.time()

    _programming_actions(backend, args)

    backend.end_session()
    backend.disconnect_from_tool()
    if args.timing:
        time_stop = time.time()
        print("Operation took {0:.03f}s".format(time_stop - time_start))

    print("Done.")

    return status


def _action_getvoltage(backend):
    voltage = backend.read_target_voltage()
    print("Measured voltage: {0:0.2f}V".format(voltage))
    return STATUS_SUCCESS


def _action_getsupplyvoltage(backend):
    voltage = backend.read_supply_voltage_setpoint()
    print("Supply voltage set to {0:0.2f}V".format(voltage))
    return STATUS_SUCCESS


def _action_getusbvoltage(backend):
    voltage = backend.read_usb_voltage()
    print("USB voltage is {0:0.2f}V".format(voltage))
    return STATUS_SUCCESS


def _action_setsupplyvoltage(backend, literal):
    voltage = backend.read_supply_voltage_setpoint()
    print("Supply voltage is currently set to {0:0.2f}V".format(voltage))
    if literal is None:
        print("Specify voltage in Volts using -l <literal>")
    else:
        setvoltage = literal[0]
        if setvoltage == voltage:
            print("Voltage is already right where you want it.")
        else:
            print("Setting supply voltage to {0:0.2f}V".format(setvoltage))
            backend.set_supply_voltage_setpoint(setvoltage)
            voltage = backend.read_supply_voltage_setpoint()
            print("Supply voltage is now set to {0:0.2f}V".format(voltage))

            # Static delay to let the target voltage settle before reading it out
            # Alternatively a retry loop could be used, but it is difficult to know when to terminate
            # the loop as sometimes the final voltage is not known, for example if setting the voltage
            # to 5.5V the actual voltage will depend upon the USB voltage.  If the USB voltage is only
            # 4.9V the target voltage will never reach more than 4.9V
            time.sleep(0.5)
            voltage = backend.read_target_voltage()
            print("Measured voltage: {0:0.2f}V".format(voltage))
    return STATUS_SUCCESS


def _action_reboot_debugger(backend):
    print("Rebooting tool...")
    backend.reboot_tool()
    return STATUS_SUCCESS


def _action_ping(backend):
    print("Pinging device...")
    response = backend.read_device_id()
    idstring = ''
    for idbyte in response:
        idstring = '{:02X}'.format(idbyte) + idstring
    print("Ping response: {}".format(idstring))
    return STATUS_SUCCESS


def _action_erase(backend, args):
    if args.memory is None or args.memory == MemoryNameAliases.ALL:
        print("Chip/Bulk erase,")
        for memname in MemoryNames.get_all():
            try:
                effect = backend.get_chiperase_effect(memname)
            except ValueError:
                # This memory type does not exist for this device, just continue
                continue
            else:
                if effect != ChiperaseEffect.NOT_ERASED:
                    print("Memory type {} is {}".format(memname, effect))
        print("...")
    else:
        if backend.is_isolated_erase_possible(args.memory):
            print("Erasing {}...".format(args.memory))
        else:
            print("ERROR: {} memory can't be erased or "
                  "can't be erased without affecting other memories".format(args.memory))
            chiperase_effect = backend.get_chiperase_effect(args.memory)
            if chiperase_effect != ChiperaseEffect.NOT_ERASED:
                print("{} memory is {} by a chip/bulk erase".format(args.memory, chiperase_effect))
                print("Use erase without -m option to erase this memory")
            return STATUS_FAILURE

    backend.erase(args.memory, address=None)
    print("Erased.")
    return STATUS_SUCCESS


def _action_read(backend, args):
    # Reading with bytes argument requires that memory type is specified
    if args.bytes != 0 and args.memory == MemoryNameAliases.ALL:
        print("Memory area must be specified when number of bytes is specified.")
        return STATUS_FAILURE

    print("Reading...")
    result = backend.read_memory(args.memory, args.offset, args.bytes)

    # If a filename is specified, write to it
    hexfile = False
    binary = False
    filepath = None
    if args.filename is not None:
        filepath = os.path.normpath(args.filename)
        prefix, postfix = _get_file_prefix_and_postfix(filepath)
        # If it ends in hex, use intel hex format, else binary
        if postfix == 'hex':
            hexfile = True
        else:
            binary = True

    # Print the data or save it to a file
    if hexfile:
        if args.memory == MemoryNameAliases.ALL:
            # Only memories that can be written should go into the hex file
            result_to_write = _extract_writeable_memories(result)
            write_memories_to_hex(filepath, result_to_write)
        else:
            write_memory_to_hex(filepath, result[0], args.offset)
        print("Data written to hex file: '{0:s}'".format(filepath))
    elif binary:
        for item in result:
            memory_name = item.memory_info[DeviceMemoryInfoKeys.NAME]
            data = item.data
            filepath = "{}_{}.{}".format(prefix, memory_name, postfix)
            # Binary files does not have addressing, and needs a split on memory type
            with open(filepath, "wb") as binfile:
                binfile.write(data)
            print("Data written to binary file: '{0:s}'".format(filepath))
    else:
        for item in result:
            memory_info = item.memory_info
            print("Memory type: {}".format(memory_info[DeviceMemoryInfoKeys.NAME]))
            showdata(item.data,
                     args.offset + memory_info[DeviceMemoryInfoKeys.ADDRESS],
                     memory_info[DeviceMemoryInfoKeys.PAGE_SIZE])
            print("\n")

    return STATUS_SUCCESS


def _action_verify(backend, args):
    hexfile = False
    binary = False
    literal = False
    filepath = None
    if args.filename is not None:
        filepath = os.path.normpath(args.filename)
        _, postfix = _get_file_prefix_and_postfix(filepath)
        # If it ends in hex, use intel hex format, else binary
        if postfix == 'hex':
            hexfile = True
        else:
            binary = True
    if args.literal is not None:
        literal = True
        if args.filename is not None:
            print("Both file and literal value was specified. Literal verify will be ignored in favor of file verify")
            literal = False

    if hexfile:
        print("Verifying...")
        verify_status = verify_flash_from_hex(args.filename, backend)
        if verify_status is True:
            print("Verify successful. Data in flash matches data in specified hex-file")
    elif binary:
        print("Verifying...")
        verify_status = verify_flash_from_bin(args.filename, backend, args.offset)
        if verify_status is True:
            print("Verify successful. Data in flash matches data in specified bin-file")
    elif literal:
        print("Verifying...")
        flash_data = backend.read_memory('flash', args.offset, len(args.literal))[0].data
        compare(flash_data, args.literal, args.offset)
        print("Verify successful. Data in flash matches literal data specified")
    else:
        raise Exception('No file or literal specified for verify')

    return STATUS_SUCCESS


def _get_file_prefix_and_postfix(filepath):
    """
    Get file prefix and postfix from the filepath

    If the file name in the filepath has not file extension the file is supposed to be a binary file
    :param filepath: File name and full path
    :return: prefix, postfix
    """
    prefix = filepath.split('.')[0]
    postfix = filepath.split('.')[-1].lower()
    # If no "." is found in the filepath
    if postfix == prefix:
        postfix = "bin"

    return prefix, postfix


def _extract_writeable_memories(memory_segments):
    """
    Take a list of memory segments and return the segments that can be written

    :param memory_segments: List of namedtuples with two fields: data and memory_info.  data contains a byte array of
            raw data bytes and memory_info is a dictionary with memory information (as defined in
            deviceinfo.deviceinfo.DeviceMemoryInfo).
    :return: List of namedtuples (a subset of the memory_segments input parameter) only containing memory segments
        that can be written
    """
    writeable_segments = []
    for segment in memory_segments:
        if segment.memory_info[DeviceMemoryInfoKeys.NAME] in WRITE_TO_HEX_MEMORIES:
            writeable_segments.append(segment)
    return writeable_segments


def _action_write(backend, args):
    # If a filename is specified, read from it
    if args.filename is not None:
        filepath = os.path.normpath(args.filename)
        _, postfix = _get_file_prefix_and_postfix(filepath)
        # If it ends in hex, use intel hex format, else binary
        if postfix == 'hex':
            # Hexfiles contain addressing information that cannot be remapped, so offset/memory are not allowed here
            if args.offset:
                print("Offset cannot be specified when writing hex file")
                return STATUS_FAILURE

            if args.memory != MemoryNameAliases.ALL:
                print("Memory area cannot be specified when writing hex file")
                return STATUS_FAILURE

            result = read_memories_from_hex(args.filename, backend.device_memory_info)

            print("Writing from hex file...")

            if args.blocksize:
                _write_memory_segments(backend, result, args.verify, blocksize=args.blocksize)
            else:
                _write_memory_segments(backend, result, args.verify)
        else:
            with open(filepath, "rb") as binfile:
                data_from_file = bytearray(binfile.read())

            # Prepare and write data
            print("Writing from binary file...")
            # When writing data to target the data might be pagealigned so we make a copy to avoid verifying
            # more than needed (in case verify option is enabled)
            data_to_write = copy(data_from_file)
            backend.write_memory(data_to_write, args.memory, args.offset)
            if args.verify:
                print("Verifying from binary file...")
                # Verify content, an exception is thrown on mismatch
                backend.verify_memory(data_from_file, args.memory, args.offset)
    elif args.literal:
        # Prepare and write data
        print("Writing literal values...")
        backend.write_memory(bytearray(args.literal), args.memory, args.offset)
        if args.verify:
            print("Verifying literal values...")
            # Verify content, an exception is thrown on mismatch
            backend.verify_memory(bytearray(args.literal), args.memory, args.offset)
    else:
        print("Error: for writing use either -f <file> or -l <literal>")

    return STATUS_SUCCESS


def _write_memory_segments(backend, memory_segments, verify, blocksize = 0):
    """
    Write content of list of memory segments

    :param backend: pymcuprog Backend instance
    :param memory_segments: List of namedtuples with two fields: data and memory_info.  data contains a byte array of
        raw data bytes and memory_info is a dictionary with memory information (as defined in
        deviceinfo.deviceinfo.DeviceMemoryInfo).
    :param verify: If True verify the written data by reading it back and compare
    :param blocksize: this is a signal to write_memory for updiserial when writing flash; if 0 or not supplied
        do not use blocks (equivalent to blocksize == 2 bytes or 1 word). If -1, it will set tje blocksize to
        the page size of the target chip, which can imcrease write speed more than 10:1. Any other number will
        be used as supplied. Even numbers up to the page size are recommended.
        Any other negative number is invalid, and is zero'ed out.
    """
    if blocksize < -1:
        blocksize = 0
    for segment in memory_segments:
        memory_name = segment.memory_info[DeviceMemoryInfoKeys.NAME]
        print("Writing {}...".format(memory_name))
        backend.write_memory(segment.data, memory_name, segment.offset, blocksize=blocksize)
        if verify:
            print("Verifying {}...".format(memory_name))
            verify_ok = backend.verify_memory(segment.data, memory_name, segment.offset)
            if verify_ok:
                print("OK")
            else:
                print("Verification failed!")


def _action_reset(backend):
    backend.hold_in_reset()
    # Wait a bit to make sure the device has entered reset
    # If needed this sleep could be made configurable by a CLI parameter,
    # but for now a hardcoded value is assumed to be sufficient
    time.sleep(0.1)
    backend.release_from_reset()
    return STATUS_SUCCESS


def _debugger_actions(backend, args):
    """
    Debugger related actions

    Targetless actions only involving the debugger.  Only available on HID tools
    """
    status = None
    if args.action == 'getvoltage':
        status = _action_getvoltage(backend)
    if args.action == 'getsupplyvoltage':
        status = _action_getsupplyvoltage(backend)
    if args.action == 'getusbvoltage':
        status = _action_getusbvoltage(backend)
    if args.action == 'setsupplyvoltage':
        status = _action_setsupplyvoltage(backend, args.literal)
    if args.action == 'reboot-debugger':
        status = _action_reboot_debugger(backend)

    return status


def _programming_actions(backend, args):
    status = None
    # Ping: checks that the device is there by reading its ID, or equivalent
    if args.action == "ping":
        status = _action_ping(backend)
    # Erase: perform a full chip erase, or memtype-only erase if specified
    elif args.action == "erase":
        status = _action_erase(backend, args)
    # Reading data:
    elif args.action == "read":
        status = _action_read(backend, args)
    elif args.action == "write":
        status = _action_write(backend, args)
    elif args.action == "reset":
        status = _action_reset(backend)
    elif args.action == "verify":
        status = _action_verify(backend, args)
    else:
        print("Unknown command '{0:s}'".format(args.action))
        status = STATUS_FAILURE

    return status


def _setup_tool_connection(args):
    toolconnection = None

    # Parse the requested tool from the CLI
    if args.tool == "uart":
        # Embedded GPIO/UART tool (eg: raspberry pi) => no USB connection
        toolconnection = ToolSerialConnection(serialport=args.uart)
    else:
        usb_serial = args.serialnumber
        product = args.tool
        if usb_serial and product:
            print("Connecting to {0:s} ({1:s})'".format(product, usb_serial))
        else:
            if usb_serial:
                print("Connecting to any tool with USB serial number '{0:s}'".format(usb_serial))
            elif product:
                print("Connecting to any {0:s}".format(product))
            else:
                print("Connecting to anything possible")
        toolconnection = ToolUsbHidConnection(serialnumber=usb_serial, tool_name=product)

    return toolconnection


def _select_target_device(backend, args):
    device_mounted = None
    device_selected = None
    if args.tool not in ['uart']:
        # Find out from the board (kit) if a device is mounted
        device_mounted = backend.read_kit_device()
        if device_mounted is not None:
            device_mounted = device_mounted.lower()
            print("Device mounted: '{0:s}'".format(device_mounted))

    # Parse device field.  If unspecified, use the board's device
    if args.device:
        device_selected = args.device.lower()
    else:
        if device_mounted is None:
            print("Unable to determine on-board target!  Please specify device using -d <device>")
        else:
            print("No device specified.  Using on-board target ({0:s})".format(device_mounted))
            device_selected = device_mounted

    # Mismatch.  Allow user to proceed at own risk.
    if device_mounted is not None and device_selected != device_mounted:
        print("Warning: you are attempting to use a device which is not the one which was mounted on the kit!")
        print("Cut all straps between the debugger and the on-board target when accessing an external device!")

    return device_selected


def _start_session(backend, device, args):
    """
    Setup the session and try to build the stack for this device
    """
    sessionconfig = SessionConfig(device)

    # -c clock argument
    # allow Hz, or kHz ending in 'k' (eg: 100k) or MHz ending in 'M' eg (1M)
    if args.clk:
        if args.clk[-1] == 'k':
            clk = int(args.clk.strip('k')) * 1000
        elif args.clk[-1] == 'M':
            clk = int(args.clk.strip('M')) * 1000000
        else:
            clk = int(args.clk)

        sessionconfig.interface_speed = clk

    # Translate args into "special_options" to pass down the stack
    sessionconfig.special_options = {}
    if args.high_voltage:
        sessionconfig.special_options['high-voltage'] = args.high_voltage
    if args.user_row_locked_device:
        sessionconfig.special_options['user-row-locked-device'] = args.user_row_locked_device
    if args.chip_erase_locked_device:
        sessionconfig.special_options['chip-erase-locked-device'] = args.chip_erase_locked_device

    # Programming user row on locked parts and erasing to unlock are mutually exclusive
    if args.chip_erase_locked_device and args.user_row_locked_device:
        print("User row cannot be written on a locked device while erasing and unlocking.")
        return STATUS_FAILURE

    if args.interface:
        sessionconfig.interface = args.interface

    if args.packpath:
        sessionconfig.packpath = args.packpath

    status = STATUS_SUCCESS
    try:
        backend.start_session(sessionconfig)
    except PymcuprogDeviceLockedError:
        print("The device is in a locked state and is not accessible; a chip erase is required.")
        print("Locked AVR UPDI devices can:")
        print(" - be unlocked using command: erase --chip-erase-locked-device")
        print(" - write user row values using command: write --user-row-locked-device")
        status = STATUS_FAILURE_LOCKED
    except PymcuprogNotSupportedError:
        print("Unable to setup stack for device {0:s}".format(sessionconfig.device))
        print("Currently supported devices (in 'devices' folder):")
        device_list = get_supported_devices()
        print(', '.join(map(str, device_list)))
        status = STATUS_FAILURE
    except PymcuprogSessionConfigError as error:
        print("Unable to start session: {}".format(error))
        status = STATUS_FAILURE

    return status
