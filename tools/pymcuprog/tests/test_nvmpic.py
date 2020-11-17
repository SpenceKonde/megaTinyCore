# The intention is to make the test names descriptive enough to not need any docstrings for most of them
#pylint: disable=missing-docstring
# It seems better to have all tests for one module in the same file than to split across multiple files,
# so accepting many public methods and many lines makes sense
#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
import sys
import unittest
from mock import MagicMock

from pymcuprog.nvmpic import NvmAccessProviderCmsisDapPic
from pymcuprog.deviceinfo.deviceinfokeys import DeviceInfoKeys

class TestNvmAccessProviderCmsisDapPic(unittest.TestCase):

    @staticmethod
    def _mock_pic_model():
        """
        Create a mock for the device support scripts debugger model

        :returns: Mock object of the device model instance
        """
        # Mock out the debugprovider
        sys.modules['common.debugprovider'] = MagicMock()

        # Create the mock for provide_debugger_model,
        # this will be the same mock that will be imported by the module under test.
        from common.debugprovider import provide_debugger_model # pylint: disable=import-error, import-outside-toplevel

        mock_pic_model = provide_debugger_model.return_value

        return mock_pic_model


    def test_read_device_id_returns_bytearray_with_raw_id(self):
        id_expected = bytearray([0x01, 0x02])

        mock_pic_model = self._mock_pic_model()
        mock_pic_model.read_id.return_value = id_expected[0] + (id_expected[1] << 8)

        pic = NvmAccessProviderCmsisDapPic(None, {DeviceInfoKeys.NAME: 'pic'}, '')

        id_read = pic.read_device_id()

        self.assertEqual(id_expected, id_read)

    def test_hold_in_reset_propagates(self):
        mock_pic_model = self._mock_pic_model()

        pic = NvmAccessProviderCmsisDapPic(None, {DeviceInfoKeys.NAME: 'pic'}, '')
        pic.hold_in_reset()

        mock_pic_model.hold_in_reset.assert_called()

    def test_release_from_reset_propagates(self):
        mock_pic_model = self._mock_pic_model()

        pic = NvmAccessProviderCmsisDapPic(None, {DeviceInfoKeys.NAME: 'pic'}, '')
        pic.release_from_reset()

        mock_pic_model.release_from_reset.assert_called()
