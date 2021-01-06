"""
Simple wrapper for AVR ISP programming protocol over JTAGICE3 transport

Important note:
    The AVR ISP protocol dates back to very early AVR devices.  Many of the parametric information included in the
    protocol metadata applies only to those old devices.  This is not a universal implementation, and does not draw
    any such information from the XML descriptions included in Atmel Studio (ATDF) and MPLABX.
    It does work on ATmega328P and close relatives.

TODO: use SPI CLOCK settings
TODO: add more memory types
"""

from logging import getLogger
from .jtagice3protocol import Jtagice3Protocol
from ..util import binary


class AvrIspProtocolError(Exception):
    """Exception type for AVRISP command-response wrapping"""

    pass


class AvrIspProtocol(Jtagice3Protocol):
    """Protocol wrapper"""

    # Protocol commands
    SPI_CMD_SIGN_ON = 0x01
    SPI_CMD_LOAD_ADDRESS = 0x06
    SPI_CMD_ENTER_PROGMODE = 0x10
    SPI_CMD_LEAVE_PROGMODE = 0x11
    SPI_CMD_CHIP_ERASE = 0x12
    SPI_CMD_PROGRAM_FLASH = 0x13
    SPI_CMD_READ_FLASH = 0x14
    SPI_CMD_PROGRAM_EEPROM = 0x15
    SPI_CMD_READ_EEPROM = 0x16
    SPI_CMD_PROGRAM_FUSE = 0x17
    SPI_CMD_READ_FUSE = 0x18
    SPI_CMD_PROGRAM_LOCK = 0x19
    SPI_CMD_READ_LOCK = 0x1A
    SPI_CMD_READ_SIGNATURE = 0x1B
    SPI_CMD_READ_OSCCAL = 0x1C
    SPI_CMD_SET_BAUD = 0x1D
    SPI_CMD_GET_BAUD = 0x1E

    SPI_STATUS_CMD_OK = 0x00

    # Warnings
    SPI_STATUS_CMD_TOUT = 0x80
    SPI_STATUS_RDY_BSY_TOUT = 0x81

    # Errors
    SPI_STATUS_CMD_FAILED = 0xC0
    SPI_STATUS_CMD_UNKNOWN = 0xC9
    SPI_STATUS_PHY_ERROR = 0xCB
    SPI_STATUS_CLOCK_ERROR = 0xCC
    SPI_STATUS_BAUD_INVALID = 0xCD

    # Static ISP programming constants

    # Enter progmode
    SYNC_LOOPS = 32
    PIN_DELAY = 100
    FUNC_DELAY = 25
    BYTE_DELAY = 0
    AVR_POLL_VALUE = 0x53
    AVR_REPLY_OFFSET = 3

    # Could be parameterised, but is consistent on newer AVRs with SPI
    AVR_PROG_ENABLE_COMMAND = [0xAC, 0x53, 0x00, 0x00]
    AVR_READ_SIGNATURE_COMMAND = [0x30, 0x00, 0x00, 0x00]
    AVR_READ_CALIBRATION_BYTE_COMMAND = [0x38, 0x00, 0x00, 0x00]
    AVR_READ_FLASH_COMMAND = 0x20
    AVR_ERASE_COMMAND = [0xAC, 0x80]
    AVR_ERASE_DELAY = 45
    AVR_ERASE_POLLMODE = 1
    AVR_LOAD_PAGE_COMMAND = 0x40
    AVR_WRITE_PAGE_COMMAND = 0x4C

    # Leave progmode
    AVR_PRE_LEAVE_DELAY_MS = 1
    AVR_POST_LEAVE_DELAY_MS = 1

    def __init__(self, transport):
        self.logger = getLogger(__name__)
        Jtagice3Protocol.__init__(self, transport, Jtagice3Protocol.HANDLER_SPI)
        self.last_address = -1

    def _spi_cmd_resp(self, cmd):
        """Send a command, receive a response, and check its validity & status"""
        resp = self.jtagice3_command_response(cmd)
        if not resp[0] == cmd[0]:
            raise AvrIspProtocolError("AVRISP protocol: Invalid response received")
        if not resp[1] == AvrIspProtocol.SPI_STATUS_CMD_OK:
            raise AvrIspProtocolError("AVRISP protocol: Command failed")
        return resp[2:]

    def enter_progmode(self):
        """Enter programming mode"""
        command = bytearray([AvrIspProtocol.SPI_CMD_ENTER_PROGMODE])
        command.extend([0, AvrIspProtocol.PIN_DELAY, AvrIspProtocol.FUNC_DELAY, AvrIspProtocol.SYNC_LOOPS,
                        AvrIspProtocol.BYTE_DELAY, AvrIspProtocol.AVR_POLL_VALUE, AvrIspProtocol.AVR_REPLY_OFFSET])
        command.extend(AvrIspProtocol.AVR_PROG_ENABLE_COMMAND)
        self._spi_cmd_resp(command)

    def leave_progmode(self):
        """Leave programming mode"""
        command = bytearray([AvrIspProtocol.SPI_CMD_LEAVE_PROGMODE])
        command.extend([AvrIspProtocol.AVR_PRE_LEAVE_DELAY_MS, AvrIspProtocol.AVR_POST_LEAVE_DELAY_MS])
        self._spi_cmd_resp(command)

    def get_id(self):
        """Read device ID"""
        # Read 3 signature bytes from offset 0
        return self._read_signature_calibration_bytes(0, 3, AvrIspProtocol.SPI_CMD_READ_SIGNATURE,
                                                      AvrIspProtocol.AVR_READ_SIGNATURE_COMMAND)

    def load_address(self, address):
        """Loads the address pointer (stored in FW)"""
        command = bytearray([AvrIspProtocol.SPI_CMD_LOAD_ADDRESS])
        command.extend(binary.pack_be32(address))
        self._spi_cmd_resp(command)
        self.last_address = address

    def read_flash_chunk(self, byte_address, numbytes):
        """
        Reads a chunk of flash memory

        :param byte_address: start address
        :param numbytes: number of bytes
        :return: data read
        """
        if numbytes > 512:
            raise ValueError("Read chunk too large!")
        if self.last_address != byte_address >> 1:
            self.load_address(byte_address >> 1)
        self.last_address += numbytes
        command = bytearray([AvrIspProtocol.SPI_CMD_READ_FLASH])
        command.extend(binary.pack_be16(numbytes))
        command.extend([AvrIspProtocol.AVR_READ_FLASH_COMMAND])
        resp = self._spi_cmd_resp(command)
        # Strip off status byte
        return resp[:-1]

    def _read_signature_calibration_bytes(self, offset, numbytes, protocol_command, command_array):
        """
        Read device signature or calibration bytes
        :param offset: offset to read from
        :param numbytes: number of bytes to read
        :param protocol_command: protocol command to use to send to tool
        :param command_array: command bytes to send to device
        """
        # Read numbytes bytes
        result = bytearray(numbytes)
        for i in range(numbytes):
            # All SPI commands are 4-byte transfers
            command = bytearray([protocol_command, 4])
            # Append offset byte
            command_array[2] = i+offset
            # Append command array
            command.extend(command_array)
            resp = self._spi_cmd_resp(command)
            # Extract result
            result[i] = resp[0]
        return result

    def read_signature_bytes(self, offset, numbytes):
        """
        Read device signature bytes
        :param offset: offset to read from
        :param numbytes: number of bytes to read
        """
        # Read signature bytes
        return self._read_signature_calibration_bytes(offset, numbytes, AvrIspProtocol.SPI_CMD_READ_SIGNATURE,
                                                      AvrIspProtocol.AVR_READ_SIGNATURE_COMMAND)

    def read_calibration_bytes(self, offset, numbytes):
        """
        Read device calibration bytes
        :param offset: offset to read from
        :param numbytes: number of bytes to read
        """
        # Read calibration bytes
        return self._read_signature_calibration_bytes(offset, numbytes, AvrIspProtocol.SPI_CMD_READ_OSCCAL,
                                                      AvrIspProtocol.AVR_READ_CALIBRATION_BYTE_COMMAND)

    def write_flash_page(self, byte_address, data):
        """
        Writes a page of flash

        :param byte_address: start address
        :param data: data to write
        """
        if len(data) > 512:
            raise ValueError("Write chunk too large!")
        if self.last_address != byte_address >> 1:
            self.load_address(byte_address >> 1)
        command = bytearray([AvrIspProtocol.SPI_CMD_PROGRAM_FLASH])
        command.extend(binary.pack_be16(len(data)))
        command.extend([0x81])  # Page mode
        command.extend([0])  # Not used
        command.extend([AvrIspProtocol.AVR_LOAD_PAGE_COMMAND])
        command.extend([AvrIspProtocol.AVR_WRITE_PAGE_COMMAND])
        command.extend([0])  # Not used
        command.extend([0])  # Not used
        command.extend([0])  # Not used
        command.extend(data)
        self._spi_cmd_resp(command)

    def erase(self):
        """Chip erase"""
        command = bytearray([AvrIspProtocol.SPI_CMD_CHIP_ERASE])
        command.extend([AvrIspProtocol.AVR_ERASE_DELAY])
        command.extend([AvrIspProtocol.AVR_ERASE_POLLMODE])
        command.extend(AvrIspProtocol.AVR_ERASE_COMMAND)
        self._spi_cmd_resp(command)
