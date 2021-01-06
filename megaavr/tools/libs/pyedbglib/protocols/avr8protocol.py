"""
Implements Avr8Protocol, a sub-protocol in the JTAGICE3 family of protocols.
"""

from logging import getLogger
from .jtagice3protocol import Jtagice3Protocol
from .avr8protocolerrors import AVR8_ERRORS
from ..util import binary


class Avr8Protocol(Jtagice3Protocol):
    """
    Implements AVR8 protocol functionality on the JTAGICE3 protocol family.
    """

    # Command IDs
    CMD_AVR8_QUERY = 0x00  # Capability discovery
    CMD_AVR8_SET = 0x01  # Set parameters
    CMD_AVR8_GET = 0x02  # Get parameters
    CMD_AVR8_ACTIVATE_PHYSICAL = 0x10  # Connect physically
    CMD_AVR8_DEACTIVATE_PHYSICAL = 0x11  # Disconnect physically
    CMD_AVR8_GET_ID = 0x12  # Read the ID
    CMD_AVR8_ATTACH = 0x13  # Attach to OCD module
    CMD_AVR8_DETACH = 0x14  # Detach from OCD module
    CMD_AVR8_PROG_MODE_ENTER = 0x15  # Enter programming mode
    CMD_AVR8_PROG_MODE_LEAVE = 0x16  # Leave programming mode
    CMD_AVR8_DISABLE_DEBUGWIRE = 0x17  # Disable debugWIRE interface
    CMD_AVR8_ERASE = 0x20  # Erase the chip
    CMD_AVR8_MEMORY_READ = 0x21  # Read memory
    CMD_AVR8_MEMORY_READ_MASKED = 0x22  # Read memory while via a mask
    CMD_AVR8_MEMORY_WRITE = 0x23  # Write memory
    CMD_AVR8_CRC = 0x24  # Calculate CRC
    CMD_AVR8_RESET = 0x30  # Reset the MCU
    CMD_AVR8_STOP = 0x31  # Stop the MCU
    CMD_AVR8_RUN = 0x32  # Resume execution
    CMD_AVR8_RUN_TO_ADDRESS = 0x33  # Resume with breakpoint
    CMD_AVR8_STEP = 0x34  # Single step
    CMD_AVR8_PC_READ = 0x35  # Read PC
    CMD_AVR8_PC_WRITE = 0x36  # Write PC
    CMD_AVR8_HW_BREAK_SET = 0x40  # Set breakpoints
    CMD_AVR8_HW_BREAK_CLEAR = 0x41  # Clear breakpoints
    CMD_AVR8_SW_BREAK_SET = 0x43  # Set software breakpoints
    CMD_AVR8_SW_BREAK_CLEAR = 0x44  # Clear software breakpoints
    CMD_AVR8_SW_BREAK_CLEAR_ALL = 0x45  # Clear all software breakpoints
    CMD_AVR8_PAGE_ERASE = 0x50  # Erase page

    # Response IDs
    RSP_AVR8_OK = 0x80  # All OK
    RSP_AVR8_LIST = 0x81  # List of items returned
    RSP_AVR8_DATA = 0x84  # Data returned
    RSP_AVR8_PC = 0x83  # PC value returned
    RSP_AVR8_FAILED = 0xA0  # Command failed to execute

    # Failure codes
    AVR8_FAILURE_OK = 0x00
    AVR8_FAILURE_DW_PHY_ERROR = 0x10  # debugWIRE physical error
    AVR8_FAILURE_JTAGM_INIT_ERROR = 0x11  # JTAGM failed to initialise
    AVR8_FAILURE_JTAGM_ERROR = 0x12  # JTAGM did something strange
    AVR8_FAILURE_JTAG_ERROR = 0x13  # Error in JTAG low level (bit banger or JTAGM)
    AVR8_FAILURE_JTAGM_VERSION = 0x14  # Unsupported version of JTAG master hardware
    AVR8_FAILURE_JTAGM_TIMEOUT = 0x15  # JTAG master timed out
    AVR8_FAILURE_JTAG_BIT_BANGER_TIMEOUT = 0x16  # JTAG bit banger timed out
    AVR8_FAILURE_PARITY_ERROR = 0x17  # Parity error in received data
    AVR8_FAILURE_EB_ERROR = 0x18  # Did not receive EMPTY byte when writing (Xmega)
    AVR8_FAILURE_PDI_TIMEOUT = 0x19  # PDI physical timed out
    AVR8_FAILURE_COLLISION = 0x1A  # Collision on physical level
    AVR8_FAILURE_PDI_ENABLE = 0x1B  # PDI enable failed
    AVR8_FAILURE_FRAMING_ERROR = 0x1C  # Framing error
    AVR8_FAILURE_DMA_ERROR = 0x1D  # DMA error during transfer
    AVR8_FAILURE_NO_DEVICE_FOUND = 0x20  # devices == 0!
    AVR8_FAILURE_CLOCK_ERROR = 0x21  # Failure when increasing communication baud
    AVR8_FAILURE_NO_TARGET_POWER = 0x22  # Target power not detected
    AVR8_FAILURE_NOT_ATTACHED = 0x23  # Must run attach command first
    AVR8_FAILURE_DAISY_CHAIN_TOO_LONG = 0x24  # devices > 31
    AVR8_FAILURE_DAISY_CHAIN_CONFIG = 0x25  # configured dev bits do not add up to detected ones
    AVR8_FAILURE_INVALID_PHYSICAL_STATE = 0x31  # Physical is not in the assumed activation state
    AVR8_FAILURE_ILLEGAL_STATE = 0x32  # Illegal run / stopped state
    AVR8_FAILURE_INVALID_CONFIG = 0x33  # Invalid configuration for activate physical
    AVR8_FAILURE_INVALID_MEMTYPE = 0x34  # Not a valid memtype
    AVR8_FAILURE_INVALID_SIZE = 0x35  # Asked for too many or too few bytes
    AVR8_FAILURE_INVALID_ADDRESS = 0x36  # Asked for a bad address
    AVR8_FAILURE_INVALID_ALIGNMENT = 0x37  # Asked for an invalid alignment
    AVR8_FAILURE_ILLEGAL_MEMORY_RANGE = 0x38  # Address not within legal range
    AVR8_FAILURE_ILLEGAL_VALUE = 0x39  # Illegal value provided
    AVR8_FAILURE_ILLEGAL_ID = 0x3A  # Illegal target ID
    AVR8_FAILURE_INVALID_CLOCK_SPEED = 0x3B  # Clock speed specified is not valid
    AVR8_FAILURE_TIMEOUT = 0x3C  # The request timed out
    AVR8_FAILURE_ILLEGAL_OCD_STATUS = 0x3D  # Read an OCD status that is illegal (reserved bits set)
    AVR8_FAILURE_NVM_ENABLE = 0x40  # Failed to enable NVM
    AVR8_FAILURE_NVM_DISABLE = 0x41  # Error disabling NVM
    AVR8_FAILURE_CS_ERROR = 0x42  # Error accessing control/status space
    AVR8_FAILURE_CRC_FAILURE = 0x43  # CRC operation caused an error
    AVR8_FAILURE_OCD_LOCKED = 0x44  # The device is unable to enter debug mode
    AVR8_FAILURE_KEY_ERROR = 0x45  # Error while applying key
    AVR8_FAILURE_BOOT_ERROR = 0x46  # Error while booting the device
    AVR8_FAILURE_ACK_ERROR = 0x47  # ACK not received from the device
    AVR8_FAILURE_NO_OCD_CONTROL = 0x50  # Unable to gain control of the debug system
    AVR8_FAILURE_PLEASE_TOGGLE_POWER = 0x51  # Toggle power to allow high-voltage access
    AVR8_FAILURE_NO_VOUT_SET = 0x52  # Unable to toggle power when no voltage output is specified
    AVR8_FAILURE_VOUT_ERROR = 0x53  # Vout is not powering the target
    AVR8_FAILURE_PC_READ_FAILED = 0x60  # Error when reading PC
    AVR8_FAILURE_REGISTER_READ_FAILED = 0x61  # Error when reading register
    AVR8_FAILURE_READ_ERROR = 0x70  # Error reading from the target
    AVR8_FAILURE_WRITE_ERROR = 0x71  # Error writing to the target
    AVR8_FAILURE_WRITE_TIMEOUT = 0x72  # Timeout writing to the target
    AVR8_FAILURE_ILLEGAL_BREAKPOINT = 0x80  # An illegal breakpoint address/type was requested
    AVR8_FAILURE_TOO_MANY_BREAKPOINTS = 0x81  # Breakpoint memory is full
    AVR8_FAILURE_NOT_SUPPORTED = 0x90  # The command is not supported
    AVR8_FAILURE_NOT_IMPLEMENTED = 0x91  # The command has not been implemented
    AVR8_FAILURE_UNKNOWN = 0xFF  # Internal error

    # Event IDs
    EVT_AVR8_BREAK = 0x40  # Break event
    EVT_AVR8_IDR = 0x41  # Debugger data register value received

    # Context IDs
    AVR8_CTXT_CONFIG = 0x00  # Configuration
    AVR8_CTXT_PHYSICAL = 0x01  # Physical interface related
    AVR8_CTXT_DEVICE = 0x02  # Device specific settings
    AVR8_CTXT_OPTIONS = 0x03  # Option-related settings
    AVR8_CTXT_SESSION = 0x04  # Session-related settings

    # Configs
    AVR8_CONFIG_VARIANT = 0x00  # Device family/variant
    AVR8_CONFIG_FUNCTION = 0x01  # Functional intent

    # Physical settings
    AVR8_PHY_INTERFACE = 0x00  # Physical interface selector
    AVR8_PHY_JTAG_DAISY = 0x01  # JTAG daisy chain settings
    AVR8_PHY_DW_CLK_DIV = 0x10  # debugWIRE clock divide ratio
    AVR8_PHY_MEGA_PRG_CLK = 0x20  # Clock for programming megaAVR
    AVR8_PHY_MEGA_DBG_CLK = 0x21  # Clock for debugging megaAVR
    AVR8_PHY_XM_JTAG_CLK = 0x30  # JTAG clock for AVR XMEGA
    AVR8_PHY_XM_PDI_CLK = 0x31  # PDI clock for AVR XMEGA/UPDI devices

    # Options
    AVR8_OPT_RUN_TIMERS = 0x00  # Keep timers running when stopped
    AVR8_OPT_DISABLE_DBP = 0x01  # No data breaks during reset
    AVR8_OPT_ENABLE_IDR = 0x03  # Relay IDR messages
    AVR8_OPT_POLL_INT = 0x04  # Configure polling speed
    AVR8_OPT_POWER_NAP = 0x05  # Use Power Nap (deprecated)
    AVR8_OPT_HV_UPDI_ENABLE = 0x06  # Enable UPDI using high-voltage
    AVR8_OPT_12V_UPDI_ENABLE = 0x06  # Back-compatibility alias for the above option
    AVR8_OPT_CHIP_ERASE_TO_ENTER = 0x07  # Use CHIP ERASE KEY when next entering programming mode

    # Variants
    AVR8_VARIANT_LOOPBACK = 0x00  # Dummy device
    AVR8_VARIANT_TINYOCD = 0x01  # tinyAVR or megaAVR with debugWIRE
    AVR8_VARIANT_MEGAOCD = 0x02  # megaAVR with JTAG
    AVR8_VARIANT_XMEGA = 0x03  # AVR XMEGA
    AVR8_VARIANT_UPDI = 0x05  # New AVR devices with UPDI
    AVR8_VARIANT_TINYX = 0x05  # Back-compatibility alias
    AVR8_VARIANT_NONE = 0xFF  # No device

    # Functions
    AVR8_FUNC_NONE = 0x00  # Not configured
    AVR8_FUNC_PROGRAMMING = 0x01  # I want to program only
    AVR8_FUNC_DEBUGGING = 0x02  # I want a debug session

    # Physical interfaces
    AVR8_PHY_INTF_NONE = 0x00  # Not configured
    AVR8_PHY_INTF_JTAG = 0x04  # JTAG
    AVR8_PHY_INTF_DW = 0x05  # debugWIRE
    AVR8_PHY_INTF_PDI = 0x06  # PDI (2 wire)
    AVR8_PHY_INTF_PDI_1W = 0x08  # UPDI (1 wire)
    AVR8_PHY_INTF_PDI_3W = 0x0A  # PDI/UPDI 3W (3 wire)

    # UPDI high-voltage enable modes
    UPDI_HV_NONE = 0x00  # Do not use high-voltage
    UPDI_12V_NONE = 0x00  # Back-compatibility alias
    UPDI_HV_SIMPLE_PULSE = 0x01  # Issue a single high-voltage pulse immediately
    UPDI_HV_AUTO_POWER_TOGGLE = 0x02  # Toggle power automatically and then apply a high-voltage pulse
    UPDI_HV_USER_POWER_TOGGLE = 0x03  # The user toggles power, and the tool applies a high-voltage pulse on power-up

    # Memory types for read/write
    AVR8_MEMTYPE_SRAM = 0x20  # RAM
    AVR8_MEMTYPE_EEPROM = 0x22  # EEPROM
    AVR8_MEMTYPE_IO_SHADOW = 0x30  # Shadow IO (deprecated)
    AVR8_MEMTYPE_SPM = 0xA0  # Flash memory using SPM (in debug mode)
    AVR8_MEMTYPE_FLASH_PAGE = 0xB0  # Full-page flash access
    AVR8_MEMTYPE_EEPROM_PAGE = 0xB1  # Full-page EEPROM access
    AVR8_MEMTYPE_FUSES = 0xB2  # Fuses
    AVR8_MEMTYPE_LOCKBITS = 0xB3  # Lockbits
    AVR8_MEMTYPE_SIGNATURE = 0xB4  # Signatures
    AVR8_MEMTYPE_OSCCAL = 0xB5  # OSCCAL
    AVR8_MEMTYPE_CAN = 0xB6  # CAN mailbox (deprecated)
    AVR8_MEMTYPE_TBUS = 0xB7  # Bus access (deprecated)
    AVR8_MEMTYPE_REGFILE = 0xB8  # Register file access
    AVR8_MEMTYPE_APPL_FLASH = 0xC0  # Application flash space (XMEGA)
    AVR8_MEMTYPE_BOOT_FLASH = 0xC1  # Boot flash space (XMEGA)
    AVR8_MEMTYPE_APPL_FLASH_ATOMIC = 0xC2  # Auto-erased application flash
    AVR8_MEMTYPE_BOOT_FLASH_ATOMIC = 0xC3  # Auto-erased boot flash
    AVR8_MEMTYPE_EEPROM_ATOMIC = 0xC4  # Auto-erased EEPROM
    AVR8_MEMTYPE_USER_SIGNATURE = 0xC5  # User signature row access
    AVR8_MEMTYPE_CALIBRATION_SIGNATURE = 0xC6  # Calibration row access
    AVR8_MEMTYPE_CS = 0xD0  # PDI/UPDI Control/Status space
    AVR8_MEMTYPE_OCD = 0xD1  # On-chip debug memory
    AVR8_MEMTYPE_SIB = 0xD3  # UPDI System Information Block access

    # Erase modes
    ERASE_CHIP = 0x00  # Erase the entire chip
    ERASE_APP = 0x01  # Erase application section only
    ERASE_BOOT = 0x02  # Erase boot section only
    ERASE_EEPROM = 0x03  # Erase EEPROM only
    ERASE_APP_PAGE = 0x04  # Erase application page
    ERASE_BOOT_PAGE = 0x05  # Erase boot page
    ERASE_EEPROM_PAGE = 0x06  # Erase EEPROM page
    ERASE_USERSIG = 0x07  # Erase User Signature page

    def __init__(self, transport):
        self.logger = getLogger(__name__)
        super(Avr8Protocol, self).__init__(
            transport, Jtagice3Protocol.HANDLER_AVR8_GENERIC)

    def error_as_string(self, code):
        """
        Get the response error as a string (error code translated to descriptive string)

        :return: error code as descriptive string
        """
        return AVR8_ERRORS[code]

    # Configuration shortcuts for AVR8 target types
    def set_variant(self, variant):
        """
        Sets the variant field in the config context

        :param variant: type of device
        """
        self.set_byte(self.AVR8_CTXT_CONFIG, self.AVR8_CONFIG_VARIANT, variant)

    def set_function(self, debugger_function):
        """
        Sets the function field in the config context

        :param debugger_function: function of this session (prog / debug)
        :return:
        """
        self.set_byte(self.AVR8_CTXT_CONFIG, self.AVR8_CONFIG_FUNCTION, debugger_function)

    def set_interface(self, interface):
        """
        Sets the function field in the physical context

        :param interface: physical interface setting
        """
        self.set_byte(self.AVR8_CTXT_PHYSICAL, self.AVR8_PHY_INTERFACE, interface)

    def write_device_data(self, data):
        """
        Write device info into the device context

        :param data: device data content
        """
        self._set_protocol(self.AVR8_CTXT_DEVICE, 0x00, data)

    def configure_daisy_chain(self, settings):
        """
        Sets the daisy-chain fields in the physical context

        :param settings: array of daisy-chain info
        """
        self._set_protocol(self.AVR8_CTXT_PHYSICAL, self.AVR8_PHY_JTAG_DAISY,
                           [0x04, settings[0], settings[1], settings[2], settings[3]])

    # Physical / connection commands
    def activate_physical(self, use_reset=False):
        """
        Activates the physical interface

        :param use_reset: reset
        :return:
        """
        self.logger.debug("Activate physical")
        # try:
        device_id = self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR8_ACTIVATE_PHYSICAL, self.CMD_VERSION0, int(use_reset)])))

        # No ID returned is also ok (some interfaces are just not clever enough)
        if not device_id:
            self.logger.debug("ID=%02X%02X%02X%02X", device_id[3], device_id[2], device_id[1], device_id[0])
        return device_id

    def deactivate_physical(self):
        """Deactivates the physical interface"""
        self.logger.debug("Deactivate physical")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_DEACTIVATE_PHYSICAL,
                                                                      self.CMD_VERSION0])))

    # Programming mode entry / exit commands
    def enter_progmode(self):
        """Enters programming mode"""
        self.logger.debug("Enter prog mode")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_PROG_MODE_ENTER,
                                                                      self.CMD_VERSION0])))

    def leave_progmode(self):
        """Exits programming mode"""
        self.logger.debug("Leave prog mode")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_PROG_MODE_LEAVE,
                                                                      self.CMD_VERSION0])))

    def get_id(self):
        """
        Reads the device ID

        :return: device ID
        """
        self.logger.debug("%s::get ID", self.__class__.__name__)
        return self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_GET_ID, self.CMD_VERSION0])))

    # Debug mode entry / exit commands
    def attach(self, do_break=False):
        """
        Attaches the debugger to the target

        :param do_break: break execution on attach?
        """
        self.logger.debug("Attach")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_ATTACH, self.CMD_VERSION0,
                                                                      int(do_break)])))

    def detach(self):
        """Detaches the debugger from the target"""
        self.logger.debug("Detach")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_DETACH, self.CMD_VERSION0])))

    # General memory access commands

    def erase(self, mode=0, address=0):
        """
        Erase target flash

        :param mode: flash erase mode to use
        :param address: start address to erase from
        """
        return self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR8_ERASE, self.CMD_VERSION0, mode]) + binary.pack_le32(address)))

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
            bytearray([self.CMD_AVR8_MEMORY_READ, self.CMD_VERSION0, memtype]) +
            binary.pack_le32(address) + binary.pack_le32(num_bytes)))

    def memory_write(self, memtype, address, data):
        """
        Write memory to target

        :param memtype: memory type / region to access
        :param address: start address
        :param data: data to write
        """
        data = bytearray(data)
        return self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR8_MEMORY_WRITE, self.CMD_VERSION0, memtype]) + binary.pack_le32(
                address) + binary.pack_le32(len(data)) + bytearray([0x00]) + data))

    # Debugging flow-control functions

    def reset(self):
        """Resets the core and holds it in reset"""
        self.logger.debug("AVR core reset")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_RESET, self.CMD_VERSION0, 0x01])))

    def step(self):
        """
        Executes a single-step on the core.

        A BREAK event will be generated when it has completed.
        This behaviour originates from previous debuggers which could do C level stepping which took time.
        """
        self.logger.debug("AVR core step")
        self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR8_STEP, self.CMD_VERSION0, 0x01, 0x01])))

    def stop(self):
        """
        Requests a core halt.

        A BREAK even will be generated when it has successfully stopped.
        """
        self.logger.debug("AVR core halt request")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_STOP, self.CMD_VERSION0, 0x01])))

    def run(self):
        """Resumes core execution"""
        self.logger.debug("AVR core resume")
        self.check_response(self.jtagice3_command_response(bytearray([self.CMD_AVR8_RUN, self.CMD_VERSION0])))

    def run_to(self, address):
        """
        Runs to the given address.

        A BREAK event will be generated when/if it reaches the address.
        :param address:
        """
        self.logger.debug("AVR core run to address")
        self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR8_RUN_TO_ADDRESS, self.CMD_VERSION0]) + binary.pack_le32(address)))

    # Debug memory access functions

    def program_counter_read(self):
        """
        Reads out the program counter

        :return: PC value
        """
        program_counter = binary.unpack_le32(self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR8_PC_READ, self.CMD_VERSION0])), self.RSP_AVR8_PC))
        msg = "PC read as 0x{:08X}".format(program_counter)
        self.logger.debug(msg)
        return program_counter

    def program_counter_write(self, program_counter):
        """
        Writes the program counter

        :param program_counter:
        """
        self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR8_PC_WRITE, self.CMD_VERSION0]) + binary.pack_le32(program_counter)))

    def regfile_read(self):
        """
        Reads out the AVR register file (R0::R31)

        :return:
        """
        self.logger.debug("Reading register file")
        data = self.memory_read(self.AVR8_MEMTYPE_REGFILE, 0, 32)
        self.logger.debug(data)
        return data

    def regfile_write(self, data):
        """
        Writes the AVR registe file (R0::R31)

        :param data: register array
        :return:
        """
        if len(data) != 32:
            raise ValueError("Invalid data length for regfile")
        self.logger.debug("Writing register file")
        return self.memory_write(self.AVR8_MEMTYPE_REGFILE, 0, data)

    # Software breakpoint functions

    def software_breakpoint_set(self, address):
        """
        Insert a software breakpoint at the given address

        :param address: breakpoint address
        """
        self.logger.debug("Set SWBP")
        self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR8_SW_BREAK_SET, self.CMD_VERSION0]) + binary.pack_le32(address)))

    def software_breakpoint_clear(self, address):
        """
        Removes a software breakpoint from the given address

        :param address: breakpoint address
        """
        self.logger.debug("Clear SWBP")
        self.check_response(self.jtagice3_command_response(
            bytearray([self.CMD_AVR8_SW_BREAK_CLEAR, self.CMD_VERSION0]) + binary.pack_le32(address)))

    def software_breakpoint_clear_all(self):
        """Clears all software breakpoints"""
        self.logger.debug("SWBP clear all")
        self.check_response(
            self.jtagice3_command_response(bytearray([self.CMD_AVR8_SW_BREAK_CLEAR_ALL, self.CMD_VERSION0])))
