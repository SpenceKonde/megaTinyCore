#pylint: disable=too-few-public-methods
"""
Definitions of keys for device info dictionaries
"""

class DeviceInfoKeys(object):
    """
    Base class with common device info keys
    """

    NAME = 'name'
    ARCHITECTURE = 'architecture'
    INTERFACE = 'interface'
    DEVICE_ID = 'device_id'

    @classmethod
    def get_all(cls):
        """
        Get a list of all keys

        :return List of all valid keys (baseclass and any subclass keys if run on a subclass)
        """
        all_keys = []
        for attribute in dir(cls):
            if not attribute.startswith('__') and not callable(getattr(cls, attribute)):
                all_keys.append(getattr(cls, attribute))

        return all_keys

class DeviceInfoKeysAvr(DeviceInfoKeys):
    """
    Keys specific to AVR device info files
    """

    NVMCTRL_BASE = 'nvmctrl_base'
    SYSCFG_BASE = 'syscfg_base'
    OCD_BASE = 'ocd_base'
    PROG_CLOCK_KHZ = 'prog_clock_khz'
    ADDRESS_SIZE = 'address_size'

class DeviceInfoKeysAvr32(DeviceInfoKeys):
    """
    Keys specific to 32-bit AVR device info files
    """

    RESET_DOMAINS = 'reset_domains'

class DeviceInfoKeysPic(DeviceInfoKeys):
    """
    Keys specific to PIC device info files
    """

    # This key should have _byte or _word ending in device info files to specify byte or word address
    # This ending will be removed by the getdeviceinfo function before returning the device info dictionary
    DEFAULT_BULK_ERASE_ADDRESS = 'default_bulk_erase_address'

class DeviceMemoryInfoKeys(object):
    """
    Keys for device memory info dictionary

    These keys are found in the dictionaries returned by DeviceMemoryInfo for each memory type
    """
    NAME = 'name'
    ADDRESS = 'address'
    SIZE = 'size'
    PAGE_SIZE = 'page_size'
    WRITE_SIZE = 'write_size'
    READ_SIZE = 'read_size'
    ERASE_ADDRESS = 'erase_address'
    CHIPERASE_EFFECT = 'chiperase_effect'
    ISOLATED_ERASE = 'isolated_erase'
    HEXFILE_ADDRESS = 'hexfile_address'
    HEXFILE_SIZE = 'hexfile_size'
    VERIFY_MASK = 'verify_mask'

    @classmethod
    def get_all(cls):
        """
        Get a list of all keys

        :return List of all valid keys (baseclass and any subclass keys if run on a subclass)
        """
        all_keys = []
        for attribute in dir(cls):
            if not attribute.startswith('__') and not callable(getattr(cls, attribute)):
                all_keys.append(getattr(cls, attribute))

        return all_keys
