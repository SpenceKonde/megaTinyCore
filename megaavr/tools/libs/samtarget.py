"""
Access to SAM devices via CMSIS-DAP

Transport provided by HID inside pyedbglib
"""
import time
from logging import getLogger

from pyedbglib.protocols.cmsisdap import CmsisDapSamDebugger

from .pymcuprog_errors import PymcuprogError

class SamTarget:
    """
    General SAM target interface
    """
    def __init__(self, driver):
        self.debugger = CmsisDapSamDebugger(driver)
        self.logger = getLogger(__name__)

    def read_idcode(self):
        """
        Reads the ID code from the DAP (not device ID)
        """
        return self.debugger.dap_read_idcode()

    def dp_write(self, reg, value):
        """
        Debug Port write

        :param reg: register to write
        :param value: value to write
        """
        return self.debugger.dap_write_reg(reg >> 2, value)

    def dp_read(self, reg):
        """
        Debug Port read

        :param reg: register to read
        :return: value read
        """
        return self.debugger.dap_read_reg(reg >> 2)

    def ap_write(self, reg, value):
        """
        Access Port write

        :param reg: register to write
        :param value: value to write
        """
        return self.debugger.dap_write_reg(4 + (reg >> 2), value)

    def ap_read(self, reg):
        """
        Access port read

        :param reg: register to read
        :return: value read
        """
        return self.debugger.dap_read_reg(4 + ((reg & 0x0F) >> 2))


