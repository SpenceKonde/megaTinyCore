"""
Python MCU programmer
"""
import copy
from logging import getLogger
from collections import namedtuple

# Device data
from .deviceinfo import deviceinfo

from . import utils
from .pymcuprog_errors import PymcuprogNotSupportedError, PymcuprogSessionConfigError
from .pymcuprog_errors import PymcuprogError
from .nvm import get_nvm_access_provider
from .deviceinfo.memorynames import MemoryNameAliases
from .deviceinfo.deviceinfokeys import DeviceInfoKeysPic, DeviceMemoryInfoKeys

DEFAULT_BULK_ERASE_ADDRESS_KEY = DeviceInfoKeysPic.DEFAULT_BULK_ERASE_ADDRESS

class Programmer:
    """
    Main programmer class.
    """

    def __init__(self, transport):
        # Hook onto logger
        self.logger = getLogger(__name__)
        # Use transport passed in
        self.transport = transport
        # Clear device model and mem info objects
        self.device_info = None
        self.device_model = None
        self.device_memory_info = None
        self.options = {}

    def set_options(self, options):
        """
        Stores options

        :param options: options to store
        """
        self.options = options

    def load_device(self, device_name):
        """
        Loads the device from the device folder

        :param device_name:
        :raises: PymcuprogNotSupportedError if device is not supported
        """
        # Try to instantiate device info.  This will check if there is device support at all
        try:
            self.logger.info("Setting up programming session for '%s'", device_name)
            self.device_info = deviceinfo.getdeviceinfo(device_name)
        except ImportError as err:
            raise PymcuprogNotSupportedError("Unable to find device info: {}".format(err))

        # Now build a memory model for this device
        self.device_memory_info = deviceinfo.DeviceMemoryInfo(self.device_info)

    def setup_device(self, interface=None, packpath=None, clk=None):
        """
        Sets up a programming session with a given device

        :param device_name: device to program
        :param interface: interface to use
        :param packpath: path to packs to use (for PIC)
        :param clk: clock frequency

        :raises SerialException if unable to connect to serial port (if using serial port instead of physical debugger)
        """
        # Device must be loaded first
        if self.device_info is None:
            raise PymcuprogError("Device must be loaded before setup!")

        # Find a NVM provider that matches the device and transport
        try:
            self.device_model = get_nvm_access_provider(self.transport,
                                                        self.device_info,
                                                        interface=interface,
                                                        packpath=packpath,
                                                        frequency=clk,
                                                        options=self.options)
        except ImportError:
            raise PymcuprogSessionConfigError(
                "Unable to setup stack using the given packpath: '{0:s}'".format(
                    packpath or "None"))
        if self.device_model is None:
            raise PymcuprogSessionConfigError("Unable to setup stack, check session config parameters")

    def start(self, user_interaction_callback=None):
        """
        Starts the programming session with the device model

        :param user_interaction_callback: Callback to be called when user interaction is required,
            for example when doing UPDI high-voltage activation with user target power toggle.
            This function could ask the user to toggle power and halt execution waiting for the user
            to respond (this is default behavior if the callback is None), or if the user is another
            script it could toggle power automatically and then return.
        """
        self.device_model.start(user_interaction_callback=user_interaction_callback)

    def stop(self):
        """
        Stops the programming session with the device model
        """
        return self.device_model.stop()

    def get_device_model(self):
        """
        Exposes the device model in use to clients
        """
        return self.device_model

    def get_device_memory_info(self):
        """
        Exposes the device memory model to clients
        """
        return self.device_memory_info

    # Device model API functions

    def read_device_id(self):
        """
        Read the device ID

        :returns: Device ID raw bytes (Little endian)
        """
        self.logger.info("Reading device ID...")
        return self.device_model.read_device_id()

    def erase(self, memory_name, address):
        """
        Erase the device

        :param memory_name: memory region to erase as defined in deviceinfo.memorynames
            MemoryNameAliases.ALL will run the widest erase (e.g. chip erase on AVR or the widest bulk erase on PIC)
        :param address: address to erase
        """
        self.logger.info("Erase...")
        if memory_name == MemoryNameAliases.ALL:
            # Run default erase which is the widest erase
            memory_info = None
            if DEFAULT_BULK_ERASE_ADDRESS_KEY in self.device_info:
                address = self.device_info[DEFAULT_BULK_ERASE_ADDRESS_KEY]
            else:
                address = None
        else:
            memory_info = self.device_memory_info.memory_info_by_name(memory_name)
        self.device_model.erase(memory_info=memory_info, address=address)

    def write_memory(self, data, memory_name, offset=0, blocksize=0):
        """
        Write memory on the device

        :param data: data to write
        :param memory_name: memory type to write
        :param offset: offset/address within that region to write
        :return: boolean status

        :raises: ValueError if trying to write outside the specified memory
        :raises: ValueError if the specified memory is not defined for the target device
        :raises: PymcuprogNotSupportedError if memory can't be written
        """
        self.logger.info("Write...")

        # Just some sanity checking of inputs
        if offset < 0:
            raise ValueError("Write offset can't be negative, requested offset: {}".format(offset))

        # Get information about the memory area
        memory = self.device_memory_info.memory_info_by_name(memory_name)
        size = memory[DeviceMemoryInfoKeys.SIZE]

        if memory[DeviceMemoryInfoKeys.WRITE_SIZE] == 0:
            raise PymcuprogNotSupportedError("{} memory can't be written".format(memory_name))

        if offset + len(data) > size:
            msg = "{} bytes of data at offset {} is outside the boundaries of '{}' with size {}".format(len(data),
                                                                                                        offset,
                                                                                                        memory_name,
                                                                                                        size)
            raise ValueError(msg)

        # Write the data to NVM
        self.logger.info("Writing %d bytes of data to %s...", len(data), memory[DeviceMemoryInfoKeys.NAME])
        if blocksize == 0:
            self.device_model.write(memory, offset, data)
        else:
            self.device_model.write(memory, offset, data, blocksize=blocksize)
        self.logger.info("Write complete.")
        return True

    def verify_memory(self, data, memory_name, offset=0):
        """
        Verify memory content

        :param data: data to verify against
        :param memory_name: memory type
        :param offset: offset/address within that memory region
        :return: boolean compare status
        """
        # Get information about the memory area
        memory = self.device_memory_info.memory_info_by_name(memory_name)
        verify_mask = memory[DeviceMemoryInfoKeys.VERIFY_MASK]

        # Read back and compare the data to verify
        data_verify = self.read_memory(memory_name, offset, len(data))[0].data

        self.logger.info("Verifying...")
        try:
            # Use the compare util, which throws ValueError on mismatch
            utils.compare(data, data_verify, offset, verify_mask)
        except ValueError as error:
            self.logger.error("Verify failed: %s", str(error))
            return False
        return True

    def read_memory(self, memory_name, offset, numbytes=0):
        """
        Read device memory

        :param memory_name: memory type to read as defined in deviceinfo.memorynames
            MemoryNameAliases.ALL will read all memories defined in the device model for the configured
            device (numbytes and offset will be ignored)
        :param offset: offset/start address within the memory to start reading from
        :param numbytes: number of bytes to read.  0 means read all memory locations for given memory
            type (offset still applies)
        :return: list of namedtuples with two fields: data and memory_info.  data contains a byte array
            of raw data bytes and memory_info is a dictionary with memory information as defined in
            deviceinfo.deviceinfo.DeviceMemoryInfo.  Normally the list will contain one item,
            but when memory_name parameter is MemoryNameAliases.ALL there will be one namedtuple
            item per memory type read.
        :raises: ValueError if trying to read outside the specified memory
        :raises: ValueError if the specified memory is not defined for the target device
        """
        # Just some sanity checking of inputs
        if offset < 0:
            raise ValueError("Read offset can't be negative, requested offset: {}".format(offset))
        if numbytes < 0:
            raise ValueError("Can't read negative number of bytes, requested numbytes: {}".format(numbytes))

        memories_read = []

        if memory_name == MemoryNameAliases.ALL:
            memories = list(self.device_memory_info.mem_by_name.keys())

            # When reading all memories offset is ignored
            offset = 0
            # ...and the same with numbytes
            numbytes = 0
        else:
            memories = [memory_name]

        for memory in memories:
            # Get information about the memory area
            meminfo = self.device_memory_info.memory_info_by_name(memory)

            # For each memory type there will be one named tuple with raw data as a bytearray and a dictionary
            # with information about the memory
            memory_read_tuple = namedtuple("Memory", 'data memory_info')
            memory_read_tuple.data = bytearray([])
            memory_read_tuple.memory_info = meminfo

            # Align the read to a page boundary
            page_offset = offset % meminfo[DeviceMemoryInfoKeys.PAGE_SIZE]

            offset_adjusted = offset - page_offset
            numbytes_adjusted = numbytes
            # If number of bytes is not given, default to read the complete memory starting at the given offset
            if numbytes == 0:
                numbytes_adjusted = meminfo[DeviceMemoryInfoKeys.SIZE] - offset_adjusted
            else:
                numbytes_adjusted = numbytes_adjusted + page_offset

            # Read size correction
            read_size_key = DeviceMemoryInfoKeys.READ_SIZE
            if numbytes_adjusted % meminfo[read_size_key]:
                extra = meminfo[read_size_key] - numbytes_adjusted % meminfo[read_size_key]
                numbytes_adjusted += extra
            else:
                extra = 0

            if offset_adjusted + numbytes_adjusted > meminfo[DeviceMemoryInfoKeys.SIZE]:
                raise ValueError("{} bytes of data at offset {} is outside the boundaries of '{}' with size {}".format(
                    numbytes_adjusted, offset, meminfo[DeviceMemoryInfoKeys.NAME], meminfo[DeviceMemoryInfoKeys.SIZE]))

            # Read the data
            self.logger.info("Reading %d bytes from %s...", numbytes_adjusted, meminfo[DeviceMemoryInfoKeys.NAME])
            data = self.device_model.read(meminfo, offset_adjusted, numbytes_adjusted)

            # Strip the extra data that was read
            memory_read_tuple.data = data[page_offset:numbytes_adjusted - extra]

            # Append a copy of the memory namedtuple to avoid a reference being appended as the memory_read_tuple will
            # change for each loop iteration.  Note that when using a deepcopy the content of the memory_read_tuple will
            # be copied too
            memories_read.append(copy.deepcopy(memory_read_tuple))

        return memories_read

    def hold_in_reset(self):
        """
        Hold the device in reset
        """
        self.logger.info("Hold in reset")
        self.device_model.hold_in_reset()

    def release_from_reset(self):
        """
        Release the device from reset (i.e. let the device run)
        """
        self.logger.info("Release from reset")
        self.device_model.release_from_reset()
