"""
PIC NVM implementation
"""
import os
import sys

from pyedbglib.util import binary

from . import utils
from .nvm import NvmAccessProviderCmsisDapTool
from .pymcuprog_errors import PymcuprogNotSupportedError
from .deviceinfo.memorynames import MemoryNames
from .deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys, DeviceInfoKeys


class NvmAccessProviderCmsisDapPic(NvmAccessProviderCmsisDapTool):
    """
    NVM access the PIC way
    """

    def __init__(self, transport, device_info, packpath, options=""):
        """
        :raises: ImportError if packpath is None
        """
        self.pic = None
        NvmAccessProviderCmsisDapTool.__init__(self, device_info)
        self.options = {}

        if packpath is None:
            raise ImportError("No path to pack repo provided!")

        # Each part pack ships its own version of the full script stack, including pyedbglib.
        # pyedbglib, and other libraries, can be installed in the local python site-packages
        # This path hack puts the part pack path at the front of the python import path
        system_path = sys.path
        sys.path = [os.path.normpath(packpath)] + sys.path
        sys.path = [os.path.normpath(packpath + "//common")] + sys.path

        # Create driver for scripted debuggers
        self.options['skip_blank_pages'] = True
        self.options['overlapped_usb_access'] = False

        # This imports the debugger model from the provided packpath so the import must be late
        from common.debugprovider import provide_debugger_model # pylint: disable=import-outside-toplevel, import-error

        devicename = device_info[DeviceInfoKeys.NAME]
        self.pic = provide_debugger_model(devicename)
        # Start immediately
        self.pic.setup_session(transport, self.options)
        self.device_info = device_info
        # Start the programming session
        if 'pic24' in devicename.lower():
            if 'no_pe' in options:
                # Only PIC24 devices support Programming Executives
                try:
                    # Force no Programming Executive usage by setting program_pe flag but not configure a
                    # PE (i.e. not calling set_program_exec)
                    self.pic.start_programming_operation(program_pe=options['no_pe'])
                except TypeError:
                    # start_programming_operation does not have program_pe argument (i.e. old
                    # devicesupportscripts without PE support)
                    self.pic.start_programming_operation()
            else:
                self.pic.start_programming_operation(program_pe=False)
        else:
            self.pic.start_programming_operation()

        # The stack has been built, revert path hacks
        sys.path = system_path

    def read(self, memory_info, offset, numbytes):
        """
        Read the memory

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset in the memory type
        :param numbytes: number of bytes to read
        :return: array of bytes read
        """
        mem_name = memory_info[DeviceInfoKeys.NAME]
        offset += memory_info[DeviceMemoryInfoKeys.ADDRESS]
        if mem_name in [MemoryNames.FLASH, MemoryNames.USER_ID, MemoryNames.ICD]:
            mem = self.pic.read_flash_memory(offset, numbytes)
            return mem
        if mem_name == MemoryNames.CONFIG_WORD:
            mem = self.pic.read_config_memory(offset, numbytes)
            return mem
        if mem_name == MemoryNames.EEPROM:
            mem = self.pic.read_eeprom_memory(offset, numbytes)
            return mem
        self.logger.error("Unsupported memtype!")
        return []

    def write(self, memory_info, offset, data):
        """
        Write the memory with data

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset within the memory type
        :param data: the data to program
        :return: None
        """
        # Make sure the data is aligned to a memory page
        chunk, address = utils.pagealign(data,
                                         offset,
                                         memory_info[DeviceMemoryInfoKeys.PAGE_SIZE],
                                         memory_info[DeviceMemoryInfoKeys.WRITE_SIZE])

        mem_name = memory_info[DeviceInfoKeys.NAME]
        address += memory_info[DeviceMemoryInfoKeys.ADDRESS]
        if mem_name == MemoryNames.FLASH:
            self.pic.write_flash_memory(address, chunk)
        elif mem_name == MemoryNames.CONFIG_WORD:
            self.pic.write_config_memory(address, chunk)
        elif mem_name == MemoryNames.USER_ID:
            self.pic.write_user_id_memory(address, chunk)
        elif mem_name == MemoryNames.EEPROM:
            self.pic.write_eeprom_memory(address, chunk)
        elif mem_name == MemoryNames.ICD:
            try:
                self.pic.write_de_memory(address, chunk)
            except AttributeError:
                # Some PIC devices don't have the write_de_memory but instead a _write_de_block function
                self.pic._write_de_block(address, chunk) # pylint: disable=protected-access
        else:
            raise PymcuprogNotSupportedError("Unsupported memtype: {}!".format(mem_name))

    def erase(self, memory_info=None, address=None):
        """
        Erase the device or parts of it.

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
            If memory_info is None the default bulk erase will be run
        :param address: address info for erase (optional)
        """
        if address is None:
            if memory_info is None:
                self.pic.erase()
            else:
                if memory_info[DeviceInfoKeys.NAME] == MemoryNames.ICD:
                    self.pic.erase_de_memory(memory_info[DeviceMemoryInfoKeys.ADDRESS],
                                             memory_info[DeviceMemoryInfoKeys.SIZE])
                else:
                    if DeviceMemoryInfoKeys.ERASE_ADDRESS in memory_info:
                        self.pic.erase(memory_info[DeviceMemoryInfoKeys.ERASE_ADDRESS])
                    else:
                        raise ValueError("Missing erase address for {}".format(memory_info[DeviceInfoKeys.NAME]))
        else:
            self.pic.erase(address)

    def read_device_id(self):
        """
        Get the device info from the device

        :returns: Device ID raw bytes (Little endian)
        """
        pic_id = self.pic.read_id()
        id_array = binary.pack_le16(pic_id)
        self.logger.info("Device ID read out: '%04X'", pic_id)
        return id_array

    def hold_in_reset(self):
        """
        Hold the device in reset
        """
        self.pic.hold_in_reset()

    def release_from_reset(self):
        """
        Release the device from reset
        """
        self.pic.release_from_reset()

    def stop(self):
        """
        Stop programming session
        """
        if self.pic is not None:
            self.pic.end_of_operations()
