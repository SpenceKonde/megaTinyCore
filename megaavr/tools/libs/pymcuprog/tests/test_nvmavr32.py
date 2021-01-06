#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
#pylint: disable=missing-docstring

import unittest
from mock import MagicMock
from mock import patch

from pymcuprog.nvmavr32 import NvmAccessProviderCmsisDapAvr32
from pymcuprog.deviceinfo.deviceinfokeys import DeviceInfoKeysAvr32

class TestNvmAccessProviderCmsisDapAvr32(unittest.TestCase):
    def _mock_avr32_device(self):
        mock_avr32device_patch = patch("pymcuprog.nvmavr32.Avr32Device")
        self.addCleanup(mock_avr32device_patch.stop)
        mock_avr32device = mock_avr32device_patch.start()
        mock_avr32device_instance = MagicMock()
        mock_avr32device.return_value = mock_avr32device_instance

        return mock_avr32device_instance

    def test_read_device_id_returns_bytearray_with_raw_id(self):
        id_expected = bytearray([0x01, 0x02, 0x03, 0x04])
        mock_avr32device_instance = self._mock_avr32_device()
        mock_avr32device_instance.activate_physical.return_value = id_expected

        avr32 = NvmAccessProviderCmsisDapAvr32(None, {DeviceInfoKeysAvr32.RESET_DOMAINS: 5})

        id_read = avr32.read_device_id()

        self.assertEqual(id_expected, id_read)
