#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
#pylint: disable=missing-docstring

import unittest
from mock import MagicMock
from mock import patch

from pymcuprog.nvmmzeroplus import NvmAccessProviderCmsisDapMZeroPlus

class TestNvmAccessProviderCmsisDapMZeroPlus(unittest.TestCase):

    def setUp(self):
        self.mock_samd2xtarget_patch = patch('pymcuprog.nvmmzeroplus.SamD2xTarget')
        self.addCleanup(self.mock_samd2xtarget_patch.stop)
        self.mock_samd2xtarget = self.mock_samd2xtarget_patch.start()
        self.mock_samd2xtarget_instance = MagicMock()
        self.mock_samd2xtarget.return_value = self.mock_samd2xtarget_instance

    def test_read_device_id_returns_bytearray_with_raw_id(self):
        id_expected = bytearray([0x01, 0x02, 0x03, 0x04])
        self.mock_samd2xtarget_instance.read_device_id.return_value = int.from_bytes(id_expected,
                                                                                     byteorder='little',
                                                                                     signed=False)
        self.mock_samd2xtarget_instance.read_idcode.return_value = 0

        mzeroplus = NvmAccessProviderCmsisDapMZeroPlus(None, None)

        id_read = mzeroplus.read_device_id()

        self.assertEqual(id_expected, id_read)

    def test_erase_when_memory_info_is_none_performs_chiperase(self):
        mzeroplus = NvmAccessProviderCmsisDapMZeroPlus(None, None)

        mzeroplus.erase(memory_info=None, address=None)

        self.mock_samd2xtarget_instance.chip_erase_dsu.assert_called()
