# The intention is to make the test names descriptive enough to not need any docstrings for most of them
#pylint: disable=missing-docstring
# It seems better to have all tests for one module in the same file than to split across multiple files,
# so accepting many public methods and many lines makes sense
#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
# Some tests require a lot of configurable parameters so there will be many local variables
#pylint: disable=too-many-locals
"""
pymcuprog backend API tests

These tests validates the API used by external front-ends/scripts
"""
import os
import sys
import shutil
import logging
import unittest
import copy
from distutils.version import LooseVersion
from collections import namedtuple
from mock import patch
from mock import MagicMock
from mock import call
from intelhex import IntelHex

from pyedbglib.hidtransport.hidtransportbase  import HidTool
from pyedbglib.protocols.jtagice3protocol import Jtagice3ResponseError, Jtagice3Protocol
from pyedbglib.protocols.housekeepingprotocol import Jtagice3HousekeepingProtocol

from pymcuprog.backend import Backend, SessionConfig
from pymcuprog.toolconnection import ToolUsbHidConnection, ToolSerialConnection, ToolConnection
from pymcuprog.pymcuprog_errors import PymcuprogToolConfigurationError, PymcuprogToolConnectionError
from pymcuprog.pymcuprog_errors import PymcuprogNotSupportedError, PymcuprogEraseError
from pymcuprog.pymcuprog_errors import PymcuprogSessionConfigError, PymcuprogSessionError
from pymcuprog.pymcuprog_errors import PymcuprogDeviceLockedError
from pymcuprog.hexfileutils import _write_hex_to_file
from pymcuprog.deviceinfo import deviceinfo
from pymcuprog.deviceinfo.memorynames import MemoryNames
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect
from pymcuprog.deviceinfo.deviceinfokeys import DeviceInfoKeys, DeviceMemoryInfoKeys

TESTFILE_FOLDER = "pymcuprog//tests//temphexfiles//"