class SamD2xTarget(SamTarget):
    """
    Programmer for SAMD21 and friends
    """
    # DSU
    DSU_ADDRESS = 0x41002000
    DSU_EXTERNAL_OFFSET = 0x100
    DSU_CTRL_OFFSET = 0x00
    DSU_DID_OFFSET = 0x18
    DSU_CTRL_STATUS_CE_COMMAND_MASK = 0x10
    DSU_CTRL_STATUS_DONE_MASK = (1 << 8)
    DSU_CTRL_STATUS_PROT_MASK = (1 << 16)

    # Timeout for chip erase
    DSU_CHIP_ERASE_TIMEOUT_MS = 3000

    # Flash controller
    NVM_CTRL_ADDRESS = 0x41004000
    NVM_CTRL_CTRL_OFFSET = 0x00
    NVM_CTRL_CTRLB_OFFSET = 0x04
    NVM_CTRL_ADDR_OFFSET = 0x1C
    NVM_CTRL_INTFLAG_OFFSET = 0x14
    NVM_CTRL_STATUS_OFFSET = 0x18
    NVM_CTRL_LOCK_OFFSET = 0x20

    CMD_WP = 0x04
    CMD_EAR = 0x05
    CMD_WAP = 0x06
    CMD_LR = 0x40
    CMD_UR = 0x41
    CMD_PBC = 0x44

    NVM_CTRLB_MANW_BIT = 7

    NVM_INT_READY_BIT = 0
    NVM_INT_ERROR_BIT = 1

    NVM_STATUS_SB = 8
    NVM_STATUS_NVME = 4
    NVM_STATUS_LOCKE = 3
    NVM_STATUS_PROGE = 2

    DAP_TRANSFER_IDLE_CYCLES = 0
    DAP_TRANSFER_RETRY_COUNT = 250
    DAP_TRANSFER_MATCH_RETRY = 0

    def __init__(self, driver):
        SamTarget.__init__(self, driver)
        self.logger = getLogger(__name__)

    def connect(self, clk_hz):
        """
        Connect to the DAP and initialise basics
        """
        self.logger.info("Connecting to SAMD2x DAP")
        self.debugger.dap_connect()
        self.debugger.dap_reset_ext(True)
        self.debugger.dap_swj_clock(50000)
        self.debugger.dap_transfer_configure(self.DAP_TRANSFER_IDLE_CYCLES, self.DAP_TRANSFER_RETRY_COUNT,
                                             self.DAP_TRANSFER_MATCH_RETRY)
        self.debugger.dap_swd_configure(0)

        self.logger.info("Using SWD CLK of %d Hz", clk_hz)
        self.debugger.dap_swj_clock(clk_hz)

        self.debugger.init_swj()
        self.debugger.dap_target_init()

    def reinitialise(self):
        """
        Re-initialise

        Required after certain operations which reset the DAP on the target
        """
        self.logger.info("Re-init of SAMD2x DAP")
        self.debugger.dap_connect()
        self.debugger.dap_reset_ext(True)
        self.debugger.dap_swd_configure(0)
        self.debugger.init_swj()
        self.debugger.dap_target_init()

    def disconnect(self):
        """
        Disconnect from the DAP
        """
        # External reset with no reset extension
        self.debugger.dap_reset_ext(False)
        self.debugger.dap_disconnect()

    def read_device_id(self):
        """
        Read the device ID register

        :returns: Device ID raw bytes (Little endian)
        """
        return self.debugger.read_word(self.DSU_ADDRESS + self.DSU_EXTERNAL_OFFSET + self.DSU_DID_OFFSET)

    def is_flash_ready(self):
        """
        Checks if Flash Controller is ready

        :return: boolean ready state
        """
        reg = self.debugger.read_word(self.NVM_CTRL_ADDRESS + self.NVM_CTRL_INTFLAG_OFFSET)
        # Check for error bit
        if reg & (1 << self.NVM_INT_ERROR_BIT):
            # Find out what's causing the error
            error_cause = self.debugger.read_word(self.NVM_CTRL_ADDRESS + self.NVM_CTRL_STATUS_OFFSET)
            self.logger.error("NVM error: status = 0x%02X", error_cause)
            if error_cause & (1 << self.NVM_STATUS_SB):
                error_message = "Security bit is set"
            if error_cause & (1 << self.NVM_STATUS_NVME):
                error_message = "NVM controller error"
            if error_cause & (1 << self.NVM_STATUS_LOCKE):
                error_message = "Region is locked"
            if error_cause & (1 << self.NVM_STATUS_PROGE):
                error_message = "Invalid NVM command"
            raise PymcuprogError("Flash error: ({})".format(error_message))
        # Check ready bit
        return reg & (1 << self.NVM_INT_READY_BIT)

    def is_device_locked(self):
        """
        Checks if the device is locked
        """
        self.logger.info("Is device locked?")
        locked = self.debugger.read_word(self.DSU_ADDRESS + self.DSU_EXTERNAL_OFFSET + self.DSU_CTRL_OFFSET)
        if locked & self.DSU_CTRL_STATUS_PROT_MASK:
            return True
        return False

    def wait_flash_ready(self, timeout_ms=1000):
        """
        Wait for flash to be ready, or timeout

        :param timeout_ms:
        :return:
        """
        while timeout_ms > 0:
            # Done?
            if self.is_flash_ready():
                return True
            # Wait 1ms
            time.sleep(0.001)
            timeout_ms -= 1
        return False

    def read_flash(self, address, numbytes):
        """
        Read flash content

        :param address: start byte address to read from
        :param numbytes: number of bytes to read
        :return:
        """
        if numbytes % 4:
            raise PymcuprogError("Invalid read size")
        # Wait for flash to be ready
        self.wait_flash_ready()

        if bytes == 4:
            return self.debugger.read_word(address)
        return self.debugger.read_block(address, numbytes)

    def chip_erase_dsu(self):
        """
        Perform a chip erase using the DSU
        """
        self.logger.info("Chip erase via DSU")
        transfer_data = self.DSU_CTRL_STATUS_CE_COMMAND_MASK
        self.debugger.write_word(self.DSU_ADDRESS + self.DSU_EXTERNAL_OFFSET + self.DSU_CTRL_OFFSET, transfer_data)
        timeout = self.DSU_CHIP_ERASE_TIMEOUT_MS
        while timeout:
            reg = self.debugger.read_word(self.DSU_ADDRESS + self.DSU_EXTERNAL_OFFSET + self.DSU_CTRL_OFFSET)
            if reg & self.DSU_CTRL_STATUS_DONE_MASK:
                self.wait_flash_ready()
                return
            timeout -= 1
        raise PymcuprogError("Timeout erasing device via DSU!")

    def nvm_command(self, command):
        """
        Write command in NVM controller

        :param command: command to write
        """
        self.debugger.write_word(self.NVM_CTRL_ADDRESS, (0xA5 << 8) | command)
        self.wait_flash_ready()

    def set_nvmctrl_address(self, address):
        """
        Set address in NVM controller

        :param address: address to write
        """
        # Divide 8-bit address by 2, according to the datasheet
        self.debugger.write_word(self.NVM_CTRL_ADDRESS + self.NVM_CTRL_ADDR_OFFSET, address >> 1)

    def write_flash_page(self, data_buffer, address):
        """
        Write a page of flash memory

        :param data_buffer: data to write
        :param address: address to write to
        """
        # Wait for flash to be ready
        self.wait_flash_ready()
        # Disable auto-commit
        self.debugger.write_word(self.NVM_CTRL_ADDRESS + self.NVM_CTRL_CTRLB_OFFSET, (1 << self.NVM_CTRLB_MANW_BIT))

        # Write address
        self.set_nvmctrl_address(address)
        # Apply "Unlock Region" command
        self.nvm_command(self.CMD_UR)

        # Clear page buffer?  Not required
        # self.nvm_command(self.CMD_PBC)

        # Write data
        self.debugger.write_block(address, data_buffer)

        # Commit to flash
        self.nvm_command(self.CMD_WP)
        # Wait for ready
        self.wait_flash_ready()

    def erase_user_row(self, address):
        """
        Erase the user row

        :param address: address to erase
        :return:
        """
        # Wait for flash to be ready
        self.wait_flash_ready()
        # Insert address to erase
        self.set_nvmctrl_address(address)
        # Apply "unlock region" command
        self.nvm_command(self.CMD_UR)
        # Apply Erase Auxilliary Row command
        self.nvm_command(self.CMD_EAR)

    def write_user_row_word(self, address, value):
        """
        Write a word the user row

        :param address: address of the user row
        :param value: value to write
        """
        self.wait_flash_ready()
        self.set_nvmctrl_address(address)
        self.nvm_command(self.CMD_UR)
        self.debugger.write_word(address, value)
        self.nvm_command(self.CMD_WAP)

    def read_user_row(self, address, numbytes):
        """
        Read user row values

        :param address: address to read from
        :param numbytes: number of bytes to read
        :return: bytes read
        """
        self.wait_flash_ready()
        return self.read_flash(address, numbytes)


class SamM4Target(SamTarget):
    """
    SAM M4 targets.

    SAM M4 targets are not supported
    """
    def __init__(self, driver):
        SamTarget.__init__(self, driver)
        self.logger = getLogger(__name__)
        self.logger.error("SAM M4 targets are not supported")
        raise NotImplementedError("SAM M4 targets are not supported")
