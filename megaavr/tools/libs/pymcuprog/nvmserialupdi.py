"""
pyupdi-esque NVM implementation
"""
import binascii

from pyedbglib.util import binary

from . import utils
from .nvm import NvmAccessProvider
from .deviceinfo import deviceinfo
from .deviceinfo.deviceinfokeys import DeviceInfoKeysAvr, DeviceMemoryInfoKeys
from .deviceinfo.memorynames import MemoryNames
from .serialupdi.application import UpdiApplication

import math

from . import progress_bar

# This is a data class so it should not need any methods but will have many instance variables
# pylint: disable=too-many-instance-attributes,too-few-public-methods
class Dut:
    """
    Create a device object for UpdiApplication
    """

    def __init__(self, dev_info):
        # Parse the device info for memory descriptions
        device_memory_info = deviceinfo.DeviceMemoryInfo(dev_info)

        flash_info = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        self.flash_start = flash_info[DeviceMemoryInfoKeys.ADDRESS]
        self.flash_size = flash_info[DeviceMemoryInfoKeys.SIZE]
        self.flash_pagesize = flash_info[DeviceMemoryInfoKeys.PAGE_SIZE]
        self.syscfg_address = dev_info[DeviceInfoKeysAvr.SYSCFG_BASE]
        self.nvmctrl_address = dev_info[DeviceInfoKeysAvr.NVMCTRL_BASE]
        address_key = DeviceMemoryInfoKeys.ADDRESS
        self.sigrow_address = device_memory_info.memory_info_by_name(MemoryNames.SIGNATURES)[address_key]
        self.fuses_address = device_memory_info.memory_info_by_name(MemoryNames.FUSES)[address_key]
        self.userrow_address = device_memory_info.memory_info_by_name(MemoryNames.USER_ROW)[address_key]


