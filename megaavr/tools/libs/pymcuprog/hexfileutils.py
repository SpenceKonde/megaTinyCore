"""
Module providing read and write functionality towards hex files with data intended for target device memories
"""
import copy
import os
from array import array
from collections import namedtuple
from intelhex import IntelHex
try:
  from pathlib import Path
except ImportError:
  from pathlib2 import Path  # python 2 backport

from .deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys, DeviceInfoKeys

def write_memories_to_hex(filename, memory_segments):
    """
    Write a collection of memory segments to a hex file

    Each segment will be written from relative offset 0 (i.e. start of each memory segment)
    :param filename: Name/path of hex file to write to
    :param memory_segments: list of namedtuples with two fields: data and memory_info.  data contains a
        byte array of raw data bytes and memory_info is a dictionary with memory information as defined
        in deviceinfo.deviceinfo.DeviceMemoryInfo.
    """
    hexfile = IntelHex()

    for memory_segment in memory_segments:
        _add_data_to_hex(hexfile, memory_segment.data, memory_segment.memory_info)

    _write_hex_to_file(hexfile, filename)

def write_memory_to_hex(filename, memory_segment, offset):
    """
    Write one memory segment to a hex file with data starting at relative offset given by offset parameter.

    :param filename: Name/path of hex file to write to
    :param memory_segment: namedtuple with two fields: data and memory_info.  data contains a byte array
        of raw data bytes and memory_info is a dictionary with memory information as defined in
        deviceinfo.deviceinfo.DeviceMemoryInfo).
    :param offset: Relative offset for the data within the memory segment
    """
    hexfile = IntelHex()

    _add_data_to_hex(hexfile, memory_segment.data, memory_segment.memory_info, offset)

    _write_hex_to_file(hexfile, filename)

def read_memories_from_hex(filename, device_memory_info):
    """
    Read the content of a hexfile

    :param filename: Name/path of hex file to read from
    :param device_memory_info: DeviceMemoryInfo instance for the device the hex file is intended for
    :returns: list of namedtuples with three fields: data, offset and memory_info.  data contains a byte array
        of raw data bytes, offset is the start address within the memory the data starts at and memory_info
        is a dictionary with the memory info as defined in pymcuprog.deviceinfo.deviceinfo
    """
    hexfile = IntelHex()
    hexfile.fromfile(filename, format='hex')

    memory_segments = []
    for segment in hexfile.segments():
        start = segment[0]
        stop = segment[1]

        subsegment_start = start
        subsegment_stop = start
        while  subsegment_stop < stop:
            current_memory_info = device_memory_info.memory_info_by_address(subsegment_start,
                                                                            DeviceMemoryInfoKeys.HEXFILE_ADDRESS,
                                                                            DeviceMemoryInfoKeys.HEXFILE_SIZE)

            if current_memory_info is None:
                raise IndexError(
                    "Hexfile contains data at hex address 0x{:X} which is outside any memory".format(subsegment_start))

            current_hexfile_address = current_memory_info[DeviceMemoryInfoKeys.HEXFILE_ADDRESS]
            current_hexfile_size = current_memory_info[DeviceMemoryInfoKeys.HEXFILE_SIZE]
            subsegment_stop = current_hexfile_address + current_hexfile_size
            if stop < subsegment_stop:
                # Reached end of segment
                subsegment_stop = stop
            memory_tuple = namedtuple('MemorySegment', 'data offset memory_info')

            data = hexfile.tobinarray(start=subsegment_start, end=subsegment_stop - 1)
            current_size = current_memory_info[DeviceMemoryInfoKeys.SIZE]
            if current_hexfile_size == current_size*2:
                # There are phantom bytes in the hexfile (PIC16 EEPROM), so every 2nd byte should be removed
                data = remove_phantom_bytes(data)

            memory_tuple.data = data
            memory_tuple.memory_info = current_memory_info
            memory_tuple.offset = subsegment_start - current_hexfile_address

            memory_segments.append(copy.deepcopy(memory_tuple))

            subsegment_start = subsegment_stop

    return memory_segments


def verify_flash_from_hex(hex_filename, backend):
    """
    Verify the contents of flash against a hex-file

    :param filename: Name/path of hex-file to verify
    :param device_memory_info: DeviceMemoryInfo instance for the device the hex file should be verified against
    :param backend: Reference to the Backend class of pymcuprog
    :returns: Boolean value indicating success or failure of the operation
    """
    hexfile = IntelHex(hex_filename)
    segments = hexfile.segments()

    for i in range(len(segments)):
        segment_data = []
        for j in range(segments[i][1]-segments[i][0]):
            segment_data.append(hexfile[segments[i][0]+j])

        verify_status = backend.verify_memory(segment_data, 'flash', segments[i][0])
        if verify_status is False:
            return False
    return True


def remove_phantom_bytes(data):
    """
    Remove every 2nd byte from the data
    """
    data_stripped = []
    for index in range(0, len(data), 2):
        data_stripped.append(data[index])
    # Make a bin array out of the data list to be consistent with the data format of
    # the data fetched directly from the hex file
    data_stripped_binarray = array('B')
    data_stripped_binarray.fromlist(data_stripped)
    return data_stripped_binarray

def _add_data_to_hex(intelhex, data, memory_info, offset=0):
    """
    Add given data starting at relative index offset to IntelHex instance intelhex

    :param intelhex: IntelHex object
    :param data: raw data bytes
    :param memory_info: memory info as provided by pymcuprog.deviceinfo.deviceinfo
    :param offset: relative offset within the memory
    """
    hexfile_address_key = DeviceMemoryInfoKeys.HEXFILE_ADDRESS
    hexfile_size_key = DeviceMemoryInfoKeys.HEXFILE_SIZE
    size_key = DeviceMemoryInfoKeys.SIZE
    name = memory_info[DeviceInfoKeys.NAME]

    if offset+len(data) > memory_info[hexfile_size_key]:
        raise IndexError(
            "Attempting to write outside boundary of {} memory ({} bytes starting at offset {})".format(name,
                                                                                                        len(data),
                                                                                                        offset))

    hex_offset = memory_info[hexfile_address_key] + offset
    if memory_info[hexfile_size_key] == memory_info[size_key]*2:
        # Hex file should contain one phantom byte per data byte in the hex file (PIC16 EEPROM)
        for i, dat in enumerate(data):
            intelhex[i*2 + hex_offset] = data[i]
            intelhex[i*2 + 1 + hex_offset] = 0 & 0xFF
    else:
        for i, dat in enumerate(data):
            intelhex[i + hex_offset] = dat

def _write_hex_to_file(intelhex, filename):
    """
    Write intelhex object to file.

    Directories will be created if path does not exist
    :param intelhex: IntelHex instance
    :param filename: Name/path to write intelhex object to
    """
    directory = os.path.dirname(filename)
    if directory != '' and not os.path.exists(directory):
        Path(directory).mkdir(exist_ok=True, parents=True)

    intelhex.write_hex_file(filename)