class TestBackend(unittest.TestCase):
    """
    pymcuprog backend API unit tests
    """
    def setUp(self):
        # Make sure no relics from previous tests mess up the current test
        try:
            shutil.rmtree(TESTFILE_FOLDER)
        except FileNotFoundError:
            # Folder is already gone
            pass

        # Mock out the hid module.  This is a hack to avoid problems running tests in a docker on Linux where installing
        # hidapi fails due to missing libusb.h
        sys.modules['hid'] = MagicMock()
        self.backend = Backend()

    def mock_hid_transport(self, connect_return_value=True):
        """
        Mock hid_transport

        :param connect_return_value: Return value from hid_transport.connect()
        :returns: hid_transport instance mock object
        """
        mock_hid_transport_patch = patch("pymcuprog.backend.hid_transport")
        self.addCleanup(mock_hid_transport_patch.stop)
        mock_hid_transport = mock_hid_transport_patch.start()
        mock_hid_transport_instance = MagicMock()
        mock_hid_transport.return_value = mock_hid_transport_instance
        mock_hid_transport_instance.connect.return_value = connect_return_value

        return mock_hid_transport_instance

    def mock_is_connected_to_hid_tool(self, connected_to_hid=True):
        """
        Mock out the _is_connected_to_hid_tool() in the API as it won't work when the hid_transport has been mocked out

        :param connected_to_hid: Set to True if _is_connected_to_hid_tool() should return True
        """
        mock_is_connected_to_hid_tool_patch = patch("pymcuprog.backend.Backend._is_connected_to_hid_tool")
        self.addCleanup(mock_is_connected_to_hid_tool_patch.stop)
        mock_is_connected_to_hid_tool = mock_is_connected_to_hid_tool_patch.start()
        mock_is_connected_to_hid_tool.return_value = connected_to_hid

    def mock_housekeepingprotocol(self, device=''):
        """
        Mock Housekeepingprotocol

        :param device: device name to be returned in dap_info (part of tool_info)
        :returns: housekeepingprotocol instance mock object
        """
        mock_housekeepingprotocol_patch = patch("pymcuprog.backend.housekeepingprotocol")
        self.addCleanup(mock_housekeepingprotocol_patch.stop)
        mock_housekeepingprotocol = mock_housekeepingprotocol_patch.start()
        mock_housekeepingprotocol_instance = MagicMock()
        mock_housekeepingprotocol.Jtagice3HousekeepingProtocol.return_value = mock_housekeepingprotocol_instance
        mock_housekeepingprotocol_instance.dap_info.return_value = {
            'device_name': device,
            'product': 'nEDBG',
            'serial': 'MCHP0000111122223333'
            }

        return mock_housekeepingprotocol_instance

    def mock_programmer(self):
        """
        Mock programmer class

        :returns: programmer instance mock object
        """
        mock_programmer_patch = patch("pymcuprog.backend.Programmer")
        self.addCleanup(mock_programmer_patch.stop)
        mock_programmer = mock_programmer_patch.start()
        mock_programmer_instance = MagicMock()
        mock_programmer.return_value = mock_programmer_instance

        return mock_programmer_instance

    @staticmethod
    def configure_mock_programmer_with_device_memory_info(mock_programmer, devicename):
        """
        Configure a mock of the Programmer class with device memory info

        Since the complete programmer class is mocked out the device memory info must be
        generated and added to the mock
        :param mock_programmer: Mock of Programmer class instance
        """
        # Make a copy of the device info to avoid any side effects in other tests in case any test decides
        # to mess with the device info object.  If the device info is not copied any changes made will be
        # on the original device info object which will be the same used by all tests.
        device_info = copy.deepcopy(deviceinfo.getdeviceinfo(devicename))
        device_meminfo = deviceinfo.DeviceMemoryInfo(device_info)
        mock_programmer.get_device_memory_info.return_value = device_meminfo

    def mock_read_memories_from_hex(self):
        """
        Mock read_memories_from_hex helper function

        :returns: read_memories_from_hex instance mock object
        """
        mock_read_memories_from_hex_patch = patch("pymcuprog.backend.read_memories_from_hex")
        self.addCleanup(mock_read_memories_from_hex_patch.stop)
        mock_read_memories_from_hex = mock_read_memories_from_hex_patch.start()
        mock_read_memories_from_hex_instance = MagicMock()
        mock_read_memories_from_hex.return_value = mock_read_memories_from_hex_instance

        return mock_read_memories_from_hex_instance

    def test_get_api_version_returns_major_version_1_or_higher(self):
        api_version_read = self.backend.get_api_version()

        self.assertGreaterEqual(LooseVersion(api_version_read), LooseVersion('1.0'))

    def test_get_supported_devices_returns_list_of_all_devices_with_device_file(self):
        supported_devices = self.backend.get_supported_devices()

        num_devices = 0
        for filename in os.listdir("pymcuprog//deviceinfo//devices"):
            if filename not in ['__init__.py'] and filename.endswith('.py'):
                self.assertIn(filename.split('.py')[0], supported_devices)
                num_devices += 1

        self.assertEqual(num_devices, len(supported_devices))

    @patch("pyedbglib.hidtransport.cyhidapi.hid")
    def test_get_available_hid_tools_when_none_connected_returns_empty_list(self, mock_hid):
        expected_tools = []
        connected_tools = []

        mock_hid.enumerate.return_value = connected_tools

        tools = self.backend.get_available_hid_tools()

        self.assertEqual(expected_tools, tools)

    @patch("pyedbglib.hidtransport.cyhidapi.hid")
    def test_get_available_hid_tools_one_microchip_one_other_returns_only_microchip_tool(self, mock_hid):
        fake_microchip_tool_dict = {
            'vendor_id': 0x03EB,
            'product_id': 0xBEEF,
            'serial_number': 'DUMMYSERIAL1',
            'product_string': "Some Microchip Tool",
            'manufacturer_string': "Microchip"
        }
        fake_other_tool_dict = {
            'vendor_id': 0xDEAD,
            'product_id': 0xBEEF,
            'serial_number': 'DUMMYSERIAL2',
            'product_string': "Some other Tool",
            'manufacturer_string': "Some CORP"
        }

        fake_microchip_tool = HidTool(
            fake_microchip_tool_dict['vendor_id'],
            fake_microchip_tool_dict['product_id'],
            fake_microchip_tool_dict['serial_number'],
            fake_microchip_tool_dict['product_string'],
            fake_microchip_tool_dict['manufacturer_string']
        )

        expected_tools = [fake_microchip_tool]
        connected_tools_dicts = [fake_microchip_tool_dict, fake_other_tool_dict]

        mock_hid.enumerate.return_value = connected_tools_dicts

        tools = self.backend.get_available_hid_tools()

        self.assertEqual(len(tools), len(expected_tools), msg="Incorrect number of tools detected")
        self.assertEqual(tools[0].serial_number, expected_tools[0].serial_number)

    @patch("pyedbglib.hidtransport.cyhidapi.hid")
    def test_get_available_hid_tools_serial_number_matching(self, mock_hid):
        # Note: pyedbglib uses "serial_number" instead of "serialnumber"
        fake_microchip_tool_1_dict = {
            'vendor_id': 0x03EB,
            'product_id': 0xBEEF,
            'serial_number': 'DUMMYSERIAL1',
            'product_string': "First Microchip Tool",
            'manufacturer_string': "Microchip"
        }
        fake_microchip_tool_2_dict = {
            'vendor_id': 0x03EB,
            'product_id': 0xBEEF,
            'serial_number': 'DUMMYSERIAL2',
            'product_string': "Second Microchip Tool",
            'manufacturer_string': "Microchip"
        }

        fake_microchip_tool_1 = HidTool(
            fake_microchip_tool_1_dict['vendor_id'],
            fake_microchip_tool_1_dict['product_id'],
            fake_microchip_tool_1_dict['serial_number'],
            fake_microchip_tool_1_dict['product_string'],
            fake_microchip_tool_1_dict['manufacturer_string']
        )

        expected_tools = [fake_microchip_tool_1]
        connected_tools_dicts = [fake_microchip_tool_1_dict, fake_microchip_tool_2_dict]

        mock_hid.enumerate.return_value = connected_tools_dicts

        tools = self.backend.get_available_hid_tools("SERIAL1")

        self.assertEqual(len(tools), len(expected_tools))
        self.assertEqual(tools[0].serial_number, expected_tools[0].serial_number)

    def test_connect_to_tool_usb_hid_connection_calls_hid_transport_with_correct_serial_and_product(self):
        mock_hid_transport = self.mock_hid_transport()
        self.mock_housekeepingprotocol()

        serial = "DUMMYSERIAL"
        tool = "DUMMYTOOL"
        connection = ToolUsbHidConnection(serialnumber=serial, tool_name=tool)

        self.backend.connect_to_tool(connection)

        mock_hid_transport.connect.assert_called_with(serial_number=serial, product=tool)

    def test_connect_to_tool_serial_connection_sets_correct_serialport_and_does_not_connect_to_hid_tool(self):
        mock_hid_transport = self.mock_hid_transport()
        self.mock_housekeepingprotocol()

        serialport = "COM33"
        connection = ToolSerialConnection(serialport=serialport)

        self.backend.connect_to_tool(connection)

        mock_hid_transport.connect.assert_not_called()
        self.assertEqual(self.backend.transport, serialport)

    def test_connect_to_tool_with_invalid_configuration_raises_pymcuprogtoolconfigurationerror(self):
        with self.assertRaises(PymcuprogToolConfigurationError):
            self.backend.connect_to_tool(ToolConnection())

    @patch("pyedbglib.hidtransport.cyhidapi.hid")
    def test_connect_to_tool_when_no_tools_connected_raises_pymcuprogtoolconnectionerror(self, mock_hid):
        mock_hid.enumerate.return_value = []
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.connect_to_tool(ToolUsbHidConnection())

    def test_connect_to_tool_when_hid_transport_connect_fails_raises_pymcuprogtoolconnectionerror(self):
        self.mock_hid_transport(connect_return_value=False)
        self.mock_housekeepingprotocol()

        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.connect_to_tool(ToolUsbHidConnection())

    def test_disconnect_from_tool_when_not_connected_does_not_disconnect_hid(self):
        mock_hid_transport = self.mock_hid_transport()
        self.mock_housekeepingprotocol()

        self.backend.disconnect_from_tool()

        mock_hid_transport.disconnect.assert_not_called()

    def test_disconnect_from_tool_when_connected_to_serialport_does_not_disconnect_hid_nor_end_housekeeping_session(
            self):
        mock_hid_transport = self.mock_hid_transport()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.disconnect_from_tool()

        mock_hid_transport.disconnect.assert_not_called()
        mock_housekeepingprotocol.end_session.assert_not_called()

    def test_disconnect_from_tool_when_connected_to_hid_does_disconnect_hid_and_end_housekeeping_session(self):
        mock_hid_transport = self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.disconnect_from_tool()

        mock_hid_transport.disconnect.assert_called()
        mock_housekeepingprotocol.end_session.assert_called()

    def test_read_tool_info_when_not_connected_to_any_tool_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_tool_info()

    def test_read_tool_info_when_connected_to_serialport_raises_pymcuprogtoolconnectionerror(self):
        self.backend.connect_to_tool(ToolSerialConnection())
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_tool_info()

    def test_read_tool_info(self):
        device = "DUMMY_DEVICE"
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol(device)
        self.backend.connect_to_tool(ToolUsbHidConnection())
        tool_info = self.backend.read_tool_info()

        # Sanity check of some of the tool_info content
        self.assertTrue('device_name' in tool_info, msg="No device_name in tool_info")
        self.assertEqual(tool_info['device_name'], device, msg="Incorrect device_name in tool_info")
        self.assertTrue('product' in tool_info, msg="No product in tool_info")
        self.assertEqual(tool_info['product'], 'nEDBG', "Incorrect product in tool_info")

    def test_read_kit_device_when_kit_has_device_in_config_returns_device_name(self):
        device_expected = 'DUMMYDEVICE'

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol(device=device_expected)

        self.backend.connect_to_tool(ToolUsbHidConnection())

        device_read = self.backend.read_kit_device()

        self.assertEqual(device_read, device_expected.lower())

    def test_read_kit_device_when_connected_to_serialport_raises_pymcuprogtoolconnectionerror(self):
        self.backend.connect_to_tool(ToolSerialConnection())

        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_kit_device()

    def test_read_kit_device_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_kit_device()

    def test_read_kit_device_when_kit_does_not_have_device_in_config_returns_none(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())

        device_read = self.backend.read_kit_device()

        self.assertIsNone(device_read)

    def test_read_target_voltage_returns_3_3v(self):
        voltage_expected = 3.3

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()

        mock_housekeepingprotocol = self.mock_housekeepingprotocol()
        mock_housekeepingprotocol.get_le16.return_value = int(voltage_expected*1000) & 0xFFFF

        self.backend.connect_to_tool(ToolUsbHidConnection())
        voltage_read = self.backend.read_target_voltage()

        self.assertAlmostEqual(voltage_read, voltage_expected)
        mock_housekeepingprotocol.get_le16.assert_called_with(
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_ANALOG_VTREF)

    def test_read_target_voltage_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_target_voltage()

    def test_read_target_voltage_when_connected_to_serialport_raises_pymcuprogtoolconnectionerror(self):
        self.backend.connect_to_tool(ToolSerialConnection())
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_target_voltage()

    def test_read_target_voltage_when_not_supported_raises_pymcuprognotsupportederror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()
        mock_housekeepingprotocol.get_le16.side_effect = Jtagice3ResponseError('', [])

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogNotSupportedError):
            self.backend.read_target_voltage()

    def test_read_supply_voltage_setpoint_returns_3_3v(self):
        voltage_expected = 3.3

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()
        mock_housekeepingprotocol.get_le16.return_value = int(voltage_expected*1000) & 0xFFFF

        self.backend.connect_to_tool(ToolUsbHidConnection())
        voltage_read = self.backend.read_supply_voltage_setpoint()

        self.assertAlmostEqual(voltage_read, voltage_expected)
        mock_housekeepingprotocol.get_le16.assert_called_with(
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_TSUP_VOLTAGE)

    def test_read_supply_voltage_setpoint_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_supply_voltage_setpoint()

    def test_read_supply_voltage_setpoint_when_connected_to_serialport_raises_pymcuprogtoolconnectionerror(self):
        self.backend.connect_to_tool(ToolSerialConnection())
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_supply_voltage_setpoint()

    def test_read_supply_voltage_setpoint_when_not_supported_raises_pymcuprognotsupportederror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()
        mock_housekeepingprotocol.get_le16.side_effect = Jtagice3ResponseError('', [])

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogNotSupportedError):
            self.backend.read_supply_voltage_setpoint()

    def test_read_usb_voltage_returns_5v(self):
        voltage_expected = 5.0

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()
        mock_housekeepingprotocol.get_le16.return_value = int(voltage_expected*1000) & 0xFFFF

        self.backend.connect_to_tool(ToolUsbHidConnection())
        voltage_read = self.backend.read_usb_voltage()

        self.assertAlmostEqual(voltage_read, voltage_expected)
        mock_housekeepingprotocol.get_le16.assert_called_with(
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_ANALOG_VUSB)

    def test_read_usb_voltage_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_usb_voltage()

    def test_read_usb_voltage_when_connected_to_serialport_raises_pymcuprogtoolconnectionerror(self):
        self.backend.connect_to_tool(ToolSerialConnection())
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_usb_voltage()

    def test_read_usb_voltage_when_not_supported_raises_pymcuprognotsupportederror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()
        mock_housekeepingprotocol.get_le16.side_effect = Jtagice3ResponseError('', [])

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogNotSupportedError):
            self.backend.read_usb_voltage()

    def test_set_supply_voltage_setpoint_5v_ok(self):
        voltage_setpoint = 5.0

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.set_supply_voltage_setpoint(voltage_setpoint)

        mock_housekeepingprotocol.set_le16.assert_called_with(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
                                                              Jtagice3HousekeepingProtocol.HOUSEKEEPING_TSUP_VOLTAGE,
                                                              int(voltage_setpoint*1000) & 0xFFFF)

    def test_set_supply_voltage_setpoint_0v_ok(self):
        voltage_setpoint = 0.0

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.set_supply_voltage_setpoint(voltage_setpoint)

        mock_housekeepingprotocol.set_le16.assert_called_with(
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
            Jtagice3HousekeepingProtocol.HOUSEKEEPING_TSUP_VOLTAGE, int(voltage_setpoint*1000) & 0xFFFF)

    def test_set_supply_voltage_setpoint_1v_raises_valueerror(self):
        voltage_setpoint = 1.0

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()

        # pyedbglib throws generic Exception when SET command fails (see DSG-1494)
        mock_housekeepingprotocol.set_le16.side_effect = Exception(
            "Unable to SET (failure code 0x{:02X})".format(Jtagice3Protocol.SETGET_FAILURE_INVALID_VALUE))

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(ValueError):
            self.backend.set_supply_voltage_setpoint(voltage_setpoint)

    def test_set_supply_voltage_setpoint_6v_raises_valueerror(self):
        voltage_setpoint = 6.0

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()

        # pyedbglib throws generic Exception when SET command fails (see DSG-1494)
        mock_housekeepingprotocol.set_le16.side_effect = Exception(
            "Unable to SET (failure code 0x{:02X})".format(Jtagice3Protocol.SETGET_FAILURE_INVALID_VALUE))

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(ValueError):
            self.backend.set_supply_voltage_setpoint(voltage_setpoint)

    def test_set_supply_voltage_setpoint_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.set_supply_voltage_setpoint(3.3)

    def test_set_supply_voltage_setpoint_when_connected_to_serialport_raises_pymcuprogtoolconnectionerror(self):
        self.backend.connect_to_tool(ToolSerialConnection())
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.set_supply_voltage_setpoint(3.3)

    def test_set_usb_voltage_setpoint_when_not_supported_raises_pymcuprognotsupportederror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()
        mock_housekeepingprotocol.get_le16.side_effect = Jtagice3ResponseError('', [])

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogNotSupportedError):
            self.backend.set_supply_voltage_setpoint(3.3)

    def test_reboot_tool(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        mock_housekeepingprotocol = self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.reboot_tool()

        mock_housekeepingprotocol.end_session.assert_called_with(reset_tool=True)

    def test_reboot_tool_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.reboot_tool()

    def test_reboot_tool_when_connected_to_serial_port_raises_pymcuprogtoolconnectionerror(self):
        self.backend.connect_to_tool(ToolSerialConnection())
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.reboot_tool()

    def test_reboot_tool_the_second_time_without_another_connect_raises_pymcuprogtoolconnectionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool(True)
        self.mock_housekeepingprotocol()
        self.backend.connect_to_tool(ToolUsbHidConnection())
        # After the tool has rebooted the _is_connected_to_hid_tool should return False
        self.mock_is_connected_to_hid_tool(False)
        with self.assertRaises(PymcuprogToolConnectionError):
            # This checks that the backend knows that a reboot_tool disconnects the tool
            self.backend.reboot_tool()

    def test_get_device_info_when_device_is_not_supported_raises_pymcuprognotsupportederror(self):
        with self.assertRaises(PymcuprogNotSupportedError):
            self.backend.get_device_info('nodevice')

    def test_get_device_info_all_lowercase_supported_device(self):
        device = 'pic16f18446'
        info = self.backend.get_device_info(device)

        # Check that the device info for the correct device was returned.  The rest of the content
        # won't be checked as that should be tested as part of separate device info tests
        self.assertEqual(info[DeviceInfoKeys.NAME], device)

    def test_get_device_info_half_uppercase_supported_device(self):
        device = 'piC16F18446'
        info = self.backend.get_device_info(device)

        # Check that the device info for the correct device was returned and that the device name was
        # converted to lowercase
        self.assertEqual(info[DeviceInfoKeys.NAME], device.lower())

    def test_start_session_when_no_tool_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.start_session(SessionConfig('pic16f18446'))

    def test_start_session_propagates_all_config_parameters(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        config = SessionConfig('pic16f18446')
        config.interface = 'icsp'
        config.interface_speed = 500000
        config.packpath = "dsspath"
        config.special_options = {"dummyoption": True}

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)

        mock_programmer.load_device.assert_called_with(config.device)
        mock_programmer.setup_device.assert_called_with(
            config.interface,
            config.packpath,
            config.interface_speed)
        mock_programmer.set_options.assert_called_with(config.special_options)
        mock_programmer.start.assert_called()

    @patch("pymcuprog.backend.Backend.end_session")
    def test_start_session_when_already_started_does_end_session_and_start_new_session(self, mock_end_session):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        config1 = SessionConfig('pic16f18446')
        config2 = SessionConfig('atmega4809')

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config1)
        self.backend.start_session(config2)

        mock_end_session.assert_called()
        mock_programmer.assert_has_calls(
            [
                call.load_device(config1.device),
                call.load_device(config2.device)
            ],
            any_order=True
        )

    def test_start_session_when_device_is_empty_string_raises_pymcuprogsessionconfigerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        config = SessionConfig('')

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionConfigError):
            self.backend.start_session(config)

    @patch("pymcuprog.programmer.get_nvm_access_provider")
    def test_start_session_when_setup_device_returns_none_raises_pymcuprogsessoinconfigerror(
            self,
            mock_get_nvm_access_provider):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_get_nvm_access_provider.return_value = None

        config = SessionConfig('atmega4809')

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionConfigError):
            self.backend.start_session(config)

    @patch("pymcuprog.programmer.get_nvm_access_provider")
    def test_start_session_when_nvmupdi_raises_pymcuprogdevicelockederror_propagates(
            self,
            mock_get_nvm_access_provider):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_nvmaccessprovidercmsisdapupdi = MagicMock()
        mock_get_nvm_access_provider.return_value = mock_nvmaccessprovidercmsisdapupdi
        mock_nvmaccessprovidercmsisdapupdi.start.side_effect = PymcuprogDeviceLockedError()

        config = SessionConfig('atmega4809')

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogDeviceLockedError):
            self.backend.start_session(config)

    def test_start_session_when_device_is_unsupported_raises_pymcuprognotsupportederror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        config = SessionConfig('unsupported_device')

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogNotSupportedError):
            self.backend.start_session(config)

    def test_end_session_when_session_started_stops_programmer(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        config = SessionConfig('atmega4809')

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)
        self.backend.end_session()

        mock_programmer.stop.assert_called()

    def test_end_session_when_session_not_started_does_not_stop_programmer(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        self.backend.end_session()

        mock_programmer.stop.assert_not_called()

    def test_read_device_id_returns_bytearray_with_id(self):
        id_expected = bytearray([0x01, 0x02, 0x03, 0x04])

        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()
        mock_programmer.read_device_id.return_value = id_expected

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        id_read = self.backend.read_device_id()

        self.assertEqual(id_expected, id_read)

    def test_read_device_id_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_device_id()

    def test_read_device_id_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.read_device_id()
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_read_device_id_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.read_device_id()

    def test_erase_propagates_all_parameters(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'
        memory_name = MemoryNames.EEPROM
        address = 256

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(SessionConfig(target))
        self.backend.erase(memory_name=memory_name, address=address)

        mock_programmer.erase.assert_called_with(memory_name, address)

    def test_erase_when_isolated_erase_false_raises_pymcuprogeraseerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        # lockbits can't be erased in isolation on UPDI parts
        memory_name = MemoryNames.LOCKBITS
        address = 0

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        with self.assertRaises(PymcuprogEraseError):
            self.backend.erase(memory_name=memory_name, address=address)

        mock_programmer.erase.assert_not_called()

    def test_erase_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.erase()

    def test_erase_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.erase()

    def test_erase_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.erase()
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_read_memory_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.read_memory()

    def test_read_memory_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.read_memory()

    def test_read_memory_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.read_memory()
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_read_memory_propagates_all_parameters_and_returns_data_and_memoryinfo(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        # The actual read is done by the programmer, the backend just forwards the call.
        # So the test just has to check that the backend forwards parameters correctly and returns the result correctly

        memory_name = 'some_memory'
        offset = 0x1000
        numbytes = 256
        data_expected = bytearray([])
        # Generate dummy data for the read to return
        for index in range(numbytes):
            data_expected.append(index & 0xFF)
        # Just make some fake memory info
        meminfo_expected = {DeviceMemoryInfoKeys.NAME: memory_name}

        memory_read_tuple_expected = namedtuple("Memory", 'data memory_info')
        memory_read_tuple_expected.data = data_expected
        memory_read_tuple_expected.memory_info = meminfo_expected

        mock_programmer.read_memory.return_value = [memory_read_tuple_expected]

        self.backend.connect_to_tool(ToolUsbHidConnection())
        # Just pick any device
        self.backend.start_session(SessionConfig('atmega4809'))
        read_list = self.backend.read_memory(memory_name=memory_name, offset_byte=offset, numbytes=numbytes)

        data_read = read_list[0].data
        meminfo_read = read_list[0].memory_info

        self.assertEqual(len(read_list), 1, msg="read_memory returned more than one memory segment")

        mock_programmer.read_memory.assert_called_with(memory_name=memory_name, offset=offset, numbytes=numbytes)

        self.assertEqual(data_read, data_expected)
        self.assertEqual(meminfo_read, meminfo_expected)

    def test_write_memory_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.write_memory(bytearray([]))

    def test_write_memory_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.write_memory(bytearray([]))
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_write_memory_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.write_memory(bytearray([]))

    def test_write_memory_propagates_all_parameters(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        # The actual write is done by the programmer, the backend just forwards the call.
        # So the test just has to check that the backend forwards parameters correctly

        memory_name = 'some_memory'
        offset = 0x1000
        numbytes = 256
        data_expected = bytearray([])
        # Generate dummy data for the write
        for index in range(numbytes):
            data_expected.append(index & 0xFF)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        # Just pick any device
        self.backend.start_session(SessionConfig('atmega4809'))
        self.backend.write_memory(data=data_expected, memory_name=memory_name, offset_byte=offset)

        mock_programmer.write_memory.assert_called_with(data=data_expected, memory_name=memory_name, offset=offset)

    def test_verify_memory_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.verify_memory(bytearray([]))

    def test_verify_memory_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.verify_memory(bytearray([]))
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_verify_memory_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.verify_memory(bytearray([]))

    def test_verify_memory_propagates_all_parameters_and_returns_result(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()
        mock_programmer.verify_memory.return_value = True

        # The actual verify is done by the programmer, the backend just forwards the call.
        # So the test just has to check that the backend forwards parameters correctly and returns the result correctly

        memory_name = 'some_memory'
        offset = 0x1000
        numbytes = 256
        data_expected = bytearray([])
        # Generate dummy data for the verify
        for index in range(numbytes):
            data_expected.append(index & 0xFF)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        # Just pick any device
        self.backend.start_session(SessionConfig('atmega4809'))
        verify_result = self.backend.verify_memory(data=data_expected, memory_name=memory_name, offset_byte=offset)

        mock_programmer.verify_memory.assert_called_with(data=data_expected, memory_name=memory_name, offset=offset)
        self.assertTrue(verify_result, msg="Verify did not return success")

    def test_hold_in_reset_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.hold_in_reset()

    def test_hold_in_reset_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.hold_in_reset()
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_hold_in_reset_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.hold_in_reset()

    def test_hold_in_reset_propagates(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        # Just pick any device
        self.backend.start_session(SessionConfig('atmega4809'))
        self.backend.hold_in_reset()

        mock_programmer.hold_in_reset.assert_called()

    def test_release_from_reset_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.release_from_reset()

    def test_release_from_reset_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.release_from_reset()

    def test_release_from_reset_propagates(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        # Just pick any device
        self.backend.start_session(SessionConfig('atmega4809'))
        self.backend.release_from_reset()

        mock_programmer.release_from_reset.assert_called()

    def test_release_from_reset_twice_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        self.mock_programmer()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        # Just pick any device
        self.backend.start_session(SessionConfig('atmega4809'))
        self.backend.release_from_reset()

        # Calling the release_from_reset should now fail with a Session error because the previous
        # release_from_reset took down the session
        with self.assertRaises(PymcuprogSessionError):
            self.backend.release_from_reset()

    def test_write_hex_to_target_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.write_hex_to_target('nonexisting.hex')

    def test_write_hex_to_target_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.mock_read_memories_from_hex()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.write_hex_to_target('nonexisting.hex')
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_write_hex_to_target_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_read_memories_from_hex()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.write_hex_to_target('nonexisting.hex')

    def test_write_hex_to_target_propagates_all_parameters(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        # The actual write is done by the programmer, the backend just forwards the call.
        # So the test just has to check that the backend forwards parameters correctly

        target = 'atmega4809'
        flash_hexfile_address = 0
        eeprom_hexfile_address = 0x810000

        filename = "{}flash_eeprom.hex".format(TESTFILE_FOLDER)

        flash_numbytes = 256
        flash_offset = 0
        flash_data = bytearray([])
        for count in range(flash_numbytes):
            flash_data.append(count & 0xFF)

        eeprom_numbytes = 32
        eeprom_offset = 8
        eeprom_data = bytearray([])
        for count in range(eeprom_numbytes):
            eeprom_data.append(count & 0xFF)

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        # Make a hex file with a Flash segment and an EEPROM segment
        ihex = IntelHex()
        hexindex = flash_offset + flash_hexfile_address
        for databyte in flash_data:
            ihex[hexindex] = databyte
            hexindex += 1
        hexindex = eeprom_offset + eeprom_hexfile_address
        for databyte in eeprom_data:
            ihex[hexindex] = databyte
            hexindex += 1
        _write_hex_to_file(ihex, filename)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(SessionConfig(target))
        self.backend.write_hex_to_target(filename)

        mock_programmer.assert_has_calls(
            [
                call.write_memory(data=flash_data, memory_name=MemoryNames.FLASH, offset=flash_offset),
                call.write_memory(data=eeprom_data, memory_name=MemoryNames.EEPROM, offset=eeprom_offset)
            ]
        )

    def test_verify_hex_when_not_connected_raises_pymcuprogtoolconnectionerror(self):
        with self.assertRaises(PymcuprogToolConnectionError):
            self.backend.verify_hex('nonexisting.hex')

    def test_verify_hex_when_connected_to_serialport_does_not_raise_pymcuprogtoolconnection(self):
        self.mock_programmer()
        self.mock_read_memories_from_hex()
        self.backend.connect_to_tool(ToolSerialConnection())
        self.backend.start_session(SessionConfig('atmega4809'))
        try:
            self.backend.verify_hex('nonexisting.hex')
        except PymcuprogToolConnectionError as error:
            self.fail("PymcuprogToolConnectionError raised: {}".format(error))

    def test_verify_hex_when_session_not_started_raises_pymcuprogsessionerror(self):
        self.mock_hid_transport()
        self.mock_read_memories_from_hex()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()

        self.backend.connect_to_tool(ToolUsbHidConnection())
        with self.assertRaises(PymcuprogSessionError):
            self.backend.verify_hex('nonexisting.hex')

    def test_verify_hex_ok_propagates_all_parameters(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        # The actual verify is done by the programmer, the backend just forwards the call.
        # So the test just has to check that the backend forwards parameters correctly

        target = 'atmega4809'
        flash_hexfile_address = 0
        eeprom_hexfile_address = 0x810000

        filename = "{}flash_eeprom.hex".format(TESTFILE_FOLDER)

        flash_numbytes = 256
        flash_offset = 0
        flash_data = bytearray([])
        for count in range(flash_numbytes):
            flash_data.append(count & 0xFF)

        eeprom_numbytes = 32
        eeprom_offset = 8
        eeprom_data = bytearray([])
        for count in range(eeprom_numbytes):
            eeprom_data.append(count & 0xFF)

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        # Pretend all verifications were successful
        mock_programmer.verify_memory.return_value = True

        # Make a hex file with a Flash segment and an EEPROM segment
        ihex = IntelHex()
        hexindex = flash_offset + flash_hexfile_address
        for databyte in flash_data:
            ihex[hexindex] = databyte
            hexindex += 1
        hexindex = eeprom_offset + eeprom_hexfile_address
        for databyte in eeprom_data:
            ihex[hexindex] = databyte
            hexindex += 1
        _write_hex_to_file(ihex, filename)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(SessionConfig(target))
        verify_result = self.backend.verify_hex(filename)

        mock_programmer.assert_has_calls(
            [
                call.verify_memory(data=flash_data, memory_name=MemoryNames.FLASH, offset=flash_offset),
                call.verify_memory(data=eeprom_data, memory_name=MemoryNames.EEPROM, offset=eeprom_offset)
            ]
        )

        self.assertTrue(verify_result, msg="Successful verify should return True")

    def test_verify_hex_fails_returns_false(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'
        flash_hexfile_address = 0

        filename = "{}flash.hex".format(TESTFILE_FOLDER)

        flash_numbytes = 4
        flash_offset = 0
        flash_data = bytearray([])
        for count in range(flash_numbytes):
            flash_data.append(count & 0xFF)

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        # Pretend verification failed
        mock_programmer.verify_memory.return_value = False

        # Make a hex file with a Flash segment
        ihex = IntelHex()
        hexindex = flash_offset + flash_hexfile_address
        for databyte in flash_data:
            ihex[hexindex] = databyte
            hexindex += 1
        _write_hex_to_file(ihex, filename)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(SessionConfig(target))
        verify_result = self.backend.verify_hex(filename)

        self.assertFalse(verify_result, msg="Failing verify should return False")

    def test_is_isolated_erase_possible_flash_mega4809_returns_true(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        config = SessionConfig(target)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)

        self.assertTrue(self.backend.is_isolated_erase_possible(MemoryNames.FLASH))

    def test_is_isolated_erase_possible_eeprom_mega4809_returns_true(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        config = SessionConfig(target)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)

        self.assertTrue(self.backend.is_isolated_erase_possible(MemoryNames.EEPROM))

    def test_is_isolated_erase_possible_when_key_is_missing_returns_false(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'

        # Copy the device info before messing with it to avoid altering the original device info as it
        # will mess up other tests
        device_info = copy.deepcopy(deviceinfo.getdeviceinfo(target))
        # Remove the isolated_erase flag for eeprom
        device_info.pop('eeprom_isolated_erase')
        device_meminfo = deviceinfo.DeviceMemoryInfo(device_info)
        # Let the modified memory info be returned by the programmer mock
        mock_programmer.get_device_memory_info.return_value = device_meminfo

        config = SessionConfig(target)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)

        self.assertFalse(self.backend.is_isolated_erase_possible(MemoryNames.EEPROM))

    def test_get_chiperase_effect_eeprom_mega4809_returns_conditionally_erased(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        config = SessionConfig(target)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)

        self.assertEqual(self.backend.get_chiperase_effect(MemoryNames.EEPROM),
                         ChiperaseEffect.CONDITIONALLY_ERASED_AVR)

    def test_get_chiperase_effect_flash_mega4809_returns_always_erased(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'

        self.configure_mock_programmer_with_device_memory_info(mock_programmer, target)

        config = SessionConfig(target)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)

        self.assertEqual(self.backend.get_chiperase_effect(MemoryNames.FLASH), ChiperaseEffect.ALWAYS_ERASED)

    def test_get_chiperase_effect_when_key_is_missing_returns_not_erased(self):
        self.mock_hid_transport()
        self.mock_is_connected_to_hid_tool()
        self.mock_housekeepingprotocol()
        mock_programmer = self.mock_programmer()

        target = 'atmega4809'

        # Copy the device info before messing with it to avoid altering the original device info as it
        # will mess up other tests
        device_info = copy.deepcopy(deviceinfo.getdeviceinfo(target))
        # Remove the chiperase_effect flag for flash
        device_info.pop('flash_chiperase_effect')
        device_meminfo = deviceinfo.DeviceMemoryInfo(device_info)
        # Let the modified memory info be returned by the programmer mock
        mock_programmer.get_device_memory_info.return_value = device_meminfo

        config = SessionConfig(target)

        self.backend.connect_to_tool(ToolUsbHidConnection())
        self.backend.start_session(config)

        self.assertEqual(self.backend.get_chiperase_effect(MemoryNames.FLASH), ChiperaseEffect.NOT_ERASED)
