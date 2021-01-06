"""
UPDI NVM implementation
"""
from pyedbglib.protocols.avr8protocol import Avr8Protocol
from pyedbglib.protocols.housekeepingprotocol import Jtagice3HousekeepingProtocol
from pyedbglib.protocols.jtagice3protocol import Jtagice3ResponseError
from pyedbglib.util import binary

from . import utils
from .nvm import NvmAccessProviderCmsisDapAvr
from .pymcuprog_errors import PymcuprogDeviceLockedError
from .avr8target import TinyXAvrTarget
from .serialupdi.application import decode_sib
from .pymcuprog_errors import PymcuprogError
from .deviceinfo.deviceinfo import DeviceMemoryInfo
from .deviceinfo.deviceinfokeys import DeviceInfoKeysAvr, DeviceMemoryInfoKeys
from .deviceinfo.memorynames import MemoryNames

class NvmAccessProviderCmsisDapUpdi(NvmAccessProviderCmsisDapAvr):
    """
    NVM Access for AVR UPDI devices
    """

    def __init__(self, transport, device_info, frequency=None, options=""):
        NvmAccessProviderCmsisDapAvr.__init__(self, device_info)
        self.options = options
        self.avr = TinyXAvrTarget(transport)
        self.avr.setup_config(device_info)
        # Default to 900k if not specified
        if not frequency:
            frequency = 900000
        self.logger.info("UPDI baud rate: %dbps", frequency)

        # High-voltage activation?
        use_hv = Avr8Protocol.UPDI_HV_NONE
        if 'high-voltage' in self.options:
            if self.options['high-voltage'] == 'simple-unsafe-pulse':
                self.logger.info("Activating UPDI using simple high-voltage pulse")
                use_hv = Avr8Protocol.UPDI_HV_SIMPLE_PULSE
            elif self.options['high-voltage'] == 'tool-toggle-power':
                self.logger.info("Activating UPDI using high-voltage pulse with tool power-toggle")
                use_hv = Avr8Protocol.UPDI_HV_AUTO_POWER_TOGGLE
            elif self.options['high-voltage'] == 'user-toggle-power':
                self.logger.info("Activating UPDI using high-voltage pulse with user power-toggle")
                use_hv = Avr8Protocol.UPDI_HV_USER_POWER_TOGGLE

        if use_hv != Avr8Protocol.UPDI_HV_NONE:
            # Check special powers
            housekeeper = Jtagice3HousekeepingProtocol(transport)
            # Can this tool do high-voltage at all?
            abilities = housekeeper.query(Jtagice3HousekeepingProtocol.HOUSEKEEPING_QUERY_SPECIAL_ABILITIES)
            if not Jtagice3HousekeepingProtocol.HOUSEKEEPING_ABILITY_HV_UPDI_ENABLE in abilities:
                raise PymcuprogError("This tool does not have the ability to activate UPDI using high-voltage.")

        self.avr.setup_prog_session(khz=frequency // 1000, use_hv=use_hv)

    def start(self, user_interaction_callback=None):
        """
        Start (activate) session for UPDI targets

        :param user_interaction_callback: Callback to be called when user interaction is required,
            for example when doing UPDI high-voltage activation with user target power toggle.
            This function could ask the user to toggle power and halt execution waiting for the user
            to respond (this is default behavior if the callback is None), or if the user is another
            script it could toggle power automatically and then return.
        """
        self.logger.info("UPDI-specific initialiser")

        try:
            self.avr.activate_physical(user_interaction_callback=user_interaction_callback)
        except Jtagice3ResponseError as error:
            # The debugger could be out of sync with the target, retry
            if error.code == Avr8Protocol.AVR8_FAILURE_INVALID_PHYSICAL_STATE:
                self.logger.info("Physical state out of sync.  Retrying.")
                self.avr.deactivate_physical()
                self.avr.activate_physical(user_interaction_callback=user_interaction_callback)
            else:
                raise

        self.avr.sib_read()
        if 'chip-erase-locked-device' in self.options and self.options['chip-erase-locked-device']:
            self.logger.info("Activating chip-erase key entry mechanism")
            self.avr.protocol.set_byte(self.avr.protocol.AVR8_CTXT_OPTIONS,
                                       self.avr.protocol.AVR8_OPT_CHIP_ERASE_TO_ENTER, 1)

        # There is a lot that can go wrong here, but a few things can be accurately determined:
        try:
            self.avr.enter_progmode()
        except Jtagice3ResponseError as error:
            self.logger.warning(error)
            # Interesting failure code to intercept and handle gracefully
            if error.code == Avr8Protocol.AVR8_FAILURE_OCD_LOCKED:
                if 'user-row-locked-device' in self.options and self.options['user-row-locked-device']:
                    # log a warning because a) the previous log message is a warning and this should match and
                    # b) this is a special function flag which only applies to user row
                    # (but at this point we can't police that)
                    self.logger.warning("Proceding to write user row on a locked device...")
                    # return before raising exception - this causes the access to attempt to continue
                    # when using the special option flag for writing user row on locked devices.
                    return
                raise PymcuprogDeviceLockedError("Unable to enter programming mode: device is locked!")
            # Other exceptions can just propagate
            raise
        if "chip-erase-locked-device" in self.options:
            self.logger.info("Key chip-erase complete")

    def stop(self):
        """
        Stop (deactivate) session for UPDI targets
        """
        self.logger.info("UPDI-specific de-initialiser")
        self.avr.leave_progmode()
        self.avr.deactivate_physical()

    def read_device_id(self):
        """
        Read the device info

        :returns: Device ID raw bytes (Little endian)
        """
        sib = bytearray(self.avr.sib_read())
        decode_sib(sib)

        self.logger.info("PDI revision = 0x%02X",
                         self.avr.memory_read(Avr8Protocol.AVR8_MEMTYPE_CS, 0, 1)[0] >> 4)
        device_memory_info = DeviceMemoryInfo(self.device_info)
        signatures_info = device_memory_info.memory_info_by_name(MemoryNames.SIGNATURES)
        signatures_address = signatures_info[DeviceMemoryInfoKeys.ADDRESS]
        sig = self.avr.memory_read(self.avr.memtype_read_from_string("raw"),
                                   signatures_address,
                                   3)
        device_id_read = binary.unpack_be24(sig)

        self.logger.info("Device ID: '%06X'", device_id_read)
        if not self.device_info.get(DeviceInfoKeysAvr.DEVICE_ID) == device_id_read:
            self.logger.warning("ID read ('%06X') does not match expected device id! ('%06X')", device_id_read,
                                self.device_info.get(DeviceInfoKeysAvr.DEVICE_ID))

        revision = self.avr.memory_read(self.avr.memtype_read_from_string("raw"),
                                        self.device_info.get(DeviceInfoKeysAvr.SYSCFG_BASE) + 1, 1)
        self.logger.info("Device revision: '%s'", chr(revision[0] + ord('A')))

        # Return the raw signature bytes, but swap the endianness as target sends ID as Big endian
        return bytearray([sig[2], sig[1], sig[0]])

    def erase(self, memory_info=None, address=None):
        """
        Erase device, or parts of it

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class.
            If memory_info is None a chip_erase will be run
        :param address: address info for erase (optional)
        """
        memory_name = None

        if memory_info is None:
            erase_mode = Avr8Protocol.ERASE_CHIP
        else:
            memory_name = memory_info[DeviceMemoryInfoKeys.NAME]
            if memory_name == MemoryNames.EEPROM:
                erase_mode = Avr8Protocol.ERASE_EEPROM
            elif memory_name == MemoryNames.USER_ROW:
                erase_mode = Avr8Protocol.ERASE_USERSIG
            elif memory_name == MemoryNames.FLASH:
                erase_mode = Avr8Protocol.ERASE_APP_PAGE
            else:
                erase_mode = Avr8Protocol.ERASE_CHIP

        self.logger.info("Erasing")
        if address is None:
            address = 0

        if memory_name == MemoryNames.FLASH:
            page_size = memory_info[DeviceMemoryInfoKeys.PAGE_SIZE]
            n_pages = memory_info[DeviceMemoryInfoKeys.SIZE]//page_size
            self.logger.debug("Erasing %d pages of flash", n_pages)
            for i in range(n_pages):
                self.avr.erase(erase_mode, address=i*page_size)
        else:
            self.avr.erase(erase_mode, address)

    def write(self, memory_info, offset, data):
        """
        Write the memory with data

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset within the memory type
        :param data: the data to program
        :return: None
        """
        memtype_string = memory_info[DeviceMemoryInfoKeys.NAME]
        memtype = self.avr.memtype_write_from_string(memtype_string)
        if memtype == 0:
            msg = "Unsupported memory type: {}".format(memtype_string)
            self.logger.error(msg)
            raise PymcuprogError(msg)

        if memtype_string != MemoryNames.EEPROM:
            # For UPDI parts single byte access is enabled for EEPROM so no need to align to page boundaries
            data_to_write, address = utils.pagealign(data,
                                                     offset,
                                                     memory_info[DeviceMemoryInfoKeys.PAGE_SIZE],
                                                     memory_info[DeviceMemoryInfoKeys.WRITE_SIZE])
        else:
            data_to_write = data
            address = offset

        if memtype_string != MemoryNames.FLASH:
            # Flash is offset by the debugger config
            address += memory_info[DeviceMemoryInfoKeys.ADDRESS]

        allow_blank_skip = False
        if memtype_string in MemoryNames.FLASH:
            allow_blank_skip = True

        if memtype_string in (MemoryNames.FLASH, MemoryNames.EEPROM, MemoryNames.FUSES):
            # For Flash we have to write exactly one page but for EEPROM we could write less than one page,
            # but not more.  For fuses only one byte at a time can be written
            write_chunk_size = memory_info[DeviceMemoryInfoKeys.PAGE_SIZE]
            if memtype_string != MemoryNames.EEPROM:
                utils.pad_to_size(data_to_write, write_chunk_size, 0xFF)
        else:
            write_chunk_size = len(data_to_write)

        self.logger.info("Writing %d bytes of data in chunks of %d bytes to %s...",
                         len(data_to_write),
                         write_chunk_size,
                         memory_info[DeviceMemoryInfoKeys.NAME])

        first_chunk_size = write_chunk_size - address%write_chunk_size
        self.avr.write_memory_section(memtype,
                                      address,
                                      data_to_write[:first_chunk_size],
                                      write_chunk_size,
                                      allow_blank_skip=allow_blank_skip)
        address += first_chunk_size
        if len(data_to_write) > first_chunk_size:
            self.avr.write_memory_section(memtype,
                                          address,
                                          data_to_write[first_chunk_size:],
                                          write_chunk_size,
                                          allow_blank_skip=allow_blank_skip)

    def read(self, memory_info, offset, numbytes):
        """
        Read the memory in chunks

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset in the memory type
        :param numbytes: number of bytes to read
        :return: array of bytes read
        """
        memtype_string = memory_info[DeviceMemoryInfoKeys.NAME]
        memtype = self.avr.memtype_read_from_string(memtype_string)
        if memtype == 0:
            msg = "Unsupported memory type: {}".format(memtype_string)
            self.logger.error(msg)
            raise PymcuprogError(msg)

        if not memtype_string == MemoryNames.FLASH:
            # Flash is offset by the debugger config
            try:
                offset += memory_info[DeviceMemoryInfoKeys.ADDRESS]
            except TypeError:
                pass

        data = self.avr.read_memory_section(memtype, offset, numbytes, numbytes)
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
