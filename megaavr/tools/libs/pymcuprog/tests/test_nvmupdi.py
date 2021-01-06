#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
#pylint: disable=missing-docstring

import unittest
from array import array
from mock import MagicMock
from mock import patch

from pymcuprog.nvmupdi import NvmAccessProviderCmsisDapUpdi
from pymcuprog.deviceinfo import deviceinfo

class TestNvmAccessProviderCmsisDapAvr32(unittest.TestCase):
    def _mock_tinyxavrtarget(self):
        """
        Create a mock for the TinyXAvrTarget instance in pymcuprog.nvmupdi

        :returns: Mock of TinyXAvrTarget instance
        """
        mock_tinyxavrtarget_patch = patch("pymcuprog.nvmupdi.TinyXAvrTarget")
        self.addCleanup(mock_tinyxavrtarget_patch.stop)
        mock_tinyxavrtarget = mock_tinyxavrtarget_patch.start()
        mock_tinyxavrtarget_instance = MagicMock()
        mock_tinyxavrtarget.return_value = mock_tinyxavrtarget_instance

        return mock_tinyxavrtarget_instance


    def test_read_device_id_returns_bytearray_with_raw_id(self):
        atmega4809_sib = array('B', map(ord, "megaAVR P:0D:1-3M2 (01.59B20.0)"))

        id_expected = bytearray([0x01, 0x02, 0x03])
        mock_tinyxavrtarget_instance = self._mock_tinyxavrtarget()
        mock_tinyxavrtarget_instance.memory_read.return_value = bytearray([id_expected[2],
                                                                           id_expected[1],
                                                                           id_expected[0]])
        mock_tinyxavrtarget_instance.sib_read.return_value = atmega4809_sib

        dinfo = deviceinfo.getdeviceinfo('atmega4809')

        updi = NvmAccessProviderCmsisDapUpdi(None, dinfo)

        id_read = updi.read_device_id()

        self.assertEqual(id_expected, id_read)

    def test_hold_in_reset_does_nothing(self):
        mock_tinyxavrtarget = self._mock_tinyxavrtarget()

        dinfo = deviceinfo.getdeviceinfo('atmega4809')
        updi = NvmAccessProviderCmsisDapUpdi(None, dinfo)
        updi.hold_in_reset()

        mock_tinyxavrtarget.hold_in_reset.assert_not_called()

    def test_release_from_reset_leaves_progmode(self):
        mock_tinyxavrtarget = self._mock_tinyxavrtarget()

        dinfo = deviceinfo.getdeviceinfo('atmega4809')
        updi = NvmAccessProviderCmsisDapUpdi(None, dinfo)
        updi.release_from_reset()

        mock_tinyxavrtarget.leave_progmode.assert_called()
