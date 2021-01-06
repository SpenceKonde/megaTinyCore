#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
#pylint: disable=missing-docstring

import unittest
from mock import MagicMock
from mock import patch

from pymcuprog.nvmdebugwire import NvmAccessProviderCmsisDapDebugwire

class TestNvmAccessProviderCmsisDapDebugwire(unittest.TestCase):
    @patch("pymcuprog.nvmdebugwire.TinyAvrTarget")
    def test_read_device_id_returns_bytearray_with_raw_id(self, mock_tinyavrtarget):
        id_expected = bytearray([0x01, 0x02])
        mock_tinyavrtarget_instance = MagicMock()
        mock_tinyavrtarget.return_value = mock_tinyavrtarget_instance
        mock_tinyavrtarget_instance.activate_physical.return_value = id_expected

        dwtarget = NvmAccessProviderCmsisDapDebugwire(None, None)

        id_read = dwtarget.read_device_id()

        self.assertEqual(id_expected, id_read)
