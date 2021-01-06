"""
CMSIS DAP access protocol

Interfaces with CMSIS-DAP standard debuggers over HID
"""

import time
from logging import getLogger

from .dapwrapper import DapWrapper
from ..util import binary
from ..pyedbglib_errors import PyedbglibError


class CmsisDapUnit(DapWrapper):
    """Communicates with a DAP via standard CMSIS-DAP firmware stack over HID transport"""

    # DAP command constants
    ID_DAP_Info = 0x00
    ID_DAP_HostStatus = 0x01
    ID_DAP_Connect = 0x02
    ID_DAP_Disconnect = 0x03
    ID_DAP_TransferConfigure = 0x04
    ID_DAP_Transfer = 0x05
    ID_DAP_TransferBlock = 0x06
    ID_DAP_TransferAbort = 0x07
    ID_DAP_WriteABORT = 0x08
    ID_DAP_Delay = 0x09
    ID_DAP_ResetTarget = 0x0A
    ID_DAP_SWJ_Pins = 0x10
    ID_DAP_SWJ_Clock = 0x11
    ID_DAP_SWJ_Sequence = 0x12
    ID_DAP_SWD_Configure = 0x13
    ID_DAP_JTAG_Sequence = 0x14
    ID_DAP_JTAG_Configure = 0x15
    ID_DAP_JTAG_IDCODE = 0x16

    # DAP responses
    DAP_OK = 0x00
    DAP_ERROR = 0xff

    # DAP info fields
    DAP_ID_VENDOR = 0x01
    DAP_ID_PRODUCT = 0x02
    DAP_ID_SER_NUM = 0x03
    DAP_ID_FW_VER = 0x04
    DAP_ID_DEVICE_VENDOR = 0x05
    DAP_ID_DEVICE_NAME = 0x06
    DAP_ID_CAPABILITIES = 0xF0
    DAP_ID_PACKET_COUNT = 0xFE
    DAP_ID_PACKET_SIZE = 0xFF

    # DAP ports
    DAP_PORT_AUTODETECT = 0
    DAP_PORT_DISABLED = 0
    DAP_PORT_SWD = 1
    DAP_PORT_JTAG = 2

    def __init__(self, transport):
        self.logger = getLogger(__name__)
        DapWrapper.__init__(self, transport)

    def _check_response(self, cmd, rsp):
        """
        Checks that the response echoes the command

        :param cmd: command going in
        :param rsp: response coming out
        """
        self.logger.debug("Checking response: cmd=0x%02X rsp=0x%02X", cmd[0], rsp[0])
        if cmd[0] != rsp[0]:
            raise PyedbglibError("Invalid response header")

    def dap_info(self):
        """Collects the dap info"""
        info = {
            'vendor': self._dap_info_field(self.DAP_ID_VENDOR),
            'product': self._dap_info_field(self.DAP_ID_PRODUCT),
            'serial': self._dap_info_field(self.DAP_ID_SER_NUM),
            'fw': self._dap_info_field(self.DAP_ID_FW_VER),
            'device_vendor': self._dap_info_field(self.DAP_ID_DEVICE_VENDOR),
            'device_name': self._dap_info_field(self.DAP_ID_DEVICE_NAME),
            'capabilities': self._dap_info_field(self.DAP_ID_CAPABILITIES)
        }
        return info

    def _dap_info_field(self, field):
        """
        Queries one field from the dap info

        :param field: which field to query
        """
        self.logger.debug("dap_info (%d)", field)
        cmd = bytearray(2)
        cmd[0] = self.ID_DAP_Info
        cmd[1] = field
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        return (rsp[2:rsp[1] + 2].decode()).strip('\0')

    def dap_led(self, index, state):
        """
        Operates the LED

        :param index: which led
        :param state: what to do with it
        :return:
        """
        self.logger.debug("dap_led (%d, %d)", index, state)
        cmd = bytearray(3)
        cmd[0] = self.ID_DAP_HostStatus
        cmd[1] = index
        cmd[2] = state
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)

    def dap_connect(self):
        """Connects to the DAP"""
        self.logger.debug("dap_connect (SWD)")
        cmd = bytearray(2)
        cmd[0] = self.ID_DAP_Connect
        cmd[1] = self.DAP_PORT_SWD
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_PORT_SWD:
            raise PyedbglibError("Connect failed (0x{0:02X})".format(rsp[1]))

    def dap_disconnect(self):
        """Disconnects from the DAP"""
        self.logger.debug("dap_disconnect")
        cmd = bytearray(1)
        cmd[0] = self.ID_DAP_Disconnect
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)


