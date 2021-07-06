"""
Utility functions for pymcuprog
"""
# Python 3 compatibility for Python 2
from __future__ import print_function

from pyedbglib.protocols.housekeepingprotocol import Jtagice3HousekeepingProtocol
from pyedbglib.protocols.jtagice3protocol import Jtagice3ResponseError
from pyedbglib.protocols.jtagice3protocol import Jtagice3Protocol

from .pymcuprog_errors import PymcuprogNotSupportedError

def read_tool_info(housekeeper):
    """
    Interrogates tool (debugger) for useful info

    :returns: Dictionary with various info about the connected debugger
    """
    dap_info = housekeeper.dap_info()

    # Add alias for serialnumber(==serial)
    dap_info['serialnumber'] = dap_info['serial']

    # Read FW versions
    dap_info['firmware_major'] = housekeeper.get_byte(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_CONFIG,
                                                      Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONFIG_FWREV_MAJ)
    dap_info['firmware_minor'] = housekeeper.get_byte(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_CONFIG,
                                                      Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONFIG_FWREV_MIN)
    dap_info['build'] = housekeeper.get_le16(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_CONFIG,
                                             Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONFIG_BUILD)

    # Read HW revision
    dap_info['hardware_rev'] = housekeeper.get_byte(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_CONFIG,
                                                    Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONFIG_HWREV)

    # Some EDBG versions do NOT have the dap_info 'device' tag populated for non-ARM parts.
    # Sneak in and collect the data from the EDBG config instead
    if dap_info['product'][:4] == 'EDBG' and dap_info['device_name'] == '':
        try:
            # Vendor command
            cmd = bytearray([0x83])
            # Add cnt of '1' element:
            cmd.append(1)
            # Add tag of 'TARGET DEVICE NAME'
            cmd.append(0x04)
            # Add dummy 'param'
            cmd.append(ord('?'))
            # Add the offset
            offset = 0
            cmd.extend([offset & 0xFF, offset >> 8])

            # Add the chunk size
            numbytes = 32
            cmd.extend([numbytes & 0xFF, numbytes >> 8])

            # raw command routed via the HK interface
            response = housekeeper.dap_command_response(cmd)
            dap_info['device_name'] = response[6:6 + numbytes].split(b'\0')[0].decode()
        except: #pylint: disable=bare-except
            # resort to ''
            pass
    return dap_info

def print_tool_info(info):
    """
    Print out various tool information

    :param info: Dictionary with various tool info as returned from read_tool_info()
    """
    print("Connected to {0:s} from {1:s} (serial number {2:s})".format(info['product'], info['vendor'],
                                                                       info['serial']))

    print("Debugger firmware version {0:d}.{1:d}.{2:d}".format(info['firmware_major'],
                                                               info['firmware_minor'],
                                                               info['build']))

    print("Debugger hardware revision {0:d}".format(info['hardware_rev']))

def read_target_voltage(housekeeper):
    """
    Read target voltage

    :param housekeeper: instance of pyedbglib.protocols.housekeepingprotocol.Jtagice3HousekeepingProtocol
    """
    return read_voltage_parameter(housekeeper, Jtagice3HousekeepingProtocol.HOUSEKEEPING_ANALOG_VTREF)

def read_supply_voltage_setpoint(housekeeper):
    """
    Read supply setpoint

    :param housekeeper: instance of pyedbglib.protocols.housekeepingprotocol.Jtagice3HousekeepingProtocol
    """
    return read_voltage_parameter(housekeeper, Jtagice3HousekeepingProtocol.HOUSEKEEPING_TSUP_VOLTAGE)

def read_usb_voltage(housekeeper):
    """
    Read USB voltage

    :param housekeeper: instance of pyedbglib.protocols.housekeepingprotocol.Jtagice3HousekeepingProtocol
    """
    return read_voltage_parameter(housekeeper, Jtagice3HousekeepingProtocol.HOUSEKEEPING_ANALOG_VUSB)

def read_voltage_parameter(housekeeper, offset):
    """
    Generic read voltage from tool parameter

    :param housekeeper: Instance of pyedbglib.protocols.housekeepingprotocol.Jtagice3HousekeepingProtocol
    :param offset: Tool parameter offset to read
    """
    housekeeper.start_session()
    voltage = housekeeper.get_le16(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG, offset)
    voltage = voltage / 1000.0
    housekeeper.end_session()
    return voltage

