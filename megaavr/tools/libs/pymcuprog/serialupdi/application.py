"""
Application layer for UPDI stack
"""
from logging import getLogger
from pymcuprog.pymcuprog_errors import PymcuprogError
from . import constants
from .link import UpdiDatalink16bit, UpdiDatalink24bit
from .nvm import NvmUpdi, NvmUpdiTinyMega, NvmUpdiAvrDx
from .readwrite import UpdiReadWrite
from .physical import UpdiPhysical
from .timeout import Timeout


def decode_sib(sib):
    """
    Turns the SIB into something readable
    :param sib: SIB data to decode
    """
    sib_info = {}
    logger = getLogger(__name__)
    
    sib = sib.replace(b"\x00", b"")

    try:
        sib_string = sib.decode('ascii')
    except UnicodeDecodeError:
        return None

    if len(sib_string) < 19:
        return None

    logger.info("SIB: '%s'", sib_string)

    # Parse fixed width fields according to spec
    family = sib[0:7].strip().decode()
    logger.info("Device family ID: '%s'", family)
    sib_info['family'] = family

    nvm = sib[8:11].strip().decode()
    logger.info("NVM interface: '%s'", nvm)
    sib_info['NVM'] = nvm.split(':')[1]

    ocd = sib[11:14].strip().decode()
    logger.info("Debug interface: '%s'", ocd)
    sib_info['OCD'] = ocd.split(':')[1]

    osc = sib[15:19].strip().decode()
    logger.info("PDI oscillator: '%s'", osc)
    sib_info['OSC'] = osc

    extra = sib[19:].strip().decode()
    logger.info("Extra info: '%s'", extra)
    sib_info['extra'] = extra

    return sib_info


