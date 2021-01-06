import unittest
from mock import patch
from mock import Mock

from pyedbglib.hidtransport.hidtransportbase import HidTransportBase, HidTool

# Some dummy tools used in the tests
NEDBG_PRODUCTSTRING = "nEDBG CMSIS-DAP"
NEDBG_1 = HidTool(serial_number="MCHP00000000000000061", product_string=NEDBG_PRODUCTSTRING, vendor_id="", product_id="")
NEDBG_2 = HidTool(serial_number="ATML0000000000001561", product_string=NEDBG_PRODUCTSTRING, vendor_id="", product_id="")
NEDBG_3 = HidTool(serial_number="ATML0000000000006100", product_string=NEDBG_PRODUCTSTRING, vendor_id="", product_id="")
ATMELICE_1 = HidTool(serial_number="J41800000081", product_string="Atmel-ICE CMSIS-DAP", vendor_id="", product_id="")
PICKIT4_1 = HidTool(serial_number="BUR181875661", product_string="MPLAB PICkit 4 CMSIS-DAP", vendor_id="", product_id="")

NEDBG_TOOLS = [NEDBG_1, NEDBG_2, NEDBG_3]
ALL_TOOLS = [ATMELICE_1, PICKIT4_1]
ALL_TOOLS.extend(NEDBG_TOOLS)

class TestGetMatchingTools(unittest.TestCase):
    """Tests for the get_matching_tools function in hidtransportbase.HidTranpsortBase"""

    def setUp(self):
        mock_detect_devices_patch = patch("pyedbglib.hidtransport.hidtransportbase.HidTransportBase.detect_devices")
        self.addCleanup(mock_detect_devices_patch.stop)
        mock_detect_devices = mock_detect_devices_patch.start()
        self.transport = HidTransportBase()

    def _assert_toollist_is_list_of_all_nedbgs(self, toollist):
        """
        Check that the toollist contains all nEDBGs and only nEDBGS
        """
        for tool in toollist:
            self.assertEqual(tool.product_string, NEDBG_PRODUCTSTRING)

        self.assertEqual(len(toollist), len(NEDBG_TOOLS))

        for nedbg in NEDBG_TOOLS:
            self.assertTrue(nedbg in toollist)

    def test_match_anything_when_one_tool_connnected_returns_list_of_one_item(self):
        CONNECTED_DEVICES = [
            NEDBG_1
        ]
        self.transport.devices = CONNECTED_DEVICES
        toollist = self.transport.get_matching_tools()
        self.assertEqual(len(toollist), 1)
        self.assertEqual(toollist[0].serial_number, CONNECTED_DEVICES[0].serial_number)

    def test_match_anything_when_nothing_connected_returns_empty_list(self):
        toollist = self.transport.get_matching_tools()
        self.assertEqual(len(toollist), 0)

    def test_match_product_none_serial_empty_returns_list_with_all_devices(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring='', product=None)
        self.assertEqual(toollist, ALL_TOOLS)

    def test_match_product_none_serial_none_returns_list_with_all_devices(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring=None, product=None)
        self.assertEqual(toollist, ALL_TOOLS)

    def test_match_product_nedbg_serial_none_returns_list_with_all_nedbg_devices(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring=None, product='nedbg')
        self._assert_toollist_is_list_of_all_nedbgs(toollist)

    def test_match_product_nedbg_serial_empty_returns_list_with_all_nedbgs(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring="", product="nedbg")
        self._assert_toollist_is_list_of_all_nedbgs(toollist)

    def test_match_product_NEDBG_serial_empty_returns_list_with_all_nedbgs(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring="", product="NEDBG")
        self._assert_toollist_is_list_of_all_nedbgs(toollist)

    def test_match_product_pickit4_serial_empty_returns_list_with_pickit4(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring="", product="pickit4")
        self.assertEqual(len(toollist), 1)
        self.assertTrue(PICKIT4_1 in toollist)

    def test_match_product_pickit4_serial_empty_returns_list_with_pickit4(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring="", product="pickit4")
        self.assertEqual(len(toollist), 1)
        self.assertTrue(PICKIT4_1 in toollist)

    def test_match_product_MPLAB_PICkit_4_serial_empty_returns_list_with_pickit4(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring="", product="MPLAB PICkit 4")
        self.assertEqual(len(toollist), 1)
        self.assertTrue(PICKIT4_1 in toollist)

    def test_match_product_none_full_serial_returns_list_with_one_tool(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring=NEDBG_1.serial_number, product=None)
        self.assertEqual(len(toollist), 1)
        self.assertTrue(NEDBG_1 in toollist)

    def test_match_product_nedbg_serial_substring_61_only_matches_serials_ending_in_61(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring='61', product="nedbg")
        self.assertEqual(len(toollist), 2)
        self.assertTrue(NEDBG_1 in toollist)
        self.assertTrue(NEDBG_2 in toollist)
        self.assertFalse(NEDBG_3 in toollist)

    def test_match_product_pickit4_serial_of_connected_nedbg_returns_empty_list(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring=NEDBG_3.serial_number, product="pickit4")
        self.assertEqual(toollist, [])

    def test_match_product_none_serial_substring_1_returns_list_of_all_tools_with_serial_ending_in_1(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring="1", product=None)
        expected_tools = []
        for tool in ALL_TOOLS:
            if tool.serial_number.endswith("1"):
                expected_tools.append(tool)

        self.assertEqual(len(expected_tools), len(toollist))
        for tool in expected_tools:
            self.assertTrue(tool in toollist)

    def test_match_serial_is_case_insensitive(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring=NEDBG_1.serial_number.lower())
        self.assertEqual(toollist, [NEDBG_1])

    def test_exact_match_full_serial_and_tool_shortname(self):
        self.transport.devices = ALL_TOOLS
        toollist = self.transport.get_matching_tools(serial_number_substring=ATMELICE_1.serial_number, product='atmelice')
        self.assertEqual(toollist, [ATMELICE_1])
