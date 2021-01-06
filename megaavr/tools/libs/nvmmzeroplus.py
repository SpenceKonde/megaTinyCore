"""
M0+ NVM implementation
"""
from logging import getLogger

from pyedbglib.util import binary

from . import utils
from .nvm import NvmAccessProviderCmsisDapTool
from .samtarget import SamD2xTarget
from .pymcuprog_errors import PymcuprogError
from .deviceinfo.memorynames import MemoryNames
from .deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys

import math
from . import progress_bar


class NvmAccessProviderCmsisDapMZeroPlus(NvmAccessProviderCmsisDapTool):
    """
    SAMD programmer
    """

    def __init__(self, transport, device_info, frequency=None):
        self.logger = getLogger(__name__)
        NvmAccessProviderCmsisDapTool.__init__(self, device_info)

        self._log_incomplete_stack('SAM-M0+')

        self.sam = SamD2xTarget(transport)
        if not frequency:
            # Default clock to 2MHz
            frequency = 2000000
        self.sam.connect(frequency)

    def read_device_id(self):
        """
        Read out the DAP ID and Device ID

        :returns: Device ID raw bytes (Little endian)
        """
        dap_id = self.sam.read_idcode()
        self.logger.info("DAP ID detected: 0x%08X", dap_id)
        d_id = self.sam.read_device_id()
        self.logger.info("Device ID detected: 0x%08X", d_id)
        locked = self.sam.is_device_locked()
        if locked:
            self.logger.error("Device is locked - a chip erase will be required!")
        return binary.pack_le32(d_id)

    def read(self, memory_info, offset, numbytes):
        """
        Read the memory in chunks

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset in the memory type
        :param numbytes: number of bytes to read
        :return: array of bytes read
        """
        offset += memory_info[DeviceMemoryInfoKeys.ADDRESS]

        return self.sam.read_flash(address=offset, numbytes=numbytes)

    def write(self, memory_info, offset, data):
        """
        Write the memory with data

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset within the memory type
        :param data: the data to program
        :return: None
        """
        locked = self.sam.is_device_locked()
        if locked:
            raise PymcuprogError("Device is locked - a chip erase is required first!")

        memtype_string = memory_info[DeviceMemoryInfoKeys.NAME]
        offset += memory_info[DeviceMemoryInfoKeys.ADDRESS]

        if memtype_string == MemoryNames.FLASH:
            write_chunk_size = memory_info[DeviceMemoryInfoKeys.PAGE_SIZE]
        elif memtype_string == MemoryNames.USER_ROW:
            write_chunk_size = memory_info[DeviceMemoryInfoKeys.WRITE_SIZE]
        else:
            raise PymcuprogError("Unknown memtype")

        # Align data to always start at chunk boundary
        data_aligned, offset_aligned = utils.pagealign(data,
                                                       offset,
                                                       write_chunk_size,
                                                       memory_info[DeviceMemoryInfoKeys.WRITE_SIZE])

        # Pad
        while len(data_aligned) % write_chunk_size:
            data_aligned.append(0xFF)

        n_chunk = math.ceil(len(data_aligned)/write_chunk_size)
        bar = progress_bar.ProgressBar(n_chunk, hide=n_chunk == 1)

        # Chunk and write
        while data_aligned:
            bar.step()
            self.logger.debug("Writing %d bytes to address 0x%06X", write_chunk_size, offset_aligned)
            chunk = data_aligned[0:write_chunk_size]
            if memtype_string == MemoryNames.FLASH:
                self.sam.write_flash_page(chunk, offset_aligned)
            else:
                self.sam.write_user_row_word(offset_aligned, binary.unpack_le32(chunk))
            offset_aligned += write_chunk_size
            data_aligned = data_aligned[write_chunk_size:]

    def erase(self, memory_info=None, address=None):
        """
        Do an erase of the device
        """
        _dummy = address
        if memory_info is not None and memory_info[DeviceMemoryInfoKeys.NAME] == MemoryNames.USER_ROW:
            self.sam.erase_user_row(memory_info[DeviceMemoryInfoKeys.ADDRESS])
        else:
            # All other erases are DSU chip erase
            self.sam.chip_erase_dsu()
            # Re-init
            self.sam.reinitialise()

    def stop(self):
        """
        Stop programming session
        """
        self.sam.disconnect()
