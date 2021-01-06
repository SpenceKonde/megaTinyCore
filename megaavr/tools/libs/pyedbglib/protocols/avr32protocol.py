"""Implements Avr32 Protocol, a sub-protocol in the JTAGICE3 family of protocols."""

from logging import getLogger
from .jtagice3protocol import Jtagice3Protocol
from ..util import binary


class Avr32Protocol(Jtagice3Protocol):
    """Implements AVR32 protocol functionality on the JTAGICE3 protocol family"""

    CMD_AVR32_QUERY = 0x00
    CMD_AVR32_SET = 0x01
    CMD_AVR32_GET = 0x02
    CMD_AVR32_ACTIVATE_PHYSICAL = 0x03
    CMD_AVR32_DEACTIVATE_PHYSICAL = 0x04
    CMD_AVR32_GET_ID = 0x10
    CMD_AVR32_ERASE = 0x11
    CMD_AVR32_HALT = 0x12
    CMD_AVR32_RESET = 0x13
    CMD_AVR32_STEP = 0x14
    CMD_AVR32_READ = 0x15
    CMD_AVR32_WRITE = 0x16
    CMD_AVR32_TAP = 0x17
    CMD_AVR32_IS_PROTECTED = 0x18
    CMD_AVR32_ERASE_SECTION = 0x19

    RSP_AVR32_OK = 0x80
    RSP_AVR32_LIST = 0x81
    RSP_AVR32_ID = 0x82
    RSP_AVR32_PC = 0x83
    RSP_AVR32_DATA = 0x84
    RSP_AVR32_DATA_PROBED = 0x85
    RSP_AVR32_FAILED = 0xA0

    AVR32_QUERY_COMMANDS = 0x00
    AVR32_QUERY_FAMILIES = 0x01
    AVR32_QUERY_FUNCTIONS = 0x02
    AVR32_QUERY_INTERFACES = 0x03
    AVR32_QUERY_DEVICE = 0x04
    AVR32_QUERY_CONFIGURATION = 0x05
    AVR32_QUERY_READ_MEMTYPES = 0x06
    AVR32_QUERY_WRITE_MEMTYPES = 0x07
    AVR32_QUERY_COMMAND_VERSIONS = 0x08

    AVR32_CONTEXT_USB = 0x03
    AVR32_CONTEXT_SESSION = 0x04
    AVR32_CONTEXT_PHYSICAL = 0x80
    AVR32_CONTEXT_DEVICE = 0x82

    AVR32_SESSION_RUN_LED = 0x00

    AVR32_USB_MAX_READ = 0x00
    AVR32_USB_MAX_WRITE = 0x01

    AVR32_PHYSICAL_PHYSICAL = 0x00
    AVR32_PHYSICAL_JTAG_CLOCK = 0x01
    AVR32_PHYSICAL_AWIRE_MAX_BAUD = 0x02
    AVR32_PHYSICAL_DAISY = 0x03
    AVR32_PHYSICAL_AWIRE_BAUD = 0x04
    AVR32_PHYSICAL_AWIRE_VERSION = 0x05
    AVR32_PHYSICAL_EXTERNAL_RESET = 0x0C

    AVR32_FLASH_CTRL_BASE = 0x02  # Flash controller base address
    AVR32_FLASH_PAGEBYTES = 0x06  # Number of bytes in one flash page
    AVR32_RESET_DOMAINS = 0x0A  # Number of reset domains
    AVR32_FLASH_PAGES = 0x0B  # Number of pages in the flash block
    AVR32_AWIRE_BASE = 0x10  # Base address for aWire module

    AVR32_MEMTYPE_SAB = 0x80
    AVR32_MEMTYPE_NEXUS = 0x81
    AVR32_MEMTYPE_MEMORY_SERVICE = 0x82
    AVR32_MEMTYPE_BLOCK = 0x83
    AVR32_MEMTYPE_BYTE = 0x84
    AVR32_MEMTYPE_HALF_WORD = 0x85
    AVR32_MEMTYPE_INTERNAL_FLASH = 0x90
    AVR32_MEMTYPE_SYSREG = 0x91
    AVR32_MEMTYPE_REGFILE = 0x92
    AVR32_MEMTYPE_USER_PAGE = 0x93
    AVR32_MEMTYPE_FUSES = 0x94

    AVR32_PHY_INTF_NONE = 0x00
    AVR32_PHY_INTF_JTAG = 0x04
    AVR32_PHY_INTF_AWIRE = 0x07

    AVR32_TAP_IR = 0x00
    AVR32_TAP_DR = 0x01

    AVR32_FAILURE_OK = 0x00

    def __init__(self, transport):
        self.logger = getLogger(__name__)
        super(Avr32Protocol, self).__init__(transport, Jtagice3Protocol.HANDLER_AVR32_GENERIC)

    @staticmethod
    def _pack_sab(address):
        """
        Packs an address into SAB format

        :param address: address in
        :return: SAB address
        """
        return binary.pack_le32(address & 0xFFFFFFFF) + bytearray([(address >> 32)])

    # Physical / connection commands
    def activate_physical(self, use_reset=False):
        """
        Activates the physical interface

        :param use_reset: reset
        :return:
        """
        self.logger.debug("Activate physical")
        device_id = self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR32_ACTIVATE_PHYSICAL, self.CMD_VERSION0, int(use_reset)])), self.RSP_AVR32_DATA)
        self.logger.debug("ID=%02X%02X%02X%02X", device_id[3], device_id[2], device_id[1], device_id[0])
        return device_id

    def deactivate_physical(self):
        """Deactivates the physical interface"""
        self.logger.debug("Deactivate physical")
        return self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR32_DEACTIVATE_PHYSICAL, self.CMD_VERSION0])))

    def get_id(self):
        """
        Reads the device ID

        :return: device ID
        """
        self.logger.debug("%s::get ID", self.__class__.__name__)
        return self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR32_GET_ID, self.CMD_VERSION0])))

    def erase(self):
        """
        Chip erase

        :return:
        """
        self.logger.debug("Erase")
        return self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR32_ERASE, self.CMD_VERSION0])))

    def halt(self, value):
        """
        Apply a Halt request to the device

        :return:
        """
        self.logger.debug("Halt")
        return self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR32_HALT, self.CMD_VERSION0, value & 0xFF])))

    def reset(self, value):
        """
        Apply a Reset request to the device

        :return:
        """
        self.logger.debug("Reset")
        return self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR32_RESET, self.CMD_VERSION0, value & 0xFF])))

    def is_protected(self):
        """
        Checks if the device is protected

        :return:
        """
        self.logger.debug("IsProtected?")
        protected = self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR32_IS_PROTECTED, self.CMD_VERSION0])))
        return True if protected[0] > 0 else False

    def step(self):
        """
        Single steps the device

        :return: new PC value
        """
        self.logger.debug("Step")
        program_counter = binary.unpack_le32(
            self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR32_STEP, self.CMD_VERSION0])),
                                self.RSP_AVR32_PC))
        msg = "PC read as 0x{:08X}".format(program_counter)
        self.logger.debug(msg)
        return program_counter

    def memory_read(self, memtype, address, num_bytes):
        """
        Read memory form the target

        :param memtype: memory type (section)
        :param address: start address
        :param num_bytes: number of bytes
        :return: memory read
        """
        self.logger.debug("Reading memory...")
        return self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR32_READ, self.CMD_VERSION0, memtype]) + self._pack_sab(address) + binary.pack_le32(
                num_bytes)))

    def memory_write(self, memtype, address, data):
        """
        Write memory to target

        :param memtype: memory type / region to access
        :param address: start address
        :param data: data to write
        """
        data = bytearray(data)
        return self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR32_WRITE, self.CMD_VERSION0, memtype]) + self._pack_sab(address) + binary.pack_le32(
                len(data)) + data))

    def jtag_ir(self, bits, data):
        """
        Raw JTAG IR operation

        :param bits: number of bits to shift
        :param data: data to shift
        :return:
        """
        return self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR32_TAP, self.CMD_VERSION0, self.AVR32_TAP_IR, bits, data & 0xFF])))

    def jtag_dr(self, bits, data):
        """
        Raw JTAG DR operation

        :param bits: number of bits to shift
        :param data: data to shift
        :return: data shifted out
        """
        return self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR32_TAP, self.CMD_VERSION0, self.AVR32_TAP_DR, bits, data & 0xFF])))
