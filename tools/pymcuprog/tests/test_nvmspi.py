#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
#pylint: disable=missing-docstring

import unittest
from mock import MagicMock
from mock import patch

from pymcuprog.nvmspi import NvmAccessProviderCmsisDapSpi

class TestNvmAccessProviderCmsisDapSpi(unittest.TestCase):
    def _mock_avrispprotocol(self):
        """
        Create a mock of the AvrIspProtocol in pymcuprog.nvmspi

        :returns: Mock of AvrIspProtocol instance
        """
        mock_avrispprotocol_patch = patch("pymcuprog.nvmspi.AvrIspProtocol")
        self.addCleanup(mock_avrispprotocol_patch.stop)
        mock_avrispprotocol = mock_avrispprotocol_patch.start()
        mock_avrispprotocol_instance = MagicMock()
        mock_avrispprotocol.return_value = mock_avrispprotocol_instance

        return mock_avrispprotocol_instance

    def test_read_device_id_returns_bytearray_with_raw_id(self):
        id_expected = bytearray([0x01, 0x02, 0x03])
        mock_avrispprotocol_instance = self._mock_avrispprotocol()
        mock_avrispprotocol_instance.get_id.return_value = bytearray([id_expected[2], id_expected[1], id_expected[0]])

        isp = NvmAccessProviderCmsisDapSpi(None, None)

        id_read = isp.read_device_id()

        self.assertEqual(id_expected, id_read)

    def test_hold_in_reset_does_nothing(self):
        mock_avrispprotocol_instance = self._mock_avrispprotocol()

        isp = NvmAccessProviderCmsisDapSpi(None, None)
        isp.hold_in_reset()

        mock_avrispprotocol_instance.hold_in_reset.assert_not_called()

    def test_release_from_reset_leaves_progmode(self):
        mock_avrispprotocol_instance = self._mock_avrispprotocol()

        isp = NvmAccessProviderCmsisDapSpi(None, None)
        isp.release_from_reset()

        mock_avrispprotocol_instance.leave_progmode.assert_called()