def set_supply_voltage_setpoint(housekeeper, voltage):
    """
    Set supply setpoint

    :param housekeeper: Instance of pyedbglib.protocols.housekeepingprotocol.Jtagice3HousekeepingProtocol
    :param voltage: New setpoint for target supply
    """
    try:
        housekeeper.get_le16(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
                             Jtagice3HousekeepingProtocol.HOUSEKEEPING_TSUP_VOLTAGE)
    except Jtagice3ResponseError:
        raise PymcuprogNotSupportedError("Connected debugger/board does not have supply voltage capability.")

    setpoint_mv = int(voltage*1000)

    try:
        housekeeper.set_le16(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
                             Jtagice3HousekeepingProtocol.HOUSEKEEPING_TSUP_VOLTAGE, setpoint_mv)
    # Unfortunately pyedbglib only throws a generic Exception in this case so no specific Exceptions can be caught
    # See DSG-1494
    #pylint: disable=broad-except
    except Exception as error:
        if "failure code 0x{:02x}".format(Jtagice3Protocol.SETGET_FAILURE_INVALID_VALUE) in str(error).lower():
            raise ValueError("Specified voltage out of range!")
        # Voltage was within range but something else went wrong.  Just forward the exception.
        raise

def compare(data0, data1, offset, verify_mask=None):
    """
    Compares the two byte arrays

    :param data0: first array for compare
    :param data1: second array for compare
    :param offset: address offset in the memory area, for printing
    :param verify_mask: compare mask (for varying instruction width)
    :return:
    """
    if verify_mask is None:
        verify_mask = [0xFF]

    # Check first that lengths match
    if len(data0) != len(data1):
        raise ValueError("Length mismatch on verify, expect 0x{:04X} but got 0x{:04X}".format(len(data0),len(data1)))

    mask_len = len(verify_mask)

    for i in range(0, len(data0), mask_len):
        for dat in range(0, mask_len):
            if (data0[i+dat] & verify_mask[dat]) != (data1[i+dat] & verify_mask[dat]):
                raise ValueError("Verify mismatch starting at location 0x{:06X}: 0x{:02X} vs 0x{:02X}".
                                 format(i+dat+offset, data0[i+dat] & verify_mask[dat], data1[i+dat] & verify_mask[dat]))


def showdata(data, address=0, page_size=None, line_wrap=16):
    """
    Show (print) the data

    :param data: an array/list of data to show
    :param address: byte address to data
    :param page_size: page size in bytes
    :param line_wrap: how many bytes to print per line
    """

    # Cannot print more per line than the page size
    if page_size is not None:
        if line_wrap > page_size:
            line_wrap = page_size

    print("-"*(line_wrap*3+9))

    # Page alignment
    rows = 0
    if page_size is not None:
        page = address % page_size
        rows = int(page / line_wrap)
        for row in range(rows):
            print("0x{0:06X}: ".format(address-page+row*line_wrap), end='')
            print("xx "*line_wrap, end='')
            print("")

    # Calculate offset from aligned data
    div = address % line_wrap

    print("0x{0:06X}: ".format(address-div), end='')
    # Add some empty bytes
    print("xx "*div, end='')

    # keep track of page wraps
    wrap = False

    for i, value in enumerate(data, div+1):
        print("{0:02X} ".format(value), end='')
        if page_size is not None:
            if (i+(rows*line_wrap)) % page_size == 0 and i != len(data)+div:
                print("")
                wrap = True
        if i % line_wrap == 0 and i != len(data)+div or wrap:
            print("")
            print("0x{0:06X}: ".format(address-div + i), end='')
            wrap = False

    # Figure out how many extra empty data positions to print
    extra = line_wrap - div - (len(data) % line_wrap)
    if extra % line_wrap == 0:
        extra = 0

    print("xx "*extra, end='')
    print("")
    print("-"*(line_wrap*3+9))


def pagealign(data, address, page_size, data_size=1):
    """
    Aligns data to the start of a page
    """
    # Pre-pad the data if it does not start at the start of a page
    offset = address % page_size
    for _ in range(offset):
        data.insert(0, 0xFF)
    # In case of other data sizes, post-pad the data
    while len(data) % data_size:
        data.append(0xFF)

    return data, address-offset

def pad_to_size(memory_block, chunk_size, pad_value):
    """
    Pads a chunk of memory
    """
    while len(memory_block) % chunk_size > 0:
        memory_block.append(pad_value)


def enum(**enums):
    """
    Emulates an Enum type

    Needed for Python 2.7 compatibility as Python did not get built-in support for enums until version 3.4
    """
    return type('Enum', (), enums)


def verify_flash_from_bin(bin_filename, backend, offset=0, max_read_chunk=None):
    """
    Verify the contents of flash against a bin-file

    :param filename: Name/path of bin-file to verify
    :param backend: Reference the Backend class of pymcuprog
    :param offset: Memory offset to start verify from
    :returns: Boolean value indicating success or failure of the operation
    """
    bin_file = open(bin_filename, 'rb')
    bin_data = bytearray()
    for line in bin_file.readlines():
        bin_data.extend(line)

    verify_status = backend.verify_memory(bin_data, 'flash', offset, max_read_chunk=max_read_chunk)
    if verify_status is False:
        return False
    return True