class NvmAccessProviderSerial(NvmAccessProvider):
    """
    NVM Access the Python AVR way
    """

    def __init__(self, port, device_info, baud):
        self.avr = None
        NvmAccessProvider.__init__(self, device_info)
        if not baud:
            baud = 115200
        self.dut = Dut(device_info)
        self.avr = UpdiApplication(port, baud, self.dut)
        # Read the device info to set up the UPDI stack variant

        self.avr.read_device_info()
        try:
            self.avr.enter_progmode()
        except IOError as inst:
            self.logger.error("Device is locked.\nError:\n%s", inst)

    def read_device_id(self):
        """
        Read and display (log) the device info

        :returns: Device ID raw bytes (Little endian)
        """
        self.avr.read_device_info()

        signatures_base = self.dut.sigrow_address

        # Read 3 bytes
        sig = self.avr.read_data(signatures_base, 3)
        device_id_read = binary.unpack_be24(sig)
        self.logger.info("Device ID: '%06X'", device_id_read)
        if not self.device_info.get(DeviceInfoKeysAvr.DEVICE_ID) == device_id_read:
            self.logger.warning("ID read ('%06X') does not match expected device id! ('%06X')", device_id_read,
                                self.device_info.get(DeviceInfoKeysAvr.DEVICE_ID))
            raise ValueError("Device ID does not match")
        revision = self.avr.read_data(self.device_info.get(DeviceInfoKeysAvr.SYSCFG_BASE) + 1, 1)
        self.logger.info("Device revision: '%s'", chr(revision[0] + ord('A')))
        serial = self.avr.read_data(signatures_base + 3, 10)
        self.logger.info("Device serial number: '%s'", binascii.hexlify(serial))

        # Return the raw signature bytes, but swap the endianness as target sends ID as Big endian
        return bytearray([sig[2], sig[1], sig[0]])

    def erase(self, memory_info=None, address=None):
        """
        Do a chip erase of the device
        """
        _dummy = memory_info
        _dummy = address
        try:
            self.avr.nvm.chip_erase()
        except IOError as inst:
            self.logger.error("Device is locked. Performing unlock with chip erase.\nError: ('%s')", inst)
            self.avr.unlock()

    def write(self, memory_info, offset, data, blocksize=0, pagewrite_delay=0):
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
        memtype_string = memory_info[DeviceMemoryInfoKeys.NAME]

        offset_aligned += memory_info[DeviceMemoryInfoKeys.ADDRESS]

        if memtype_string in (MemoryNames.FLASH, MemoryNames.EEPROM, MemoryNames.FUSES):
            write_chunk_size = memory_info[DeviceMemoryInfoKeys.PAGE_SIZE]
        else:
            write_chunk_size = len(data_aligned)

        n_chunk = math.ceil(len(data_aligned)/write_chunk_size)
        bar = progress_bar.ProgressBar(n_chunk, hide=n_chunk == 1)
        while data_aligned:
            if len(data_aligned) < write_chunk_size:
                write_chunk_size = len(data_aligned)
            chunk = data_aligned[0:write_chunk_size]
            self.logger.debug("Writing %d bytes to address 0x%06X", write_chunk_size, offset_aligned)
            if memtype_string == MemoryNames.FUSES:
                self.avr.nvm.write_fuse(offset_aligned, chunk)
            elif memtype_string == MemoryNames.EEPROM:
                self.avr.nvm.write_eeprom(offset_aligned, chunk)
            else:
                # Spence Konde, 5/8/2021:
                # As far as I can tell, this is the only point where, we're writing a hex file, we know both the page size
                # AND are in the path of blocksize parameter. So - if its 0 or not given  we should "do the old behavior", then
                # blocksize=2. The special value -1 tells us to have it write blocks equal to chunk/page size. Any other number
                # will be used as blocksize. Negative numbers beyond -1 were replaced with zero way at the beginning, as they would
                # result in crazy behavior and make everything fall over.
                # megaTinyCore and DxCore will always pass -1 as blocksize unless we find something where that doesn't work.
                #
                # Also, we are now finally in the section of the code specific to serialupdi. Up until we get here, 0 is the default
                # and if that's what we got, we omit it when making other calls, because there are almost certainly calls elsewhere
                # that. Now that we are here, the default value is 2 (ie, one word at a time) but that won'ty be something we see often.
                #
                # It strikes me that here is *ALSO* where we know whether we are on the first, a middle, or the last page. Say we
                # kept count of how many pages had been written already - if it was 0 and nChunk > 1, we would pass an argument that says
                # This is the first page we are writing, do all that stuff we need to do at the start of a bulk write.
                # if it was nChunk - 1, we would send a different value for that argumennt, saying it was the last one of a bulk write
                # so it should do the stuff to end the bulk write mode. And otherwise, it gets a third value that gets treated as
                # a signal to omit all of those. for the streamlined write protocol, which could improve performance by another 22-45%
                # If you agree, we should do that.
                # What we currently do is grossly inefficient, because (due to the penalty for small packets) we spend half of our time
                # for every page: Setting the address pointer (only need to do this at the beginning  - when reading second and subsequent pages
                # the previous writes left the pointer at exactly the location we then set it to.). Setting NVM cmd to FLWR - only needs to be done
                # at the start of a bulk write, assuming we also stop setting NVM command to NOOP after every page. Setting RSD - if we
                # do all I'm talking about here, we can set it at start of bulk write. And we can juyst check for for NVM errors before
                # the first and after the last page, not before and after every page. My models suggest this should improve performance
                # by 22% at 115200 baud, and 44% and 345600 baud (which is 1.5x 230400 baud - and happens to be about the fastest you can
                # do a bulk write that is consistent with the datasheet flash write time spec.
                #
                # See also my comment below in read() - these two places are where we can achieve the last noticable performance leaps.
                # -Spence
                bulk = 1
                if n_chunk == 1:
                    #if omly one chunk, it is NOT a bulk write.
                    bulk = 0
                elif len(data_aligned) <= write_chunk_size:
                    # We are on the last page of a bulk write
                    bulk = 2
                if blocksize == 0:
                    self.avr.nvm.write_flash(offset_aligned, chunk, pagewrite_delay=pagewrite_delay)
                else:
                    self.avr.nvm.write_flash(offset_aligned, chunk, blocksize=blocksize, bulkwrite = bulk, pagewrite_delay=pagewrite_delay)
            offset_aligned += write_chunk_size
            data_aligned = data_aligned[write_chunk_size:]
            bar.step()

    def read(self, memory_info, offset, numbytes, max_read_chunk=None):
        """
        Read the memory in chunks

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset in the memory type
        :param numbytes: number of bytes to read
        :param max_read_chunk: <=256
        :return: array of bytes read
        """
        offset += memory_info[DeviceMemoryInfoKeys.ADDRESS]

        # if reading from flash, we want to read words if it would reduce number of USB serial transactions.
        # this function is called for everything though, so be careful not to use it for memories read one byte at a time, like fuses
        data = []

        if max_read_chunk is None:
            read_chunk_size = 0x100
        else:
            read_chunk_size = max_read_chunk

        use_word_access = False
        memtype_string = memory_info[DeviceMemoryInfoKeys.NAME]
        if memtype_string in (MemoryNames.FLASH):
            if numbytes > 0x100 and max_read_chunk is None:
                use_word_access = True
                read_chunk_size = 0x200

        # SACRIFICES SPEED FOR COMPATIBILITY - above line should happen whenever --limitreadsize=1  command line parameter is not passed, so we can only turn it on for specific tools -> programmer options that have this weird limitation. I couldn't propagate it through this mess!
        n_chunk = math.ceil(numbytes/read_chunk_size)
        bar = progress_bar.ProgressBar(n_chunk, hide=n_chunk == 1)

        while numbytes:
            if numbytes < read_chunk_size:
                read_chunk_size = numbytes
            self.logger.debug("Reading %d bytes from address 0x%06X", read_chunk_size, offset)
            if use_word_access:
                data += self.avr.read_data_words(offset, read_chunk_size>> 1)
            else:
                data += self.avr.read_data(offset, read_chunk_size)
            offset += read_chunk_size
            numbytes -= read_chunk_size
            bar.step()

        return data

    def hold_in_reset(self):
        """
        Hold device in reset
        """
        # For UPDI parts it is sufficient to enter programming mode to hold the target in reset
        # Since the start function is a prerequisite to all functions in this file it can be
        # assumed that programming mode already has been entered
        return

    def release_from_reset(self):
        """
        Release device from reset
        """
        # Entering programming mode on UPDI parts will hold the device in reset.  So to release
        # the reset the programming mode must be left.
        self.avr.leave_progmode()

    def stop(self):
        """
        Stop the debugging session
        """
        if self.avr is not None:
            self.avr.leave_progmode()
