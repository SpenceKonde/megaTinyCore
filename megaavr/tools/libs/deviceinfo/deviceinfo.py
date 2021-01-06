"""
deviceinfo.py
A simple Device Information service

Device information is stored in files named <devicename>.py in the devices sub-folder
Each device file contains a dict of values
These device files are [ideally] generated from DFP information by [running generate_device_info.py | hand]
"""
# Python 3 compatibility for Python 2
from __future__ import print_function

import os
import importlib

from logging import getLogger

from pymcuprog.pymcuprog_errors import PymcuprogError
from .memorynames import MemoryNames
from .deviceinfokeys import DeviceMemoryInfoKeys, DeviceInfoKeys, DeviceInfoKeysPic

def getdeviceinfo(devicename):
    """
    Looks up device info for a given part

    :param devicename: device to look up
    :return: device information dict
    """
    logger = getLogger(__name__)
    logger.info("Looking for device %s", devicename)

    devicename = devicename.lower()

    try:
        device_module = importlib.import_module("deviceinfo.devices.{}".format(devicename))
    except ImportError:
        try:
            # When pymcuprog is used as a package in other scripts
            # the deviceinfo module is part of the pymcuprog package
            device_module = importlib.import_module("pymcuprog.deviceinfo.devices.{}".format(devicename))
        except ImportError:
            device_module = importlib.import_module("{}".format(devicename))

    device_info = getattr(device_module, "DEVICE_INFO")

    # For PIC devices there will be a default_bulk_erase_address outside any memory information
    # This address needs to be converted to byte address
    default_bulk_erase_address_byte = None
    for param in device_info:
        if param.startswith(DeviceInfoKeysPic.DEFAULT_BULK_ERASE_ADDRESS):
            # Check if it's word or byte oriented data
            mul = DeviceMemoryInfo.bytes_or_words(param)
            if mul is not None:
                default_bulk_erase_address_byte = int(device_info[param] * mul)
            else:
                default_bulk_erase_address_byte = device_info[param]

    if default_bulk_erase_address_byte is not None:
        device_info[DeviceInfoKeysPic.DEFAULT_BULK_ERASE_ADDRESS] = default_bulk_erase_address_byte

    return device_info

def get_supported_devices():
    """
    Return a list of all supported devices

    A device is supported if it has a device model file in the devices folder
    """
    root_folder = os.path.dirname(os.path.abspath(__file__))
    dir_list = os.listdir(root_folder + "//devices")
    ignore_list = ['__init__.py']
    device_list = []
    for devicefile in dir_list:
        if devicefile.endswith(".py") and devicefile not in ignore_list:
            devicename = devicefile.split('.')[0]
            device_list.append(devicename)

    return device_list