class CmsisDapDebugger(CmsisDapUnit):
    """ARM-specific cmsis-dap implementation"""

    # SWJ pin IDs
    DAP_SWJ_SWCLK_TCK = (1 << 0)
    DAP_SWJ_SWDIO_TMS = (1 << 1)
    DAP_SWJ_TDI = (1 << 2)
    DAP_SWJ_TDO = (1 << 3)
    DAP_SWJ_nTRST = (1 << 5)
    DAP_SWJ_nRESET = (1 << 7)

    # DAP transfer types
    DAP_TRANSFER_APnDP = (1 << 0)
    DAP_TRANSFER_RnW = (1 << 1)
    DAP_TRANSFER_A2 = (1 << 2)
    DAP_TRANSFER_A3 = (1 << 3)
    DAP_TRANSFER_MATCH_VALUE = (1 << 4)
    DAP_TRANSFER_MATCH_MASK = (1 << 5)

    # DAP transfer responses
    DAP_TRANSFER_INVALID = 0
    DAP_TRANSFER_OK = (1 << 0)
    DAP_TRANSFER_WAIT = (1 << 1)
    DAP_TRANSFER_FAULT = (1 << 2)
    DAP_TRANSFER_ERROR = (1 << 3)
    DAP_TRANSFER_MISMATCH = (1 << 4)

    # DP definitions
    DP_IDCODE = 0x00
    DP_ABORT = 0x00
    DP_CTRL_STAT = 0x04
    DP_WCR = 0x04
    DP_SELECT = 0x08
    DP_RESEND = 0x08
    DP_RDBUFF = 0x0C

    # JTAG-specific codes
    JTAG_ABORT = 0x08
    JTAG_DPACC = 0x0A
    JTAG_APACC = 0x0B
    JTAG_IDCODE = 0x0E
    JTAG_BYPASS = 0x0F

    # SWD-specific codes
    SWD_AP_CSW = 0x00
    SWD_AP_TAR = 0x04
    SWD_AP_DRW = 0x0C

    # TAR size
    TAR_MAX = 0x400

    # DAP CTRL_STAT bits
    # Source: Coresight Techref
    CSYSPWRUPACK = (1 << 31)
    CSYSPWRUPREQ = (1 << 30)
    CDBGPWRUPACK = (1 << 29)
    CDBGPWRUPREQ = (1 << 28)
    CDBGRSTACK = (1 << 27)
    CDBGRSTREQ = (1 << 26)
    WDATAERR = (1 << 7)
    READOK = (1 << 6)
    STICKYERR = (1 << 5)
    STICKYCMP = (1 << 4)
    TRNMODE = (1 << 2)
    STICKYORUN = (1 << 1)
    ORUNDETECT = (1 << 0)

    # Useful CSW settings
    CSW_32BIT = 0x02
    CSW_16BIT = 0x01
    CSW_8BIT = 0x00
    CSW_ADDRINC_OFF = 0x00
    CSW_ADDRINC_ON = (1 << 4)

    # Supported DAP IDs.
    CM0P_DAPID = 0x0BC11477

    def __init__(self, transport):
        self.logger = getLogger(__name__)
        CmsisDapUnit.__init__(self, transport)

    def dap_swj_clock(self, clock):
        """
        Sets up the SWD clock timing

        :param clock: clock value in Hz
        """
        self.logger.debug("dap_swj_clk (%d)", clock)
        cmd = bytearray(1)
        cmd[0] = self.ID_DAP_SWJ_Clock
        cmd.extend(binary.pack_le32(clock))
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_OK:
            raise PyedbglibError("SWJ clock setting failed (0x{0:02X})".format(rsp[1]))

    def dap_transfer_configure(self, idle, count, retry):
        """
        Configures SWD transfers

        :param idle: idle cycles
        :param count: retry count
        :param retry: match retry value
        :return:
        """
        self.logger.debug("dap_transfer_configure (%d, %d, %d)", idle, count, retry)
        cmd = bytearray(2)
        cmd[0] = self.ID_DAP_TransferConfigure
        cmd[1] = idle
        cmd.extend(binary.pack_le16(count))
        cmd.extend(binary.pack_le16(retry))
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_OK:
            raise PyedbglibError("Transfer configure failed (0x{0:02X})".format(rsp[1]))

    def dap_swd_configure(self, cfg):
        """
        Configures the SWD interface

        :param cfg: turnaround and data phase config parameters
        """
        self.logger.debug("dap_swd_configure (%d)", cfg)
        cmd = bytearray(2)
        cmd[0] = self.ID_DAP_SWD_Configure
        cmd[1] = cfg
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_OK:
            raise PyedbglibError("SWD configure failed (0x{0:02X})".format(rsp[1]))

    def dap_reset_target(self):
        """Reset the target using the DAP"""
        self.logger.debug("dap_reset_target")
        cmd = bytearray(1)
        cmd[0] = self.ID_DAP_ResetTarget
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_OK:
            raise PyedbglibError("Reset target failed (0x{0:02X})".format(rsp[1]))

    def dap_read_reg(self, reg):
        """
        Reads a DAP AP/DP register

        :param reg: register to read
        """
        self.logger.debug("dap_read_reg (0x%02X)", reg)
        cmd = bytearray(8)
        cmd[0] = self.ID_DAP_Transfer
        cmd[1] = 0x00  # dap
        cmd[2] = 0x01  # 1 word
        cmd[3] = reg | self.DAP_TRANSFER_RnW
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != 1 or rsp[2] != self.DAP_TRANSFER_OK:
            raise PyedbglibError("Read reg failed (0x{0:02X}, {1:02X})".format(rsp[1], rsp[2]))
        value = binary.unpack_le32(rsp[3:7])
        return value

    def dap_write_reg(self, reg, value):
        """
        Writes a DAP AP/DP register

        :param reg: register to write
        :param value: value to write
        """
        self.logger.debug("dap_write_reg (0x%02X) = 0x%08X", reg, value)
        cmd = bytearray(4)
        cmd[0] = self.ID_DAP_Transfer
        cmd[1] = 0x00  # dap
        cmd[2] = 0x01  # 1 word
        cmd[3] = reg
        cmd.extend(binary.pack_le32(value))
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != 1 or rsp[2] != self.DAP_TRANSFER_OK:
            raise PyedbglibError("Write reg failed (0x{0:02X}, {1:02X})".format(rsp[1], rsp[2]))

    def read_word(self, address):
        """
        Reads a word from the device memory bus

        :param address: address to read
        """
        self.logger.debug("read word at 0x%08X", address)
        self.dap_write_reg(self.SWD_AP_TAR | self.DAP_TRANSFER_APnDP, address)
        return self.dap_read_reg(self.SWD_AP_DRW | self.DAP_TRANSFER_APnDP)

    def write_word(self, address, data):
        """
        Writes a word to the device memory bus

        :param address: address to write
        :param data: data to write
        """
        self.logger.debug("write word at 0x%08X = 0x%08X", address, data)
        self.dap_write_reg(self.SWD_AP_TAR | self.DAP_TRANSFER_APnDP, address)
        self.dap_write_reg(self.SWD_AP_DRW | self.DAP_TRANSFER_APnDP, data)

    @staticmethod
    def multiple_of_four(x):
        """ 4 byte boundary """
        return x & ~0x03

    def read_block(self, address, numbytes):
        """
        Reads a block from the device memory bus

        :param address: byte address
        :param numbytes: number of bytes
        """
        self.logger.debug("Block read of %d bytes at address 0x%08X", numbytes, address)
        # Collect results here
        result = bytearray()
        # In chunks of (len-header)
        max_payload_size_bytes = self.multiple_of_four(self.transport.get_report_size() - 5)
        self.logger.debug("Max payload size of %d bytes", max_payload_size_bytes)
        while numbytes:
            # Calculate read size
            read_size_bytes = max_payload_size_bytes

            # Last chunk?
            if read_size_bytes > numbytes:
                read_size_bytes = numbytes

            # Too large for TAR?
            tar_max_chunk = self.TAR_MAX - (address - (address & (1-self.TAR_MAX)))
            if read_size_bytes > tar_max_chunk:
                read_size_bytes = tar_max_chunk

            # Log
            self.logger.debug("Read %d bytes from TAR address 0x%08X", read_size_bytes, address)

            # Set TAR
            self.dap_write_reg(self.SWD_AP_TAR | self.DAP_TRANSFER_APnDP, address)

            # Read chunk
            cmd = bytearray(2)
            cmd[0] = self.ID_DAP_TransferBlock
            cmd[1] = 0x00
            cmd.extend(binary.pack_le16(read_size_bytes // 4))
            cmd.extend([self.SWD_AP_DRW | self.DAP_TRANSFER_RnW | self.DAP_TRANSFER_APnDP])

            rsp = self.dap_command_response(cmd)
            self._check_response(cmd, rsp)

            # Check outcome
            if rsp[3] != self.DAP_TRANSFER_OK:
                raise PyedbglibError("Transfer failed (0x{0:02X}) address 0x{1:08X}".format(rsp[3], address))

            # Extract payload
            num_words_read = binary.unpack_le16(rsp[1:3])

            # Check
            if num_words_read * 4 != read_size_bytes:
                raise PyedbglibError(
                    "Unexpected number of bytes returned from block read ({0:d} != {1:d})".format(num_words_read * 4,
                                                                                                  read_size_bytes))

            # Extend results
            result.extend(rsp[4:4 + read_size_bytes])
            numbytes -= read_size_bytes
            address += read_size_bytes

        return result

    def write_block(self, address, data):
        """
        Writes a block to the device memory bus

        :param address: byte address
        :param data: data
        """
        self.logger.debug("Block write of %d bytes at address 0x%08X", len(data), address)

        # In chunks of (len-header)
        max_payload_size_bytes = self.multiple_of_four(self.transport.get_report_size() - 5)
        while data:
            # Calculate write size
            write_size_bytes = max_payload_size_bytes
            if write_size_bytes > len(data):
                write_size_bytes = len(data)

            # Too large for TAR?
            tar_max_chunk = self.TAR_MAX - (address - (address & (1 - self.TAR_MAX)))
            if write_size_bytes > tar_max_chunk:
                write_size_bytes = tar_max_chunk

            # Set TAR
            self.dap_write_reg(self.SWD_AP_TAR | self.DAP_TRANSFER_APnDP, address)

            cmd = bytearray(2)
            cmd[0] = self.ID_DAP_TransferBlock
            cmd[1] = 0x00
            cmd.extend(binary.pack_le16(write_size_bytes // 4))
            cmd.extend([self.SWD_AP_DRW | self.DAP_TRANSFER_APnDP])
            cmd.extend(data[0:write_size_bytes])
            rsp = self.dap_command_response(cmd)
            self._check_response(cmd, rsp)

            # Shrink data buffer
            data = data[write_size_bytes:]
            address += write_size_bytes

    def _send_flush_tms(self):
        cmd = bytearray(2)
        cmd[0] = self.ID_DAP_SWJ_Sequence
        cmd[1] = 7 * 8
        for _ in range(7):
            cmd.extend([0xff])
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_OK:
            raise PyedbglibError("SWJ sequence failed (0x{0:02X})".format(rsp[1]))

    def init_swj(self):
        """Magic sequence to execute on pins to enable SWD in case of JTAG-default parts"""
        self.logger.debug("SWJ init sequence")
        # According to ARM manuals:
        # Send at least 50 cycles with TMS=1
        self._send_flush_tms()

        # Send 16-bit switching code
        cmd = bytearray(2)
        cmd[0] = self.ID_DAP_SWJ_Sequence
        cmd[1] = 16
        cmd.extend(binary.pack_le16(0xE79E))
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_OK:
            raise PyedbglibError("SWJ sequence failed (0x{0:02X})".format(rsp[1]))

        # Flush TMS again
        self._send_flush_tms()

        # Set data low again
        cmd = bytearray(3)
        cmd[0] = self.ID_DAP_SWJ_Sequence
        cmd[1] = 1
        cmd[2] = 0x00
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)
        if rsp[1] != self.DAP_OK:
            raise PyedbglibError("SWJ sequence failed (0x{0:02X})".format(rsp[1]))

        # Now read the ID to check that it has switched
        dap_id = self.dap_read_idcode()
        if dap_id != self.CM0P_DAPID:
            raise PyedbglibError("Invalid SWD DAP ID code! Only M0+ is currently supported.")

    def dap_read_idcode(self):
        """Reads the IDCODE from the SWD DP"""
        self.logger.debug("reading swd idcode")
        return self.dap_read_reg(self.DP_IDCODE)

    def dap_target_init(self):
        """Configures the DAP for use"""
        self.logger.debug("dap_target_init")
        # Clear all stickies
        self.dap_write_reg(self.DP_ABORT, self.STICKYERR | self.STICKYCMP | self.STICKYORUN)
        # Select to 0
        self.dap_write_reg(self.DP_SELECT, 0)
        # Request debug power
        self.dap_write_reg(self.DP_CTRL_STAT, self.CDBGPWRUPREQ | self.CSYSPWRUPREQ)
        # Most useful default of 32-bit word access with auto-increment enabled
        self.dap_write_reg(self.SWD_AP_CSW | self.DAP_TRANSFER_APnDP, self.CSW_ADDRINC_ON | self.CSW_32BIT)


class CmsisDapSamDebugger(CmsisDapDebugger):
    """SAM specific CMSIS-DAP debugger"""

    def dap_reset_ext(self, extend=False):
        """
        Reset the target using the hardware

        Some SAM devices (for example SAMDx and SAMLx) have an additional 'reset extension' capability which is not part
        of the CMSIS-DAP standard.  It is used to prevent the device from running after reset and then overriding its
        SWD IO.  The procedure is simply to hold SW_CLK low while releasing /RESET.  This is done here using SWJ pins
        function IF the extend argument is set.

        :param extend: boolean flag to extend reset
        """
        self.logger.debug("dap_reset_ext")
        cmd = bytearray(7)
        cmd[0] = self.ID_DAP_SWJ_Pins
        cmd[1] = 0  # Reset LOW, TCK LOW
        cmd[2] = self.DAP_SWJ_nRESET
        if extend:
            cmd[2] |= self.DAP_SWJ_SWCLK_TCK
        cmd[3] = 0
        cmd[4] = 0
        cmd[5] = 0
        cmd[6] = 0
        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)

        cmd[1] = self.DAP_SWJ_nRESET  # Reset high, TCK still low
        cmd[2] = self.DAP_SWJ_nRESET
        if extend:
            cmd[2] |= self.DAP_SWJ_SWCLK_TCK

        rsp = self.dap_command_response(cmd)
        self._check_response(cmd, rsp)

        # Allow Reset to be pulled high
        time.sleep(0.1)
