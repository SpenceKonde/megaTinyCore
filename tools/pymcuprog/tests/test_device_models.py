# The intention is to make the test names descriptive enough to not need any docstrings for most of them
#pylint: disable=missing-docstring
"""
Content tests for device models found in deviceinfo.devices
"""
import unittest

from parameterized import parameterized_class

from pymcuprog.deviceinfo.deviceinfo import get_supported_devices, getdeviceinfo
from pymcuprog.deviceinfo.deviceinfo import DeviceMemoryInfo
from pymcuprog.deviceinfo.memorynames import MemoryNames
from pymcuprog.deviceinfo.eraseflags import get_list_of_chiperase_effects
from pymcuprog.deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys, DeviceInfoKeysPic
from pymcuprog.deviceinfo.deviceinfokeys import DeviceInfoKeys, DeviceInfoKeysAvr, DeviceInfoKeysAvr32

DEVICELIST = get_supported_devices()

DEVICETUPLELIST = []
for item in DEVICELIST:
    DEVICETUPLELIST.append((item,))

# Run all tests for each device
@parameterized_class(('devicename'), DEVICETUPLELIST)
class TestDeviceModels(unittest.TestCase):
    """
    This test class contains tests that are repeated for each device file
    """

    def setUp(self):
        # pylint does not understand the trick done by parameterized which injects the devicename at runtime
        #pylint: disable=no-member
        self.deviceinfo = getdeviceinfo(self.devicename)

    @staticmethod
    def _is_8bit_pic(devicename):
        return 'pic' in devicename.lower() and 'pic24' not in devicename.lower()

    def _param_exists(self, param, memory_info):
        memname = memory_info[DeviceMemoryInfoKeys.NAME]
        self.assertTrue(param in memory_info,
                        msg="{} memory is missing {} parameter".format(memname, param))

    def _check_param_type(self, param, paramtype, memory_info):
        paramvalue = memory_info[param]
        memname = memory_info[DeviceMemoryInfoKeys.NAME]
        self.assertIsInstance(paramvalue, paramtype,
                              msg="Invalid {} parameter of {} memory".format(param, memname))

    def _check_param_list_of_positive_ints(self, param, memory_info):
        self._check_param_type(param, list, memory_info)
        paramvalue = memory_info[param]
        memname = memory_info[DeviceMemoryInfoKeys.NAME]
        for subparam in paramvalue:
            message = "Invalid item in {} parameter list of {} memory".format(param, memname)
            self.assertIsInstance(subparam, int, msg=message)
            message = "Item in {} parameter list of {} memory should be a positive value".format(subparam, memname)
            self.assertGreaterEqual(subparam, 0, msg=message)

    def _check_param_is_positive(self, param, memory_info):
        paramvalue = memory_info[param]
        memname = memory_info[DeviceMemoryInfoKeys.NAME]
        self.assertGreaterEqual(paramvalue, 0,
                                msg="{} parameter of {} memory should be a positive value".format(param, memname))

    def _check_param_is_greater_than_zero(self, param, memory_info):
        paramvalue = memory_info[param]
        memname = memory_info[DeviceMemoryInfoKeys.NAME]
        message = "{} parameter of {} memory should be a positive value greater than 0".format(param, memname)
        self.assertGreater(paramvalue, 0, msg=message)

    def _check_chiperase_effect_param(self, memory_info):
        chiperase_effect_key = DeviceMemoryInfoKeys.CHIPERASE_EFFECT
        chiperase_effect_valid_values = get_list_of_chiperase_effects()
        self._param_exists(chiperase_effect_key, memory_info)
        chiperase_effect_value = memory_info[chiperase_effect_key]
        memname = memory_info[DeviceMemoryInfoKeys.NAME]
        message = "Invalid {} value ({}) for {} memory".format(chiperase_effect_key, chiperase_effect_value, memname)
        self.assertTrue(chiperase_effect_value in chiperase_effect_valid_values, msg=message)

    def _check_isolated_erase_param(self, memory_info):
        isolated_erase_key = DeviceMemoryInfoKeys.ISOLATED_ERASE
        self._param_exists(isolated_erase_key, memory_info)
        self._check_param_type(isolated_erase_key, bool, memory_info)

    @staticmethod
    def _is_valid_memory_type(param):
        valid_memory_types = MemoryNames.get_all()
        valid_memory_param = False
        for memtype in valid_memory_types:
            if param.startswith(memtype):
                valid_memory_param = True
                break
        return valid_memory_param

    @staticmethod
    def _strip_size_qualifier(param):
        if param.endswith('_byte') or param.endswith('_bytes'):
            param = param.split('_byte')[0]
        elif param.endswith('_word') or param.endswith('_words'):
            param = param.split('_word')[0]

        return param

    def test_device_info_parameters(self):
        """Check each parameter in the device info dict for the current device"""
        devicename = self.deviceinfo[DeviceInfoKeys.NAME].lower()
        valid_device_keys_pic = DeviceInfoKeysPic.get_all()
        valid_device_keys_avr32 = DeviceInfoKeysAvr32.get_all()
        valid_device_keys_avr = DeviceInfoKeysAvr.get_all()
        valid_device_keys_common = DeviceInfoKeys.get_all()
        for key in self.deviceinfo:
            # The key is a memory parameter for a valid memory type, continue with next.
            # Note that the test_device_memories will check that all required memory parameters are in place for
            # each memory, so it is sufficient to check that the parameter belongs to a valid memory.
            if self._is_valid_memory_type(key):
                continue

            # Remove any word/byte qualifiers before checking for valid non-memory related parameters as some
            # parameters (like default_bulk_erase_address) will have such a qualifier that will be removed while
            # parsing the device info dict
            key = self._strip_size_qualifier(key)

            # If not check if it is a valid device info key
            if 'pic' in devicename:
                if key in valid_device_keys_pic:
                    # This is a PIC device and the key is a valid PIC device file key, continue with next
                    continue
            elif 'uc3' in devicename:
                if key in valid_device_keys_avr32:
                    # This is a UC3 device (32-bit AVR) and the key is a valid AVR32 device file key, continue with next
                    continue
            elif 'sam' in devicename:
                # No extra keys defined for SAM devices so just check the common keys
                if key in valid_device_keys_common:
                    # This is a SAM device and the key is a valid device file key, continue with next
                    continue
            else:
                # Everything else is assumed to be AVR devices since these have the least consistent names
                if key in valid_device_keys_avr:
                    # This is an AVR device and the key is a valid AVR device file key, continue with next
                    continue

            self.fail(msg="{} is not a valid device info parameter for {}".format(key, devicename))


    def test_default_bulk_erase_address(self):
        # The Q43 family bulk erase command takes a bit pattern as parameter so no need for a bulk erase address
        # pylint does not understand the trick done by parameterized which injects the devicename at runtime
        #pylint: disable=no-member
        if self._is_8bit_pic(self.devicename) and not any(substring in self.devicename.lower() for substring in ["q8", "q4"]):
            # pylint does not understand the trick done by parameterized which injects the devicename at runtime
            #pylint: disable=no-member
            message = "No {} key found for {}".format(DeviceInfoKeysPic.DEFAULT_BULK_ERASE_ADDRESS, self.devicename)
            self.assertTrue(DeviceInfoKeysPic.DEFAULT_BULK_ERASE_ADDRESS in self.deviceinfo, msg=message)
        else:
            message = "{} key makes no sense for {}".format(DeviceInfoKeysPic.DEFAULT_BULK_ERASE_ADDRESS,
                                                            self.devicename)
            self.assertFalse(DeviceInfoKeysPic.DEFAULT_BULK_ERASE_ADDRESS in self.deviceinfo, msg=message)

    def test_device_memories(self):
        required_int_params = [DeviceMemoryInfoKeys.ADDRESS,
                               DeviceMemoryInfoKeys.SIZE,
                               DeviceMemoryInfoKeys.PAGE_SIZE,
                               DeviceMemoryInfoKeys.WRITE_SIZE,
                               DeviceMemoryInfoKeys.READ_SIZE,
                               DeviceMemoryInfoKeys.ERASE_ADDRESS,
                               DeviceMemoryInfoKeys.HEXFILE_ADDRESS,
                               DeviceMemoryInfoKeys.HEXFILE_SIZE]

        required_list_of_int_params = [DeviceMemoryInfoKeys.VERIFY_MASK]

        device_memory_info = DeviceMemoryInfo(self.deviceinfo)
        all_memories = MemoryNames.get_all()
        for memname in all_memories:
            try:
                meminfo = device_memory_info.memory_info_by_name(memname)
            except ValueError:
                # This memory is not defined for the current device, just move on
                continue
            else:
                for param in required_int_params:
                    self._param_exists(param, meminfo)
                    self._check_param_type(param, int, meminfo)
                    if DeviceMemoryInfoKeys.PAGE_SIZE in param:
                        # Page size should be more than 0 to avoid errors like divide by 0,
                        # chunking into chunks with size 0 etc.
                        self._check_param_is_greater_than_zero(param, meminfo)
                    else:
                        # Other parameters should also be positive, but can be 0
                        self._check_param_is_positive(param, meminfo)

                for param in required_list_of_int_params:
                    self._param_exists(param, meminfo)
                    self._check_param_list_of_positive_ints(param, meminfo)

                self._check_chiperase_effect_param(meminfo)

                self._check_isolated_erase_param(meminfo)