class UpdiApplication:
    """
    Generic application layer for UPDI
    """

    def __init__(self, serialport, baud, device=None):
        self.logger = getLogger(__name__)
        self.device = device
        # Build the UPDI stack:
        # Create a physical

        baud_temp = min(baud, 115200)
        self.phy = UpdiPhysical(serialport, baud_temp)

        # Create a DL - use 16-bit until otherwise known
        datalink = UpdiDatalink16bit()

        # Set the physical for use in the datalink
        datalink.set_physical(self.phy)

        # Init (active) the datalink
        datalink.init_datalink()

        # set the actual baud
        datalink.change_baud(baud)

        # Create a read write access layer using this data link
        self.readwrite = UpdiReadWrite(datalink)

        # Create an NVM driver
        self.nvm = NvmUpdi(self.readwrite, self.device)

    def read_device_info(self):
        """
        Reads out device information from various sources
        """
        sib = self.readwrite.read_sib()
        sib_info = decode_sib(sib)

        if sib_info is None:
            self.logger.warning("Cannot read SIB, hard reset...")
            self.phy.send_double_break()
            sib = self.readwrite.read_sib()
            sib_info = decode_sib(sib)
            if sib_info is None:
                self.logger.error("Hard reset failed.")
                raise RuntimeError("Failed to read device info.")

        if sib_info['NVM'] == '2':
            # This is a Dx-family member, and needs new DL and NVM
            self.logger.info("Using 24-bit UPDI")
            # Create new DL
            datalink = UpdiDatalink24bit()
            # Use the existing PHY
            datalink.set_physical(self.phy)
            # And re-init
            datalink.init_datalink()
            # Create a read write access layer using this data link
            self.readwrite = UpdiReadWrite(datalink)
            # Create new NVM driver
            self.nvm = NvmUpdiAvrDx(self.readwrite, self.device)
        else:
            self.logger.info("Using 16-bit UPDI")
            # DL is correctly configured already
            # Create new NVM driver
            self.nvm = NvmUpdiTinyMega(self.readwrite, self.device)

        self.logger.info("PDI revision = 0x%02X", self.readwrite.read_cs(constants.UPDI_CS_STATUSA) >> 4)
        if self.in_prog_mode():
            if self.device is not None:
                devid = self.read_data(self.device.sigrow_address, 3)
                devrev = self.read_data(self.device.syscfg_address + 1, 1)
                self.logger.info("Device ID from pyupdi = '%02X%02X%02X' rev '%s'", devid[0], devid[1], devid[2],
                                 chr(ord('A') + devrev[0]))

    def read_data(self, address, size):
        """
        Reads a number of bytes of data from UPDI
        :param address: address to write to
        :param size: number of bytes to read
        """
        return self.readwrite.read_data(address, size)

    def read_data_words(self, address, words):
        """
        Reads a number of words of data from UPDI
        :param address: address to write to
        :param words: number of words to read
        """
        return self.readwrite.read_data_words(address, words)

    def write_data_words(self, address, data):
        """
        Writes a number of words to memory
        :param address: address to write to
        :param data: data to write
        """
        return self.readwrite.write_data_words(address, data)

    def write_data(self, address, data):
        """
        Writes a number of bytes to memory
        :param address: address to write to
        :param data: data to write
        """
        return self.write_data(address, data)

    def in_prog_mode(self):
        """
        Checks whether the NVM PROG flag is up
        """
        if self.readwrite.read_cs(constants.UPDI_ASI_SYS_STATUS) & (1 << constants.UPDI_ASI_SYS_STATUS_NVMPROG):
            return True
        return False

    def wait_unlocked(self, timeout_ms):
        """
        Waits for the device to be unlocked.
        All devices boot up as locked until proven otherwise
        :param timeout_ms: number of milliseconts to wait
        """
        timeout = Timeout(timeout_ms)

        while not timeout.expired():
            if not self.readwrite.read_cs(constants.UPDI_ASI_SYS_STATUS) & (
                    1 << constants.UPDI_ASI_SYS_STATUS_LOCKSTATUS):
                return True

        self.logger.error("Timeout waiting for device to unlock")
        return False

    def unlock(self):
        """
        Unlock by chip erase
        """
        # Put in the key
        self.readwrite.write_key(constants.UPDI_KEY_64, constants.UPDI_KEY_CHIPERASE)

        # Check key status
        key_status = self.readwrite.read_cs(constants.UPDI_ASI_KEY_STATUS)
        self.logger.debug("Key status = 0x%02X", key_status)

        if not key_status & (1 << constants.UPDI_ASI_KEY_STATUS_CHIPERASE):
            raise PymcuprogError("Key not accepted")

        # Toggle reset
        self.reset(apply_reset=True)
        self.reset(apply_reset=False)

        # And wait for unlock
        if not self.wait_unlocked(100):
            raise PymcuprogError("Failed to chip erase using key")

    def enter_progmode(self, chip_erase_locked_device=False):
        """
        Enters into NVM programming mode
        """
        # First check if NVM is already enabled
        if self.in_prog_mode():
            self.logger.info("Already in NVM programming mode")
            return True

        self.logger.info("Entering NVM programming mode")

        # Put in the key
        self.readwrite.write_key(constants.UPDI_KEY_64, constants.UPDI_KEY_NVM)

        # Check key status
        key_status = self.readwrite.read_cs(constants.UPDI_ASI_KEY_STATUS)
        self.logger.debug("Key status = 0x%02X", key_status)

        if not key_status & (1 << constants.UPDI_ASI_KEY_STATUS_NVMPROG):
            self.logger.error("Key status = 0x%02X", key_status)
            raise IOError("Key not accepted")

        # Toggle reset
        self.reset(apply_reset=True)
        self.reset(apply_reset=False)

        # And wait for unlock
        if not self.wait_unlocked(100):
            if chip_erase_locked_device:
                self.logger.error("Device locked, performing chip erase to unlock")
                self.unlock()
                self.enter_progmode() # need to retry, since in_prog_mode() will fail presently
                return
            raise IOError("Failed to enter NVM programming mode: device is locked")

        # Check for NVMPROG flag
        if not self.in_prog_mode():
            raise IOError("Failed to enter NVM programming mode")

        self.logger.debug("Now in NVM programming mode")
        return True

    def leave_progmode(self):
        """
        Disables UPDI which releases any keys enabled
        """
        self.logger.info("Leaving NVM programming mode")
        self.reset(apply_reset=True)
        self.reset(apply_reset=False)
        self.readwrite.write_cs(constants.UPDI_CS_CTRLB,
                                (1 << constants.UPDI_CTRLB_UPDIDIS_BIT) | (1 << constants.UPDI_CTRLB_CCDETDIS_BIT))

    def reset(self, apply_reset):
        """
        Applies or releases an UPDI reset condition
        :param apply_reset: True to apply, False to release
        """
        if apply_reset:
            self.logger.info("Apply reset")
            self.readwrite.write_cs(constants.UPDI_ASI_RESET_REQ, constants.UPDI_RESET_REQ_VALUE)
        else:
            self.logger.info("Release reset")
            self.readwrite.write_cs(constants.UPDI_ASI_RESET_REQ, 0x00)
