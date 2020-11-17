"""
SPI NVM implementation
"""
from pyedbglib.protocols.avrispprotocol import AvrIspProtocol

from . import utils
from .nvm import NvmAccessProviderCmsisDapAvr
from .deviceinfo.memorynames import MemoryNames
from .deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys
from .pymcuprog_errors import PymcuprogNotSupportedError

class NvmAccessProviderCmsisDapSpi(NvmAccessProviderCmsisDapAvr):
    """
    NVM Access the SPI way
    """

    def __init__(self, transport, device_info):
        NvmAccessProviderCmsisDapAvr.__init__(self, device_info)

        self._log_incomplete_stack('AVR-ISP/SPI')

        self.isp = AvrIspProtocol(transport)
        self.isp.enter_progmode()

    def stop(self):
        """
        Stop programming session
        """
        self.isp.leave_progmode()

    def read_device_id(self):
        """
        Read the device info

        :returns: Device ID raw bytes (Little endian)
        """
        resp = self.isp.get_id()
        self.logger.info("ID read: %02X%02X%02X", resp[0], resp[1], resp[2])
        # Return the raw ID bytes, but swap byte order to get LSB first
        return bytearray([resp[2], resp[1], resp[0]])

    def erase(self, memory_info=None, address=None):
        """
        Do a chip erase of the device
        """
        _dummy = memory_info
        _dummy = address
        self.isp.erase()

    def write(self, memory_info, offset, data):
        """
        Write the memory with data

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset within the memory type
        :param data: the data to program
        :return: None
        """
        # Make sure the data is aligned to a memory page
        data_aligned, offset_aligned = utils.pagealign(data,
                                                       offset,
                                                       memory_info[DeviceMemoryInfoKeys.PAGE_SIZE],
                                                       memory_info[DeviceMemoryInfoKeys.WRITE_SIZE])
        if memory_info[DeviceMemoryInfoKeys.NAME] != MemoryNames.FLASH:
            raise PymcuprogNotSupportedError("Currently only Flash memory is supported by write for SPI/ISP")
        write_chunk_size = memory_info[DeviceMemoryInfoKeys.PAGE_SIZE]
        while data_aligned:
            if len(data_aligned) < write_chunk_size:
                write_chunk_size = len(data_aligned)
            chunk = data_aligned[0:write_chunk_size]
            self.logger.debug("Writing %d bytes to address 0x%06X", write_chunk_size, offset_aligned)
            self.isp.write_flash_page(offset_aligned, chunk)
            offset_aligned += write_chunk_size
            data_aligned = data_aligned[write_chunk_size:]

    def read(self, memory_info, offset, numbytes):
        """
        Read the memory in chunks

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset in the memory type
        :param numbytes: number of bytes to read
        :return: array of bytes read
        """
        if memory_info[DeviceMemoryInfoKeys.NAME] == MemoryNames.FLASH:
            _dummy = memory_info
            data = []
            read_chunk_size = 0x100
            while numbytes:
                if numbytes < read_chunk_size:
                    read_chunk_size = numbytes
                self.logger.debug("Reading %d bytes from address 0x%06X", read_chunk_size, offset)
                data += self.isp.read_flash_chunk(offset, read_chunk_size)
                offset += read_chunk_size
                numbytes -= read_chunk_size
        elif memory_info[DeviceMemoryInfoKeys.NAME] == MemoryNames.SIGNATURES:
            data = self.isp.read_signature_bytes(offset, numbytes)
        elif memory_info[DeviceMemoryInfoKeys.NAME] == MemoryNames.CALIBRATION_ROW:
            data = self.isp.read_calibration_bytes(offset, numbytes)
        else:
            raise PymcuprogNotSupportedError(
                "Currently only Flash, Signature and Calibration memories are supported by read for SPI/ISP")
        return data

    def hold_in_reset(self):
        """
        Hold device in reset
        """
        # For SPI/ISP parts it is sufficient to enter programming mode to hold the target in reset
        # Since the start function is a prerequisite to all functions in this file it can be
        # assumed that programming mode already has been entered
        return

    def release_from_reset(self):
        """
        Release device from reset
        """
        # Entering programming mode on SPI/ISP parts will hold the device in reset.  So to release
        # the reset the programming mode must be left.
        self.isp.leave_progmode()
