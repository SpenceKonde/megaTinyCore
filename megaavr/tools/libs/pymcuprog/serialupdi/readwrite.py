"""
Read/write access provider for UPDI
"""
from logging import getLogger
from pymcuprog.pymcuprog_errors import PymcuprogError
from . import constants


class UpdiReadWrite(object):
    """
    Provides various forms of reads and writes for UPDI applications
    Makes us of the datalink provided
    """

    def __init__(self, datalink):
        self.logger = getLogger(__name__)
        self.datalink = datalink

    def read_cs(self, address):
        """
        Read from Control/Status space
        :param address: address (index) to read
        :return: value read
        """
        return self.datalink.ldcs(address)

    def write_cs(self, address, value):
        """
        Write to Control/Status space
        :param address: address (index) to write
        :param value: 8-bit value to write
        """
        return self.datalink.stcs(address, value)

    def write_key(self, size, key):
        """
        Write a KEY into UPDI
        :param size: size of key to send
        :param key: key value
        """
        return self.datalink.key(size, key)

    def read_sib(self):
        """
        Read the SIB from UPDI
        :return: SIB string (bytearray) read
        """
        return self.datalink.read_sib()

    def read_byte(self, address):
        """
        Read a single byte from UPDI
        :param address: address to read from
        :return: value read
        """
        return self.datalink.ld(address)

    def write_byte(self, address, value):
        """
        Writes a single byte to UPDI
        :param address: address to write to
        :param value: value to write
        """
        return self.datalink.st(address, value)

    def read_data(self, address, size):
        """
        Reads a number of bytes of data from UPDI
        :param address: address to write to
        :param size: number of bytes to read
        """
        self.logger.debug("Reading %d bytes from 0x%04X", size, address)
        # Range check
        if size > constants.UPDI_MAX_REPEAT_SIZE:
            raise PymcuprogError("Cant read that many bytes in one go")


        # Store the address
        self.datalink.st_ptr(address)

        # Fire up the repeat
        if size > 1:
            self.datalink.repeat(size)

        # Do the read(s)
        return self.datalink.ld_ptr_inc(size)

    def read_data_words(self, address, words):
        """
        Reads a number of words of data from UPDI
        :param address: address to write to
        :param words: number of words to read
        """
        self.logger.debug("Reading %d words from 0x%04X", words, address)

        # Range check
        if words > constants.UPDI_MAX_REPEAT_SIZE:
            raise PymcuprogError("Cant read that many words in one go")

        # special case for single word - so we can optimize ld_ptr_inc16 for >1 word to improve performance.
        if words == 1:
            return self.datalink.ld16(self,address)

        # Otherwise, store the address
        self.datalink.st_ptr(address)

        # For performance, managing repeat count is done in ld_ptr_inc16()
        return self.datalink.ld_ptr_inc16(words)

    def write_data_words(self, address, data, blocksize):
        """
        Writes a number of words to memory
        :param address: address to write to
        :param data: data to write
        :blocksize: max number of bytes being sent
        """
        # Special-case of 1 word
        if len(data) == 2:
            value = data[0] + (data[1] << 8)
            return self.datalink.st16(address, value)

        # Range check
        if len(data) > constants.UPDI_MAX_REPEAT_SIZE << 1:
            raise PymcuprogError("Invalid length")

        # Store the address
        self.datalink.st_ptr(address)


        # For performance, we want to do this with Response Signature Disable set, otherwise the USB Serial latency kills you
        # Just setting RSD here then turning it off at the end - it helps a lot, but you run up against the USB latency. So
        # now, EVERYTHING was moved into the st_ptr_inc16_RSD() function. Unless blocksize precludes it, the whole thing
        # is sent to the serial adapter in a single transfer.
        # the st_pty_inc16_RSD routine does the repeat and rsd enable/disable stu
        return self.datalink.st_ptr_inc16_RSD(data, blocksize)

    def write_data(self, address, data):
        """
        Writes a number of bytes to memory
        :param address: address to write to
        :param data: data to write
        """
        # Special case of 1 byte
        if len(data) == 1:
            return self.datalink.st(address, data[0])
        # Special case of 2 byte
        if len(data) == 2:
            self.datalink.st(address, data[0])
            return self.datalink.st(address + 1, data[1])

        # Range check
        if len(data) > constants.UPDI_MAX_REPEAT_SIZE:
            raise PymcuprogError("Invalid length")

        # Store the address
        self.datalink.st_ptr(address)

        # Fire up the repeat
        self.datalink.repeat(len(data))
        return self.datalink.st_ptr_inc(data)
