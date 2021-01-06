"""
Kit configuration programming protocol layer
This protocol is used in manufacturing to provision Curiosity Nano kits.
It is not intended for end-user usage - for kit customisation use the pydebuggerconfig package.
"""
from .dapwrapper import DapWrapper
from ..util.chopper import DataChopper
from ..util.binary import pack_le16

# Kit config map
FACTORY_CONFIG_BASE_TAG = 0xE00
USER_CONFIG_BASE_TAG = 0xC00
DEVICE_DATA_BASE_TAG = 0x100

CONFIG_BLOCKS = 8
CONFIG_BLOCK_SIZE = 512
DEVICE_DATA_BLOCK_SIZE = 11 * 256
CONFIG_CHUNK_SIZE = 32

EDBG_CONFIG_VERSION3_RAW_ACCESS = 0x40


def create_blank_config_block():
    """
    Create a blank config block

    :return: Blank config block
    """
    config = []
    for _ in range(CONFIG_BLOCK_SIZE):
        config.append(0x00)
    return config


def create_blank_device_data_block():
    """
    Create a blank device data block

    :return: Blank device data block
    """
    config = []
    for _ in range(DEVICE_DATA_BLOCK_SIZE):
        config.append(0x00)
    return config

class ConfigProtocolResponseError(Exception):
    """Exception type for Config protocol responses"""

    def __init__(self, msg, code):
        super(ConfigProtocolResponseError, self).__init__(msg)
        self.code = code

class ConfigProtocol(DapWrapper):
    """Transport layer for programming and reading device config"""

    # Protocol constants
    RSP_OK = 0x00
    GET_CONFIG = 0x83
    SET_CONFIG = 0x84

    def __init__(self, transport):
        self.transport_initialised = False
        if transport:
            DapWrapper.__init__(self, transport)
            self.transport_initialised = True
        self.chopper = DataChopper(CONFIG_CHUNK_SIZE)

    def set_transport(self, transport):
        """
        Set the transport for the protocol if not set at init

        :param transport: Transport to use
        """
        DapWrapper.__init__(self, transport)
        self.transport_initialised = True

    def tool_check(self):
        """
        Check if transport has been initiated """
        if not self.transport_initialised:
            raise IOError("No transport provided to ConfigProtocol")

    def fix(self, fix_data):
        """
        Checks if the incoming data is OK to send

        :param fix_data: Input data
        :return: list of valid data to send
        """
        return self.chopper.fix_data_type(fix_data)

    def set_config(self, offset, data):
        """
        Block programming of configuration data

        :param offset: Offset to start programming
        :param data: Configuration data to program
        """
        self.tool_check()

        # Vendor command, length, tag, data
        cmd = bytearray([self.SET_CONFIG])

        # Convert the data to a list
        payload = self.fix(data)
        size = len(payload)

        # Check length
        if size != CONFIG_CHUNK_SIZE:
            raise ValueError("Invalid length (!={})".format(CONFIG_CHUNK_SIZE))

        # Check offset
        if offset > (CONFIG_BLOCKS * CONFIG_BLOCK_SIZE) or offset < 0:
            raise ValueError("Invalid offset")

        # Add the chunk size
        cmd.append(size)

        # Add tag of 'EDBG_CONFIG_VERSION3_RAW_ACCESS'
        cmd.append(EDBG_CONFIG_VERSION3_RAW_ACCESS)

        # Add the offset
        cmd.extend(pack_le16(offset))

        # Add the payload
        cmd.extend(payload)

        # Vendor command, status
        response = self.dap_command_response(cmd)
        if response[1] is not self.RSP_OK:
            message = "Programming of config returned error code: 0x{:02X}".format(response[1])
            self.logger.error(message)
            raise ConfigProtocolResponseError(message, response[1])

    def get_config(self, offset, byte_count):
        """
        Low level function to read configuration data from target.

        :param offset: Offset to read from
        :param byte_count: Number of bytes to read
        :return: Response byte + data
        """
        self.tool_check()

        # Vendor command, element cnt, Config Tag, Dummy Byte
        cmd = bytearray([self.GET_CONFIG, 1, EDBG_CONFIG_VERSION3_RAW_ACCESS, 0x3F])

        # Add the offset
        cmd.extend(pack_le16(offset))

        # Add the chunk size
        cmd.extend(pack_le16(byte_count))

        if byte_count > 60:
            raise OverflowError("Byte count is too long")

        # Vendor command, status, len(msb), len(lsb), tag, type, data[...]
        response = self.dap_command_response(cmd)
        if response[1] is not self.RSP_OK:
            message = "Reading of config data returned error code: 0x{:02X}".format(response[1])
            self.logger.error(message)
            raise ConfigProtocolResponseError(message, response[1])

        return response[6:6 + byte_count]

    def read_config_block(self, factory=False):
        """
        Read the configuration block

        :param factory: Default false, use true for factory programming
        :return: Configuration from the target
        """
        if factory:
            offset = FACTORY_CONFIG_BASE_TAG
        else:
            offset = USER_CONFIG_BASE_TAG

        config = []
        for i in range(CONFIG_BLOCK_SIZE // CONFIG_CHUNK_SIZE):
            config += self.get_config(offset + (i * CONFIG_CHUNK_SIZE), CONFIG_CHUNK_SIZE)
        return config

    def write_config_block(self, config, factory=False):
        """
        Read the configuration block

        :param config: Configuration data to be written to target
        :param factory: Use true to program the factory section
        """
        if factory:
            offset = FACTORY_CONFIG_BASE_TAG
        else:
            offset = USER_CONFIG_BASE_TAG

        for i in range(CONFIG_BLOCK_SIZE // CONFIG_CHUNK_SIZE):
            self.set_config(offset + (i * CONFIG_CHUNK_SIZE), config[:CONFIG_CHUNK_SIZE])
            config = config[CONFIG_CHUNK_SIZE:]

    def read_device_data_block(self):
        """
        Read the device data block

        :return: Device data block
        """
        device_data = []
        for i in range(DEVICE_DATA_BLOCK_SIZE // CONFIG_CHUNK_SIZE):
            device_data += self.get_config(DEVICE_DATA_BASE_TAG + (i * CONFIG_CHUNK_SIZE), CONFIG_CHUNK_SIZE)
        return device_data

    def write_device_data_block(self, device_data):
        """
        Write the device data block

        :param device_data: Device data to write
        """
        for i in range(DEVICE_DATA_BLOCK_SIZE // CONFIG_CHUNK_SIZE):
            self.set_config(DEVICE_DATA_BASE_TAG + (i * CONFIG_CHUNK_SIZE), device_data[:CONFIG_CHUNK_SIZE])
            device_data = device_data[CONFIG_CHUNK_SIZE:]