class DeviceMemoryInfo:
    """
    API to fetch information about device memory segments
    """
    def __init__(self, device_info):
        self.device = device_info
        self.memtypes = MemoryNames.get_all()

        # hexfile_address is the start address for the memory segment in hex files.
        # PIC and ARM devices usually does not need the parameter as all locations are mapped in a single address space.
        # AVR8 devices does not map all memory types in a single address space.
        # Memory types have defined offsets in hex files as defined below
        self.avr8_hex_file_offsets = {
            MemoryNames.FLASH: 0x000000,
            MemoryNames.EEPROM: 0x810000,
            MemoryNames.FUSES: 0x820000,
            MemoryNames.LOCKBITS: 0x830000,
            MemoryNames.SIGNATURES: 0x840000,
            MemoryNames.USER_ROW: 0x850000
        }

        # erase_address is the address for the erase of the memory.
        # Note that for PIC devices other memories might be erased in the same operation depending on the target,
        # see the programming spec for the target device.

        # erase_address, hexfile_address, hexfile_size and verify mask are optional in the device models.
        #   erase_address will be set to the memory address if it's missing.
        # Hex file address will be set to the memory address if it's missing, unless it's an AVR device where
        #   the hex file offset is used instead.
        # Hex file size will be set to the memory size if it's missing except for EEPROM on PIC16 devices where
        #   the hex file will contain phantom bytes so the hex file will contain twice as many EEPROM bytes as
        #   the actual EEPROM in the device
        # verify_mask is set based on architecture
        self.paramtypes = [DeviceMemoryInfoKeys.ADDRESS,
                           DeviceMemoryInfoKeys.SIZE,
                           DeviceMemoryInfoKeys.PAGE_SIZE,
                           DeviceMemoryInfoKeys.WRITE_SIZE,
                           DeviceMemoryInfoKeys.READ_SIZE,
                           DeviceMemoryInfoKeys.VERIFY_MASK,
                           DeviceMemoryInfoKeys.ERASE_ADDRESS,
                           DeviceMemoryInfoKeys.HEXFILE_ADDRESS,
                           DeviceMemoryInfoKeys.HEXFILE_SIZE,
                           DeviceMemoryInfoKeys.CHIPERASE_EFFECT,
                           DeviceMemoryInfoKeys.ISOLATED_ERASE]

        self.mem_by_name = {}

        # Find information about memory segments
        for param in self.device:
            for mtype in self.memtypes:
                # Does this line describe a memory location?
                if param.startswith(mtype):
                    self._configure_memory_param(mtype, param)

        # erase_address and hexfile_address are optional and should default to the value of the address parameter
        optional_params = [DeviceMemoryInfoKeys.VERIFY_MASK,
                           DeviceMemoryInfoKeys.HEXFILE_ADDRESS,
                           DeviceMemoryInfoKeys.ERASE_ADDRESS,
                           DeviceMemoryInfoKeys.HEXFILE_SIZE]
        for optional_param in optional_params:
            for memtype in self.mem_by_name:
                if optional_param not in self.mem_by_name[memtype]:
                    # Set the verify mask based on architecture
                    if optional_param == DeviceMemoryInfoKeys.VERIFY_MASK:
                        verify_mask = self._get_verify_mask(self.device[DeviceInfoKeys.ARCHITECTURE], memtype)
                        self.mem_by_name[memtype][optional_param] = verify_mask
                    # Set the hexfile_address
                    elif optional_param == DeviceMemoryInfoKeys.HEXFILE_ADDRESS:
                        self._add_hexfile_address(memtype, optional_param)
                    # Set the hexfile_size
                    elif optional_param == DeviceMemoryInfoKeys.HEXFILE_SIZE:
                        self._add_hexfile_size(memtype, optional_param)
                    # Set the erase_address
                    elif optional_param == DeviceMemoryInfoKeys.ERASE_ADDRESS:
                        # By default the erase_address is the same as the address of the memory
                        address = self.mem_by_name[memtype][DeviceMemoryInfoKeys.ADDRESS]
                        self.mem_by_name[memtype][optional_param] = address

    def _configure_memory_param(self, memorytype, param):
        # Check if it's word or byte oriented data
        mul = self.bytes_or_words(param)
        # Create a dict for the memory type if it does not exist
        if not self.mem_by_name.get(memorytype):
            self.mem_by_name[memorytype] = {DeviceMemoryInfoKeys.NAME: memorytype}
        # Parse and store parameter
        for ptype in self.paramtypes:
            if param.startswith("{}_{}".format(memorytype, ptype)):
                if mul is not None:
                    self.mem_by_name[memorytype][ptype] = int(self.device[param] * mul)
                else:
                    self.mem_by_name[memorytype][ptype] = self.device[param]

    def _add_hexfile_address(self, memorytype, paramname):
        # Inject hex file addresses for AVR memory areas
        if self.device[DeviceInfoKeys.ARCHITECTURE].startswith('avr8'):
            if memorytype in self.avr8_hex_file_offsets:
                self.mem_by_name[memorytype][paramname] = self.avr8_hex_file_offsets[memorytype]
            else:
                # The hexfile_address for memory types that doesn't make sense in a hex file like SRAM
                # and regular I/O space is defined to an address the other memory types will not reach
                self.mem_by_name[memorytype][paramname] = 0xFFFFFF
        # All other memory types are mapped 1 to 1 in the hex file
        else:
            self.mem_by_name[memorytype][paramname] = self.mem_by_name[memorytype][DeviceMemoryInfoKeys.ADDRESS]

    def _add_hexfile_size(self, memorytype, paramname):
        if self.device[DeviceInfoKeys.ARCHITECTURE].startswith('PIC16') and memorytype == MemoryNames.EEPROM:
            # For PIC16 devices there will be one phantom byte in the hex file for each EEPROM byte, so
            # the size of EEPROM in a hex file will be twice the size of the actual EEPROM memory
            self.mem_by_name[memorytype][paramname] = self.mem_by_name[memorytype][DeviceMemoryInfoKeys.SIZE] * 2
        else:
            self.mem_by_name[memorytype][paramname] = self.mem_by_name[memorytype][DeviceMemoryInfoKeys.SIZE]

    @staticmethod
    def _get_verify_mask(architecture, memtype):
        # byte oriented memory
        mask = [0xFF]

        # PIC16 is word addressed and has 14-bit flash, except EEPROM which is byte oriented
        if architecture == 'PIC16' and memtype not in [MemoryNames.EEPROM]:
            mask = [0xFF, 0x3F]

        # PIC18 is word addressed and has 16-bit flash, except EEPROM which is byte oriented
        elif architecture == 'PIC18' and memtype not in [MemoryNames.EEPROM]:
            mask = [0xFF, 0xFF]

        # PIC24 is word addressed and has 24-bit flash, except EEPROM which is word oriented
        elif architecture == 'PIC24':
            if memtype in [MemoryNames.EEPROM]:
                mask = [0xFF, 0xFF]
            else:
                mask = [0xFF, 0xFF, 0xFF, 0x00]

        return mask

    @staticmethod
    def bytes_or_words(address_param):
        """
        Return multiplier for address parameter

        The returned multiplier can be used to convert the address parameter to byte address
        :param address_param: Address parameter (used as key in device info dict)
        :return: Multiplier to convert the address to byte address
        """
        if address_param.endswith("_byte") or address_param.endswith("_bytes"):
            mul = 1
        elif address_param.endswith("_word") or address_param.endswith("_words"):
            mul = 2
        else:
            mul = None
        return mul

    def memory_info_by_address_range(self,
                                     start,
                                     stop,
                                     address_type=DeviceMemoryInfoKeys.ADDRESS,
                                     size_type=DeviceMemoryInfoKeys.SIZE):
        """
        Returns a list of all memories applicable for the address range(start, stop)

        :param start: Start address (byte)
        :param stop: End address (byte)
        :param address_type: Selects between normal addresses and addresses used in hex files
            (address vs hexfile_address)
        :param size_type: Selects between normal size and size used in hexfiles (size vs hexfile_size)
        """
        # We do not support negative memory ranges
        if start > stop:
            raise PymcuprogError("Cannot parse reverse memory range {} to {}".format(start, stop))

        memtypes = []

        # Loop through all known memory types for this device
        for memtype in self.mem_by_name:
            address = self.mem_by_name[memtype][address_type]
            size = self.mem_by_name[memtype][size_type]

            # Check if any of the addresses between start and stop is within the memory type range
            if start < address+size and stop > address:
                memtypes.append(self.mem_by_name[memtype])
        return memtypes

    def memory_info_by_address(self,
                               byte_address,
                               address_type=DeviceMemoryInfoKeys.ADDRESS,
                               size_type=DeviceMemoryInfoKeys.SIZE):
        """
        Returns information about the memory type for a given byte address

        :param byte_address: Memory address to check
        :param address_type: Selects between normal addresses and addresses used in hex files
            (ADDRESS vs HEXFILE_ADDRESS)
        :param size_type: Selects between normal size and size used in hexfiles (size vs hexfile_size)
       """
        memtype = None
        for memory in self.mem_by_name:
            if self.mem_by_name[memory][address_type] <= byte_address < \
                    self.mem_by_name[memory][address_type] + self.mem_by_name[memory][size_type]:
                if memtype is not None:
                    raise PymcuprogError("Duplicate memory area found for byte address '{}'".format(byte_address))
                memtype = self.mem_by_name[memory]
        return memtype

    def memory_info_by_name(self, name):
        """
        Returns information about the requested memory
        """
        memory = self.mem_by_name.get(name)
        if not memory:
            message = "Memory type '{}' not defined for device '{}'".format(name, self.device[DeviceInfoKeys.NAME])
            raise ValueError(message)
        return memory
