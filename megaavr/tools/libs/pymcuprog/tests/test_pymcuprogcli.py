# The intention is to make the test names descriptive enough to not need any docstrings for most of them
#pylint: disable=missing-docstring
# It seems better to have all tests for one module in the same file than to split across multiple files,
# so accepting many public methods and many lines makes sense
#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
# Some tests require a lot of configurable parameters so there will be many local variables
#pylint: disable=too-many-locals
"""
pymcuprog CLI (integration) tests
"""
import unittest
import sys
import os

import shutil
import io
import logging
import time
import tempfile
from copy import copy
from array import array
from argparse import Namespace
from intelhex import IntelHex
try:
  from pathlib import Path
except ImportError:
  from pathlib2 import Path  # python 2 backport

from mock import patch
from mock import Mock
from mock import call

# pymcuprog main function
import pymcuprog.pymcuprog_main as pymcuprog

from pymcuprog.deviceinfo.memorynames import MemoryNames

from pyedbglib.protocols.avr8protocol import Avr8Protocol

# Hex file offsets for AVR for all memories that can be written
HEX_OFFSETS_AVR = {
    MemoryNames.FLASH: 0x000000,
    MemoryNames.EEPROM: 0x810000,
    MemoryNames.FUSES: 0x820000,
    MemoryNames.LOCKBITS: 0x830000,
    MemoryNames.USER_ROW: 0x850000}

# Hex file offsets for PIC for all memories that can be written
HEX_OFFSETS_PIC = {
    MemoryNames.FLASH: 0x000000,
    MemoryNames.EEPROM: 0xF000*2,
    MemoryNames.ICD: 0x8600*2,
    MemoryNames.CONFIG_WORD: 0x8007*2,
    MemoryNames.USER_ID: 0x8000*2}

PAGE_SIZES_AVR = {
    MemoryNames.FLASH: 128,
    MemoryNames.EEPROM: 64,
    MemoryNames.FUSES: 1,
    MemoryNames.LOCKBITS: 1,
    MemoryNames.USER_ROW: 64}

PAGE_SIZES_PIC = {
    MemoryNames.FLASH: 64,
    MemoryNames.EEPROM: 2,
    MemoryNames.ICD: 64,
    MemoryNames.CONFIG_WORD: 2,
    MemoryNames.USER_ID: 2}

MEMTYPES_AVR_READ = {
    MemoryNames.FLASH: 'AVR8_MEMTYPE_FLASH_PAGE',
    MemoryNames.EEPROM: 'AVR8_MEMTYPE_EEPROM',
    MemoryNames.FUSES: 'AVR8_MEMTYPE_FUSES',
    MemoryNames.LOCKBITS: 'AVR8_MEMTYPE_LOCKBITS',
    MemoryNames.USER_ROW: 'AVR8_MEMTYPE_USER_SIGNATURE'}

MEMTYPES_AVR_WRITE = {
    MemoryNames.FLASH: MEMTYPES_AVR_READ[MemoryNames.FLASH],
    MemoryNames.EEPROM: 'AVR8_MEMTYPE_EEPROM_ATOMIC',
    MemoryNames.FUSES: MEMTYPES_AVR_READ[MemoryNames.FUSES],
    MemoryNames.LOCKBITS: MEMTYPES_AVR_READ[MemoryNames.LOCKBITS],
    MemoryNames.USER_ROW: MEMTYPES_AVR_READ[MemoryNames.USER_ROW]}

# Memory offsets in the target device for AVR
MEM_OFFSETS_AVR = {
    MemoryNames.FLASH: 0x4000,
    MemoryNames.EEPROM: 0x1400,
    MemoryNames.FUSES: 0x1280,
    MemoryNames.LOCKBITS: 0x128A,
    MemoryNames.USER_ROW: 0x1300}

# Memory offsets in the target device for PIC
MEM_OFFSETS_PIC = {
    MemoryNames.FLASH: 0x000000,
    MemoryNames.EEPROM: 0xF000*2,
    MemoryNames.ICD: 0x8600*2,
    MemoryNames.CONFIG_WORD: 0x8007*2,
    MemoryNames.USER_ID: 0x8000*2}

# Data values are hardcoded to different values for each memory to be able to differentiate the various memories
MEM_READ_VALUES_PIC = {
    MemoryNames.FLASH: 0xBB,
    MemoryNames.EEPROM: 0xEE,
    MemoryNames.ICD: 0xDE,
    MemoryNames.CONFIG_WORD: 0xCC,
    MemoryNames.USER_ID: 0xAA}

TESTFILE_FOLDER = os.path.realpath(os.path.normpath("{}//testpymcuprogcli".format(tempfile.gettempdir())))

DEFAULT_ARGS = Namespace(
    action='ping',
    bytes=0,
    clk=None,
    device=None,
    erase=False,
    filename=None,
    interface=None,
    literal=None,
    memory='all',
    offset=0,
    packpath=None,
    serialnumber=None,
    timing=False,
    tool=None,
    uart=None,
    verbose=False,
    verbosedebug=False,
    verify=False,
    version=False,
    release_info=False,
    high_voltage=None,
    user_row_locked_device=False,
    chip_erase_locked_device=False)

# This test class does a lot of mocking that is common to all tests so there need to be many instance attributes
#pylint: disable=too-many-instance-attributes
class TestPymcuprogCLI(unittest.TestCase):
    """
    Integration tests testing the complete pymcuprog command line utility stack
    except the lower layers which are mocked out.
    """
    def setUp(self):
        self.mock_stdout_patch = patch('sys.stdout', new_callable=io.StringIO)
        self.mock_stdout = self.mock_stdout_patch.start()
        self.mock_hidtransport_patch = patch('pymcuprog.backend.hid_transport')
        self.mock_hidtransport = self.mock_hidtransport_patch.start()
        self.mock_hidtransport_obj = self.mock_hidtransport.return_value
        self.mock_hidtransport_obj.connect.return_value = True
        self.mock_hidtransport_obj.get_report_size.return_value = 64
        self.stub_data = {MemoryNames.FLASH: [],
                          MemoryNames.EEPROM: [],
                          MemoryNames.FUSES: [],
                          MemoryNames.LOCKBITS: [],
                          MemoryNames.USER_ROW: []}
        self.mock_print_tool_info_patch = patch('pymcuprog.pymcuprog_main.print_tool_info')
        self.mock_print_tool_info = self.mock_print_tool_info_patch.start()
        # mock out the _is_connected_to_hid_tool in the API as it won't work when the hid_transport has been mocked out
        self.mock_is_connected_to_hid_tool_patch = patch("pymcuprog.backend.Backend._is_connected_to_hid_tool")
        self.mock_is_connected_to_hid_tool = self.mock_is_connected_to_hid_tool_patch.start()
        self.mock_is_connected_to_hid_tool.return_value = True


        # Remove temp folder were files are stored during the tests to get rid of any residue from previous test runs
        try:
            shutil.rmtree(TESTFILE_FOLDER)
        except FileNotFoundError:
            # Folder is already gone
            pass

        # Create an empty temp folder were files can be stored during test runs
        Path(TESTFILE_FOLDER).mkdir(exist_ok=True, parents=True)

    def tearDown(self):
        logging.info(
            "Captured output (note it arrives at the end because it was caught by a mock): \n%s",
            self.mock_stdout.getvalue())
        self.mock_stdout_patch.stop()
        self.mock_hidtransport_patch.stop()
        self.mock_print_tool_info_patch.stop()
        self.mock_is_connected_to_hid_tool_patch.stop()

    def updi_memory_read_stub(self, memtype, start, numbytes):
        """
        Stub to be used as side_effect for memory_read on UPDI devices
        """
        # pylint: disable=unused-argument
        atmega4809_sib = array('B', map(ord, "megaAVR P:0D:1-3M2 (01.59B20.0)"))
        atmega4809_id = bytearray([0x1E, 0x96, 0x51])
        updi_revision = bytearray([0x30])
        read_data = bytearray()
        # memtype might be a mock (which will return a string, see avr8protocol_mock_configure()) or
        # the real memtype value so we must check for both
        if memtype in ['AVR8_MEMTYPE_SIB', Avr8Protocol.AVR8_MEMTYPE_SIB]:
            read_data = atmega4809_sib
        elif memtype in ['AVR8_MEMTYPE_SRAM', Avr8Protocol.AVR8_MEMTYPE_SRAM]:
            read_data = atmega4809_id
        elif memtype in ['AVR8_MEMTYPE_CS', Avr8Protocol.AVR8_MEMTYPE_CS]:
            read_data = updi_revision
        elif memtype in ['AVR8_MEMTYPE_FLASH_PAGE', Avr8Protocol.AVR8_MEMTYPE_FLASH_PAGE]:
            read_data = self.stub_data[MemoryNames.FLASH]
        elif memtype in ['AVR8_MEMTYPE_EEPROM', Avr8Protocol.AVR8_MEMTYPE_EEPROM]:
            read_data = self.stub_data[MemoryNames.EEPROM]
        elif memtype in ['AVR8_MEMTYPE_FUSES', Avr8Protocol.AVR8_MEMTYPE_FUSES]:
            read_data = self.stub_data[MemoryNames.FUSES]
        elif memtype in ['AVR8_MEMTYPE_LOCKBITS', Avr8Protocol.AVR8_MEMTYPE_LOCKBITS]:
            read_data = self.stub_data[MemoryNames.LOCKBITS]
        elif memtype in ['AVR8_MEMTYPE_USER_SIGNATURE', Avr8Protocol.AVR8_MEMTYPE_USER_SIGNATURE]:
            read_data = self.stub_data[MemoryNames.USER_ROW]

        # Return only the data being asked for.
        # Note that this function does not do any address checking at all, this must be done by the test
        return read_data[0:numbytes]

    def pic16f18446_read_flash_memory_stub(self, offset, numbytes):
        """
        Stub to be used as side_effect for read_flash_memory on PIC16F18446 devices
        """
        # pylint: disable=no-self-use
        data = bytearray()
        if offset >= (0x8600*2):
            # ICD memory
            data = bytearray([MEM_READ_VALUES_PIC[MemoryNames.ICD]]*numbytes)
        elif offset >= (0x8000*2):
            # user_id memory
            data = bytearray([MEM_READ_VALUES_PIC[MemoryNames.USER_ID]]*numbytes)
        else:
            # normal flash
            data = bytearray([MEM_READ_VALUES_PIC[MemoryNames.FLASH]]*numbytes)
        return data[0:numbytes]

    def delayed_memory_read_stub(self, memory_name, start, numbytes):
        """
        Stub to be used as side_effect for memory_read when testing the timer.

        This stub will return data after a time delay
        """
        # pylint: disable=unused-argument
        # pylint: disable=no-self-use
        time.sleep(0.1)

        return [0xAA]

    @staticmethod
    def create_pic_model_mock():
        """Creates a mock for the pic debugger model"""
        # Mock out the debugprovider
        sys.modules['common.debugprovider'] = Mock()

        # Create the mock for provide_debugger_model,
        # this will be the same mock that will be imported by the module under test.
        from common.debugprovider import provide_debugger_model # pylint: disable=import-error, import-outside-toplevel

        mock_pic_model = provide_debugger_model.return_value

        return mock_pic_model

    @staticmethod
    def avr8protocol_mock_configure(mock_avr8protocol):
        """
        To be able to check for various attributes in the Avr8Protocol class the attributes used must return a string
        instead of just returning another MagicMock.  This function configures an Avr8protocol mock to
        achieve this.
        """
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_SIB='AVR8_MEMTYPE_SIB')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_SRAM='AVR8_MEMTYPE_SRAM')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_CS='AVR8_MEMTYPE_CS')
        mock_avr8protocol.configure_mock(AVR8_VARIANT_TINYX='AVR8_VARIANT_TINYX')
        mock_avr8protocol.configure_mock(AVR8_FUNC_PROGRAMMING='AVR8_FUNC_PROGRAMMING')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_FLASH_PAGE='AVR8_MEMTYPE_FLASH_PAGE')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_EEPROM='AVR8_MEMTYPE_EEPROM')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_EEPROM_ATOMIC='AVR8_MEMTYPE_EEPROM_ATOMIC')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_FUSES='AVR8_MEMTYPE_FUSES')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_LOCKBITS='AVR8_MEMTYPE_LOCKBITS')
        mock_avr8protocol.configure_mock(AVR8_MEMTYPE_USER_SIGNATURE='AVR8_MEMTYPE_USER_SIGNATURE')

    @staticmethod
    def updi_session_and_configuration_check(mock_avr8protocol_obj):
        """
        Asserts that the tool was configured properly for UPDI programming and
        that the session was started and ended properly
        """
         # Checking that the tool was set up correctly
        mock_avr8protocol_obj.set_variant.assert_called_with('AVR8_VARIANT_TINYX')
        mock_avr8protocol_obj.set_function.assert_called_with('AVR8_FUNC_PROGRAMMING')
        mock_avr8protocol_obj.set_interface.assert_called_with(Avr8Protocol.AVR8_PHY_INTF_PDI_1W)
        mock_avr8protocol_obj.activate_physical.assert_called()
        mock_avr8protocol_obj.enter_progmode.assert_called()

        # Checking that the tool was taken down correctly
        mock_avr8protocol_obj.leave_progmode.assert_called()
        mock_avr8protocol_obj.deactivate_physical.assert_called()

    @staticmethod
    def pic_session_check(mock_pic_model):
        """
        Asserts that a PIC session was correctly started and ended
        """
        # Check session initialization
        mock_pic_model.setup_session.assert_called()
        mock_pic_model.start_programming_operation.assert_called()

        # Check session tear down
        mock_pic_model.end_of_operations.assert_called()

    @staticmethod
    def generate_intelhex_with_dummy_data(memories, datasizes, offsets, memtype_indicator_start=0x00):
        """
        Generate some data for each memory type

        Using the upper nibble as a memory type indicator to make it possible to identify which
        memory the data was meant for by looking at the data values
        :param memories: Dictionary with memory names as keys and hexfile addresses as values
        :param datasizes: Dictionary with memory names as keys and number of bytes to generate as values
        :param offsets: Dictionary with memory names as keys and offsets to start generating data at
        :param memtype_indicator_start: Memory type indicator of first memory (subsequent memories will
            add 0x10 to this start value)
        :return: IntelHex instance with generated data
        """
        hexfile = IntelHex()

        datamask = 0x0F
        memindex = memtype_indicator_start
        for memname in memories:
            offset = 0
            if memname in offsets:
                offset = offsets[memname]
            for dataindex in range(datasizes[memname]):
                databyte = ((datamask & dataindex) + memindex) & 0xFF
                hexfile[dataindex + memories[memname] + offset] = databyte
            memindex = (memindex + 0x10) & 0xFF

        return hexfile

    @staticmethod
    def _is_blank(data):
        """
        Checks if a buffer represents "blank" flash
        """
        for dat in data:
            if not dat == 0xFF:
                return False
        return True

    @staticmethod
    def _pad_and_align(data, address, chunksize):
        """
        Pad the data to start on chunk boundary and to contain complete chunks

        :param data: raw data bytes
        :param address: address of the data (i.e. the start address in the memory data will be written to)
        :param chunksize: number of bytes in each chunk of data
        :return padded_data, aligned_address:
            padded_data: Original data but pre- and post-padded with 0xFF to match chunk boundaries
            aligned_address: Address adjusted to the start of a chunk
        """
        num_databytes = len(data)
        padded_data = []
        num_prepad_bytes = address % chunksize
        aligned_address = address - num_prepad_bytes
        # Add padding to align to chunk start
        padded_data.extend([0xFF] * num_prepad_bytes)
        # Add the actual data
        for index in range(num_databytes):
            padded_data.append(data[index])
        # Add padding to complete the last chunk
        num_databytes_in_last_chunk = len(padded_data) % chunksize
        num_bytes_to_pad_in_last_page = 0
        if num_databytes_in_last_chunk > 0:
            num_bytes_to_pad_in_last_page = chunksize - num_databytes_in_last_chunk
            padded_data.extend([0xFF]*num_bytes_to_pad_in_last_page)

        return padded_data, aligned_address

    @staticmethod
    def find_memoryname_pic(segment_start):
        """
        Find the memory name corresponding to the segment start address (start address in device not hexfile address)

        Note the segment_start must be the first location of a memory type
        """
        memname = None
        for memory in MEM_OFFSETS_PIC:
            if MEM_OFFSETS_PIC[memory] == segment_start:
                memname = memory

        return memname

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_ping_nedbg_mega4809(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'ping'
        arguments.tool = 'nedbg'
        arguments.device = 'aTmega4809'
        arguments.interface = 'updi'

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        self.assertTrue("Ping response: 1E9651" in self.mock_stdout.getvalue())

    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_ping_nedbg_pic16f18446(self, mock_read_tool_info, mock_housekeepingprotocol):
        # pylint: disable=unused-argument
        mock_read_tool_info.return_value = {'device_name': 'PIC16F18446'}

        mock_pic_model = self.create_pic_model_mock()

        # PIC16F18446 ID
        mock_pic_model.read_id.return_value = 0x30D4

        # The device support scripts will be mocked out so the packpath can be anything
        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'ping'
        arguments.tool = 'nedbg'
        arguments.device = 'PIC16F18446'
        arguments.interface = 'icsp'
        arguments.packpath = 'anypath'

        pymcuprog.pymcuprog(arguments)

        self.pic_session_check(mock_pic_model)

        self.assertTrue("Ping response: 30D4" in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_write_and_verify_literal_nedbg_mega4809(
            self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        data_in = [0x01, 0x02, 0x03, 0x04]
        write_offset = 4
        memname = MemoryNames.FLASH
        flash_pagesize = 128

        # The data actually written to the target should be a complete page with 0xFF padding for
        # bytes not to be written
        data_out = [0xFF]*flash_pagesize
        for index in range(write_offset, write_offset + len(data_in)):
            data_out[index] = data_in[index - write_offset]

        # Data sent to the lower layers will be converted to bytearray
        data_out = bytearray(data_out)

        # Prepare data for the memory read stub (pymcuprog will read data back since the verify option is enabled)
        self.stub_data[MemoryNames.FLASH] = data_out

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'write'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.literal = data_in
        arguments.memory = memname
        arguments.offset = write_offset
        arguments.verify = True

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        # Check that the correct data was written.  Note that start address should be aligned with page boundary
        mock_avr8protocol_obj.memory_write.assert_called_with(
            'AVR8_MEMTYPE_FLASH_PAGE', write_offset//flash_pagesize, data_out)

        # Check that the correct memory was read during the verification
        mock_avr8protocol_obj.memory_read.assert_called_with(
            'AVR8_MEMTYPE_FLASH_PAGE', write_offset//flash_pagesize, len(data_in)+write_offset)

        # Make sure erase was not done (should only be done if erase option was specified)
        mock_avr8protocol_obj.erase.assert_not_called()

        self.assertTrue("Writing literal values..." in self.mock_stdout.getvalue())
        self.assertTrue("Verifying literal values..." in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_read_literal_nedbg_mega4809(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        read_offset = 4
        numbytes = 8
        memname = MemoryNames.FLASH
        flash_pagesize = 128

        data_out = []
        for index in range(flash_pagesize):
            data_out.append(index & 0xFF)

        # Data returned from the lower layers will be in the format of a bytearray
        self.stub_data[MemoryNames.FLASH] = bytearray(data_out)

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'read'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.memory = memname
        arguments.offset = read_offset
        arguments.bytes = numbytes

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        # pymcuprog should read from start of a flash page,
        # so it will have to read the bytes up to the offset and then the number of bytes requested
        mock_avr8protocol_obj.memory_read.assert_called_with(
            'AVR8_MEMTYPE_FLASH_PAGE', read_offset//flash_pagesize, numbytes+read_offset%flash_pagesize)

        # Pymcuprog should always print complete lines of 16 data bytes.
        # The bytes not being requested should be shown as XX
        printout_data = 'xx '*read_offset
        for index in range(read_offset, read_offset+numbytes):
            printout_data += "{:02X} ".format(data_out[index])
        printout_data += 'xx '*(16-read_offset-numbytes)
        self.assertTrue(printout_data in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_write_hex_nedbg_mega4809(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        # Writing several pages of Flash and EEPROM to be able to
        # check skipping of blank pages for Flash but not for EEPROM.
        datasizes = {
            MemoryNames.FLASH: PAGE_SIZES_AVR[MemoryNames.FLASH]*3,
            MemoryNames.EEPROM: PAGE_SIZES_AVR[MemoryNames.EEPROM]*3,
            MemoryNames.FUSES: 1, MemoryNames.LOCKBITS: 1,
            MemoryNames.USER_ROW: 4}

        # Setting offset for Flash and EEPROM to not be page boundary aligned to
        # check that pymcuprog aligns the write to page boundary for flash but not for EEPROM.
        # This will also result in writing only a few bytes in one of the pages for
        # FLASH to check that a complete page is written while for EEPROM it is allowed to write
        # less than a complete page
        write_offsets = {MemoryNames.FLASH: 4,
                         MemoryNames.EEPROM: 2,
                         MemoryNames.FUSES: 0,
                         MemoryNames.LOCKBITS: 0,
                         MemoryNames.USER_ROW: 0}

        hexfile = self.generate_intelhex_with_dummy_data(HEX_OFFSETS_AVR, datasizes, write_offsets)

        # To be able to check that blank pages (containing only 0xFF values) are
        # skipped for flash and not EEPROM we blank the second page for both memories
        for memname in [MemoryNames.FLASH, MemoryNames.EEPROM]:
            start_address = HEX_OFFSETS_AVR[memname] + PAGE_SIZES_AVR[memname]
            for dataindex in range(start_address, start_address + PAGE_SIZES_AVR[memname]):
                hexfile[dataindex] = 0xFF

        # Turn the hexfile object into an actual file
        temp_filename = os.path.realpath(
            os.path.normpath("{}//temp_memory_write_updi.hex".format(TESTFILE_FOLDER)))
        hexfile.write_hex_file(temp_filename)

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'write'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.filename = temp_filename

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        # Check that the correct data was written.
        for memname in HEX_OFFSETS_AVR:
            writemem_calls = []
            expected_data = []

            hex_index = HEX_OFFSETS_AVR[memname] + write_offsets[memname]
            hex_data = hexfile.tobinarray(start=hex_index, size=datasizes[memname])
            if memname == MemoryNames.FLASH:
                expected_data, start_address = self._pad_and_align(hex_data,
                                                                   write_offsets[memname],
                                                                   PAGE_SIZES_AVR[memname])
            else:
                expected_data = hex_data
                start_address = write_offsets[memname]

            # For Flash the memory offset will be added by the tool,
            # for the other memories the offset should be added by pymcuprog
            if memname != MemoryNames.FLASH:
                start_address += MEM_OFFSETS_AVR[memname]

            while expected_data:
                current_chunk_bytes = PAGE_SIZES_AVR[memname] - start_address % PAGE_SIZES_AVR[memname]
                remaining_bytes = len(expected_data)
                if current_chunk_bytes > remaining_bytes:
                    current_chunk_bytes = remaining_bytes
                current_chunk = expected_data[:current_chunk_bytes]

                # For flash blank pages (only containing 0xFF) should be skipped
                if memname != MemoryNames.FLASH or not self._is_blank(current_chunk):
                    current_chunk_array = array('B')
                    current_chunk_array.extend(current_chunk)
                    writemem_calls.append(call.memory_write(MEMTYPES_AVR_WRITE[memname],
                                                            start_address,
                                                            current_chunk_array))

                expected_data = expected_data[current_chunk_bytes:]
                start_address += current_chunk_bytes

            mock_avr8protocol_obj.assert_has_calls(writemem_calls, any_order=False)

        # Make sure erase was not done (should only be done if erase option was specified)
        mock_avr8protocol_obj.erase.assert_not_called()

        for memname in HEX_OFFSETS_AVR:
            self.assertTrue("Writing {}...".format(memname) in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_write_verify_hex_nedbg_mega4809(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        datasizes = {
            MemoryNames.FLASH: PAGE_SIZES_AVR[MemoryNames.FLASH],
            MemoryNames.EEPROM: PAGE_SIZES_AVR[MemoryNames.EEPROM],
            MemoryNames.FUSES: 1,
            MemoryNames.LOCKBITS: 1,
            MemoryNames.USER_ROW: PAGE_SIZES_AVR[MemoryNames.USER_ROW]}

        hexfile = self.generate_intelhex_with_dummy_data(HEX_OFFSETS_AVR, datasizes, {})

        # Turn the hexfile object into an actual file
        temp_filename = os.path.realpath(
            os.path.normpath("{}//temp_memory_write_verify_updi.hex".format(TESTFILE_FOLDER)))
        hexfile.write_hex_file(temp_filename)

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'write'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.filename = temp_filename
        arguments.verify = True

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        # Check that the correct data was written.
        for memname in HEX_OFFSETS_AVR:
            current_page = [0xFF]*PAGE_SIZES_AVR[memname]
            for page_offset in range(PAGE_SIZES_AVR[memname]):
                current_page[page_offset] = hexfile[page_offset + HEX_OFFSETS_AVR[memname]]
            current_page_array = array('B')
            current_page_array.fromlist(current_page)
            start_address = 0
            # For Flash the memory offset will be added by the tool,
            # for the other memories the offset should be added by pymcuprog
            if memname != MemoryNames.FLASH:
                start_address += MEM_OFFSETS_AVR[memname]
            mock_avr8protocol_obj.assert_has_calls(
                [call.memory_write(MEMTYPES_AVR_WRITE[memname], start_address, current_page_array)],
                any_order=False)

        # Check that the correct memories were read
        for memname in HEX_OFFSETS_AVR:
             # For Flash the memory offset will be added by the tool,
             # for the other memories the offset should be added by pymcuprog
            start_address = 0
            if memname != MemoryNames.FLASH:
                start_address = MEM_OFFSETS_AVR[memname]
            mock_avr8protocol_obj.assert_has_calls(
                [call.memory_read(MEMTYPES_AVR_READ[memname], start_address, datasizes[memname])])

        for memname in HEX_OFFSETS_AVR:
            self.assertTrue("Writing {}...".format(memname) in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_read_flash_to_hex_nedbg_mega4809(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        flash_pagesize = 128

        # Flash reads should always be page aligned so we give a non-aligned offset to
        # check that pymcuprog handles this properly
        read_offset = 8

        # The read in this test spans data in two flash pages so we prepare two pages for the read stub
        data_out = [0xFF]*(flash_pagesize*2)
        for index in range(flash_pagesize):
            data_out[index + read_offset] = index & 0xFF

        # Data returned from the lower layers will be in the format of a bytearray
        self.stub_data[MemoryNames.FLASH] = bytearray(data_out)

        # Use a temporary file to store the output
        temp_filename = os.path.realpath(os.path.normpath("{}//temp_memory_read.hex".format(TESTFILE_FOLDER)))

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'read'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.memory = MemoryNames.FLASH
        arguments.filename = temp_filename
        arguments.offset = read_offset
        arguments.bytes = flash_pagesize

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        generated_hex = IntelHex()
        generated_hex.fromfile(temp_filename, format='hex')

        self.assertEqual(
            len(generated_hex.segments()),
            1,
            msg="There should only be one segment in the generated hexfile")

        flash_segment = generated_hex.segments()[0]

        hex_data = generated_hex.tobinarray(start=flash_segment[0], end=flash_segment[1]-1)

        for index, datavalue in enumerate(hex_data):
            self.assertEqual(datavalue, index & 0xFF, msg="Unexpected data at index: {}".format(index))

        # Flash reads should be aligned to page boundaries so pymcuprog should read the
        # requested data + the bytes from the page start up to the read offset
        mock_avr8protocol_obj.memory_read.assert_called_with('AVR8_MEMTYPE_FLASH_PAGE', 0, flash_pagesize+read_offset)

        self.assertTrue("Data written to hex file: '{}'".format(temp_filename) in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_write_and_verify_bin_to_flash_nedbg_mega4809(
            self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        data_in = [0xAA, 0xBB, 0xCC, 0xDD]
        write_offset = 2
        flash_pagesize = 128

        # Prepare data for the memory read stub (pymcuprog will read data back since the verify option is enabled)
        # Pymcuprog should always read from the start of a flash page so we have to pad the data to match the flash page
        verify_data = [0xFF]*(write_offset)
        verify_data.extend(data_in)
        self.stub_data[MemoryNames.FLASH] = bytearray(verify_data)

        # Use a temporary file to store the data to be written
        temp_filename = os.path.realpath(os.path.normpath("{}//temp_memory_write.bin".format(TESTFILE_FOLDER)))

        with open(temp_filename, "wb") as binfile:
            binfile.write(bytearray(data_in))

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'write'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.memory = MemoryNames.FLASH
        arguments.filename = temp_filename
        arguments.offset = write_offset
        arguments.verify = True

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        # Check that the correct data was written (a complete page including the data from the bin file)
        data_expected = [0xFF]*flash_pagesize
        for index, datavalue in enumerate(data_in):
            data_expected[write_offset+index] = datavalue

        mock_avr8protocol_obj.memory_write.assert_called_with('AVR8_MEMTYPE_FLASH_PAGE', 0, bytearray(data_expected))

        # Check that the correct memory was read during the verification
        mock_avr8protocol_obj.memory_read.assert_called_with(
            'AVR8_MEMTYPE_FLASH_PAGE', write_offset//flash_pagesize, len(data_in)+write_offset)

        # Make sure erase was not done (should only be done if erase option was specified)
        mock_avr8protocol_obj.erase.assert_not_called()

        self.assertTrue("Writing from binary file..." in self.mock_stdout.getvalue())
        self.assertTrue("Verifying from binary file..." in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_read_flash_to_bin_nedbg_mega4809(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        numbytes = 8

        # Flash reads should always be page aligned so we give a non-aligned offset to
        # check that pymcuprog handles this properly
        read_offset = 4

        # Prepare some data for the read memory stub
        data_out = [0xFF]*(numbytes+read_offset)
        for index in range(numbytes):
            data_out[index + read_offset] = index & 0xFF

        # Data returned from the lower layers will be in the format of a bytearray
        data_out = bytearray(data_out)

        self.stub_data[MemoryNames.FLASH] = data_out

        # Use a temporary file to store the output
        temp_filename = os.path.realpath(os.path.normpath("{}//temp_memory_read.bin".format(TESTFILE_FOLDER)))

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'read'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.memory = MemoryNames.FLASH
        arguments.filename = temp_filename
        arguments.offset = read_offset
        arguments.bytes = numbytes

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        # Add memory name to file name as pymcuprog generates one bin file for each
        # memory type each with the memory name included in the file name
        filenamelist = temp_filename.split('.')
        actual_filename = "{}_flash.{}".format(filenamelist[0], filenamelist[1])

        with open(actual_filename, "rb") as binfile:
            data_read = bytearray(binfile.read())

        self.assertEqual(data_read, data_out[read_offset:])

        # Flash reads should be aligned to page boundaries so pymcuprog should read the
        # requested data + the bytes from the page start up to the read offset
        mock_avr8protocol_obj.memory_read.assert_called_with('AVR8_MEMTYPE_FLASH_PAGE', 0, numbytes+read_offset)

        self.assertTrue("Data written to binary file: '{}'".format(actual_filename) in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_erase_single_memories_nedbg_mega4809(self, mock_read_tool_info,
                                                  mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        erase_modes = {
            MemoryNames.EEPROM: Avr8Protocol.ERASE_EEPROM,
            MemoryNames.USER_ROW: Avr8Protocol.ERASE_USERSIG}

        for memname in erase_modes:
            arguments = copy(DEFAULT_ARGS)
            arguments.action = 'erase'
            arguments.tool = 'nedbg'
            arguments.device = 'atmega4809'
            arguments.interface = 'updi'
            arguments.memory = memname

            pymcuprog.pymcuprog(arguments)

            self.updi_session_and_configuration_check(mock_avr8protocol_obj)

            mock_avr8protocol_obj.erase.assert_called_with(erase_modes[memname], 0)

            self.assertTrue("Erasing {}...".format(memname) in self.mock_stdout.getvalue())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_default_erase_nedbg_mega4809(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_avr8protocol_obj.memory_read.side_effect = self.updi_memory_read_stub
        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'erase'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        mock_avr8protocol_obj.erase.assert_called_with(Avr8Protocol.ERASE_CHIP, 0)

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_timer_option(self, mock_read_tool_info, mock_housekeepingprotocol, mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        mock_avr8protocol_obj.memory_read.side_effect = self.delayed_memory_read_stub

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'read'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.memory = MemoryNames.FLASH
        arguments.bytes = 2
        arguments.timing = True

        pymcuprog.pymcuprog(arguments)

        self.updi_session_and_configuration_check(mock_avr8protocol_obj)

        # Since the delayed_memory_read_stub puts in a 0.1s delay the operation should take at least 0.1 seconds.
        # However this test will only pass if the system uses less than 0.1s on the rest of the test,
        # but normally the test takes "no" time so without the delay the operation is reported to take 0.000s
        self.assertTrue("Operation took 0.1" in self.mock_stdout.getvalue())

    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_write_hex_nedbg_pic16f18446(self, mock_read_tool_info, mock_housekeepingprotocol):
        # pylint: disable=unused-argument
        mock_read_tool_info.return_value = {'device_name': 'PIC16F18446'}

        mock_pic_model = self.create_pic_model_mock()

        # PIC16F18446 ID
        mock_pic_model.read_id.return_value = 0x30D4

        datasizes = {
            MemoryNames.FLASH: PAGE_SIZES_PIC[MemoryNames.FLASH]*3,
            MemoryNames.EEPROM: 8,
            MemoryNames.ICD: PAGE_SIZES_PIC[MemoryNames.ICD],
            MemoryNames.CONFIG_WORD: PAGE_SIZES_PIC[MemoryNames.CONFIG_WORD],
            MemoryNames.USER_ID: PAGE_SIZES_PIC[MemoryNames.USER_ID]}

        # Setting offsets for EEPROM and Flash to not be page boundary aligned to
        # check that pymcuprog aligns the writes to page boundaries.
        write_offsets = {MemoryNames.FLASH: 4,
                         MemoryNames.EEPROM: 2,
                         MemoryNames.ICD: 0,
                         MemoryNames.CONFIG_WORD: 0,
                         MemoryNames.USER_ID: 0}

        hexfile = self.generate_intelhex_with_dummy_data(HEX_OFFSETS_PIC, datasizes, write_offsets)

        # Turn the hexfile object into an actual file
        temp_filename = os.path.realpath(
            os.path.normpath("{}//temp_memory_write_pic.hex".format(TESTFILE_FOLDER)))
        hexfile.write_hex_file(temp_filename)

        # The device support scripts will be mocked out so the packpath can be anything
        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'write'
        arguments.tool = 'nedbg'
        arguments.device = 'PIC16F18446'
        arguments.interface = 'icsp'
        arguments.packpath = 'anypath'
        arguments.filename = temp_filename

        pymcuprog.pymcuprog(arguments)

        self.pic_session_check(mock_pic_model)

        # Check that the correct data was written.
        writemem_calls = []
        for memname in HEX_OFFSETS_PIC:
            # Memory writes should do page aligned writes so we should expect some padding
            current_mem_data = [0xFF]*(datasizes[memname] + write_offsets[memname] % PAGE_SIZES_PIC[memname])
            for data_index in range(datasizes[memname]):
                hexfile_index = data_index + HEX_OFFSETS_PIC[memname] + write_offsets[memname]
                current_data_index = write_offsets[memname] % PAGE_SIZES_PIC[memname] + data_index
                current_mem_data[current_data_index] = hexfile[hexfile_index]

            if memname == MemoryNames.EEPROM:
                # PIC16 devices have one phantom byte per EEPROM byte in the hex file.
                # These phantom bytes should be removed before sending the data to the device model
                data_stripped = []
                for index in range(0, len(current_mem_data), 2):
                    data_stripped.append(current_mem_data[index])
                current_mem_data = data_stripped

            current_mem_data_array = array('B')
            current_mem_data_array.fromlist(current_mem_data)

            # Memory writes should match page boundaries
            write_offset_page_aligned = (write_offsets[memname] // PAGE_SIZES_PIC[memname]) * PAGE_SIZES_PIC[memname]
            write_start_address = MEM_OFFSETS_PIC[memname] + write_offset_page_aligned

            if memname == MemoryNames.FLASH:
                writemem_calls.append(call.write_flash_memory(write_start_address, current_mem_data_array))
            elif memname == MemoryNames.EEPROM:
                writemem_calls.append(call.write_eeprom_memory(write_start_address, current_mem_data_array))
            elif memname == MemoryNames.ICD:
                writemem_calls.append(call.write_de_memory(write_start_address, current_mem_data_array))
            elif memname == MemoryNames.CONFIG_WORD:
                writemem_calls.append(call.write_config_memory(write_start_address, current_mem_data_array))
            elif memname == MemoryNames.USER_ID:
                writemem_calls.append(call.write_user_id_memory(write_start_address, current_mem_data_array))

        mock_pic_model.assert_has_calls(writemem_calls, any_order=True)

        # Make sure erase was not done (should only be done if erase option was specified)
        mock_pic_model.erase.assert_not_called()
        mock_pic_model.erase_de_memory.assert_not_called()

        for memname in HEX_OFFSETS_PIC:
            self.assertTrue("Writing {}...".format(memname) in self.mock_stdout.getvalue())

    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_erase_single_memories_nedbg_pic16f18446(self, mock_read_tool_info, mock_housekeepingprotocol):
        # pylint: disable=unused-argument
        mock_read_tool_info.return_value = {'device_name': 'PIC16F18446'}

        mock_pic_model = self.create_pic_model_mock()

        # PIC16F18446 ID
        mock_pic_model.read_id.return_value = 0x30D4

        erase_addresses = {
            MemoryNames.FLASH: 0x80FE*2,
            MemoryNames.ICD: 0x8600*2}

        for memname in erase_addresses:
            # The device support scripts will be mocked out so the packpath can be anything
            arguments = copy(DEFAULT_ARGS)
            arguments.action = 'erase'
            arguments.tool = 'nedbg'
            arguments.device = 'PIC16F18446'
            arguments.interface = 'icsp'
            arguments.packpath = 'anypath'
            arguments.memory = memname

            pymcuprog.pymcuprog(arguments)

            self.pic_session_check(mock_pic_model)

            if memname == MemoryNames.ICD:
                # For ICD memory the number of bytes to be erased must be given.
                # The ICD memory has a size of 1024 bytes
                mock_pic_model.erase_de_memory.assert_called_with(erase_addresses[memname], 1024)
            else:
                mock_pic_model.erase.assert_called_with(erase_addresses[memname])

        for memname in erase_addresses:
            self.assertTrue("Erasing {}...".format(memname) in self.mock_stdout.getvalue())

    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_default_erase_nedbg_pic16f18446(self, mock_read_tool_info, mock_housekeepingprotocol):
        # pylint: disable=unused-argument
        mock_read_tool_info.return_value = {'device_name': 'PIC16F18446'}

        default_bulk_erase_address = 0x8000 * 2

        mock_pic_model = self.create_pic_model_mock()

        # PIC16F18446 ID
        mock_pic_model.read_id.return_value = 0x30D4

        # The device support scripts will be mocked out so the packpath can be anything
        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'erase'
        arguments.tool = 'nedbg'
        arguments.device = 'PIC16F18446'
        arguments.interface = 'icsp'
        arguments.packpath = 'anypath'

        pymcuprog.pymcuprog(arguments)

        self.pic_session_check(mock_pic_model)

        mock_pic_model.erase.assert_called_with(default_bulk_erase_address)

    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_erase_eeprom_nedbg_pic16f18446_is_refused(self, mock_read_tool_info, mock_housekeepingprotocol):
        # pylint: disable=unused-argument
        mock_read_tool_info.return_value = {'device_name': 'PIC16F18446'}

        mock_pic_model = self.create_pic_model_mock()

        # PIC16F18446 ID
        mock_pic_model.read_id.return_value = 0x30D4

        # The device support scripts will be mocked out so the packpath can be anything
        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'erase'
        arguments.tool = 'nedbg'
        arguments.device = 'PIC16F18446'
        arguments.interface = 'icsp'
        arguments.packpath = 'anypath'
        arguments.memory = MemoryNames.EEPROM

        pymcuprog.pymcuprog(arguments)

        self.pic_session_check(mock_pic_model)

        mock_pic_model.erase.assert_not_called()

        self.assertFalse("erasing eeprom" in self.mock_stdout.getvalue().lower())

    @patch('pymcuprog.avr8target.Avr8Protocol')
    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_write_out_of_range_mega4809_raises_valueerror(self,
                                                           mock_read_tool_info,
                                                           mock_housekeepingprotocol,
                                                           mock_avr8protocol):
        # pylint: disable=unused-argument
        self.avr8protocol_mock_configure(mock_avr8protocol)

        mock_avr8protocol_obj = mock_avr8protocol.return_value

        mock_read_tool_info.return_value = {'device_name': 'atmega4809'}

        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'write'
        arguments.tool = 'nedbg'
        arguments.device = 'atmega4809'
        arguments.interface = 'updi'
        arguments.literal = [0x11, 0x22]
        arguments.memory = MemoryNames.FLASH
        arguments.offset = 0xC001

        with self.assertRaises(ValueError):
            pymcuprog.pymcuprog(arguments)

        mock_avr8protocol_obj.memory_write.assert_not_called()

    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_read_all_to_hex_nedbg_pic16f18446(self, mock_read_tool_info, mock_housekeepingprotocol):
        # pylint: disable=unused-argument
        mock_read_tool_info.return_value = {'device_name': 'PIC16F18446'}

        mock_pic_model = self.create_pic_model_mock()

        # PIC16F18446 ID
        mock_pic_model.read_id.return_value = 0x30D4

        mock_pic_model.read_flash_memory.side_effect = self.pic16f18446_read_flash_memory_stub

        datasizes = {
            MemoryNames.FLASH: 16384*2,
            MemoryNames.EEPROM: 256,
            MemoryNames.ICD: 512*2,
            MemoryNames.CONFIG_WORD: 5*2,
            MemoryNames.USER_ID: 4*2}

        config_word_return_value = MEM_READ_VALUES_PIC[MemoryNames.CONFIG_WORD]
        mock_pic_model.read_config_memory.return_value = [config_word_return_value]*datasizes[MemoryNames.CONFIG_WORD]
        eeprom_return_value = MEM_READ_VALUES_PIC[MemoryNames.EEPROM]
        mock_pic_model.read_eeprom_memory.return_value = [eeprom_return_value]*datasizes[MemoryNames.EEPROM]

        temp_filename = os.path.realpath(
            os.path.normpath("{}//temp_all_memory_read_pic.hex".format(TESTFILE_FOLDER)))


        # The device support scripts will be mocked out so the packpath can be anything
        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'read'
        arguments.tool = 'nedbg'
        arguments.device = 'PIC16F18446'
        arguments.interface = 'icsp'
        arguments.packpath = 'anypath'
        arguments.memory = 'all'
        arguments.filename = temp_filename

        pymcuprog.pymcuprog(arguments)

        self.pic_session_check(mock_pic_model)

        # Check that the hex contains the correct data
        generated_hex = IntelHex()
        generated_hex.fromfile(temp_filename, format='hex')

        for segment in generated_hex.segments():
            segment_start = segment[0]
            segment_stop = segment[1]
            memname = self.find_memoryname_pic(segment_start)
            self.assertIsNotNone(memname, msg="Could not identify memory starting at address {}".format(segment_start))

            hex_data = generated_hex.tobinarray(start=segment_start, end=segment_stop-1)

            if memname == MemoryNames.EEPROM:
                # EEPROM memory should contain one extra phantom byte per EEPROM byte so number of bytes should be twice
                # the data read from the device
                self.assertEqual(len(hex_data),
                                 datasizes[memname]*2,
                                 msg="Unexpected length of {} data in hex".format(memname))
            else:
                self.assertEqual(len(hex_data),
                                 datasizes[memname],
                                 msg="Unexpected length of {} data in hex".format(memname))

            expected_value = MEM_READ_VALUES_PIC[memname]

            if memname == MemoryNames.EEPROM:
                # EEPROM memory should contain one extra phantom byte per EEPROM byte
                index = 0
                for databyte in hex_data:
                    if index % 2:
                        # Odd bytes are phantom bytes
                        self.assertEqual(0x00, databyte, "EEPROM phantom byte mismatch @index {}".format(index))
                    else:
                        self.assertEqual(expected_value, databyte, "{} data mismatch in hex".format(memname))
                    index += 1
            else:
                for databyte in hex_data:
                    self.assertEqual(expected_value, databyte, "{} data mismatch in hex".format(memname))

        # Check that the correct memories were read
        mock_pic_model.read_eeprom_memory.assert_called_with(
            MEM_OFFSETS_PIC[MemoryNames.EEPROM], datasizes[MemoryNames.EEPROM])
        mock_pic_model.read_config_memory.assert_called_with(
            MEM_OFFSETS_PIC[MemoryNames.CONFIG_WORD], datasizes[MemoryNames.CONFIG_WORD])
        mock_pic_model.assert_has_calls([
            call.read_flash_memory(MEM_OFFSETS_PIC[MemoryNames.FLASH], datasizes[MemoryNames.FLASH]),
            call.read_flash_memory(MEM_OFFSETS_PIC[MemoryNames.USER_ID], datasizes[MemoryNames.USER_ID]),
            call.read_flash_memory(MEM_OFFSETS_PIC[MemoryNames.ICD], datasizes[MemoryNames.ICD])], any_order=True)

        for memname in MEM_OFFSETS_PIC:
            self.assertTrue("Reading..." in self.mock_stdout.getvalue())

        self.assertTrue("Data written to hex file: '{}'".format(temp_filename) in self.mock_stdout.getvalue())

    @patch('pymcuprog.backend.housekeepingprotocol')
    @patch('pymcuprog.backend.read_tool_info')
    def test_read_all_to_bin_nedbg_pic16f18446(self, mock_read_tool_info, mock_housekeepingprotocol):
        # pylint: disable=unused-argument
        mock_read_tool_info.return_value = {'device_name': 'PIC16F18446'}

        mock_pic_model = self.create_pic_model_mock()

        # PIC16F18446 ID
        mock_pic_model.read_id.return_value = 0x30D4

        mock_pic_model.read_flash_memory.side_effect = self.pic16f18446_read_flash_memory_stub

        datasizes = {
            MemoryNames.FLASH: 16384*2,
            MemoryNames.EEPROM: 256,
            MemoryNames.ICD: 512*2,
            MemoryNames.CONFIG_WORD: 5*2,
            MemoryNames.USER_ID: 4*2}

        mock_pic_model.read_config_memory.return_value = bytearray(
            [MEM_READ_VALUES_PIC[MemoryNames.CONFIG_WORD]]*datasizes[MemoryNames.CONFIG_WORD])
        mock_pic_model.read_eeprom_memory.return_value = bytearray(
            [MEM_READ_VALUES_PIC[MemoryNames.EEPROM]]*datasizes[MemoryNames.EEPROM])

        temp_filename = os.path.realpath(
            os.path.normpath("{}//temp_all_memory_read_pic.bin".format(TESTFILE_FOLDER)))

        # The device support scripts will be mocked out so the packpath can be anything
        arguments = copy(DEFAULT_ARGS)
        arguments.action = 'read'
        arguments.tool = 'nedbg'
        arguments.device = 'PIC16F18446'
        arguments.interface = 'icsp'
        arguments.packpath = 'anypath'
        arguments.memory = 'all'
        arguments.filename = temp_filename

        pymcuprog.pymcuprog(arguments)

        self.pic_session_check(mock_pic_model)

        for memname in MEM_OFFSETS_PIC:
            memory_filename = temp_filename.split('.')[0]
            memory_filename += "_{}".format(memname) + ".bin"
            with open(memory_filename, "rb") as binfile:
                data_read = bytearray(binfile.read())

            self.assertTrue("Data written to binary file: '{}'".format(memory_filename) in self.mock_stdout.getvalue())

            expected_value = MEM_READ_VALUES_PIC[memname]

            for databyte in data_read:
                self.assertEqual(expected_value, databyte, msg="Incorrect data value for {} memory".format(memname))

        # Check that the correct memories were read
        mock_pic_model.read_eeprom_memory.assert_called_with(
            MEM_OFFSETS_PIC[MemoryNames.EEPROM], datasizes[MemoryNames.EEPROM])
        mock_pic_model.read_config_memory.assert_called_with(
            MEM_OFFSETS_PIC[MemoryNames.CONFIG_WORD], datasizes[MemoryNames.CONFIG_WORD])
        mock_pic_model.assert_has_calls([
            call.read_flash_memory(MEM_OFFSETS_PIC[MemoryNames.FLASH], datasizes[MemoryNames.FLASH]),
            call.read_flash_memory(MEM_OFFSETS_PIC[MemoryNames.USER_ID], datasizes[MemoryNames.USER_ID]),
            call.read_flash_memory(MEM_OFFSETS_PIC[MemoryNames.ICD], datasizes[MemoryNames.ICD])], any_order=True)

        for memname in MEM_OFFSETS_PIC:
            self.assertTrue("Reading..." in self.mock_stdout.getvalue())
            # Add memory name to file name before checking printout
            filenamelist = temp_filename.split('.')
            actual_filename = "{}_{}.{}".format(filenamelist[0], memname, filenamelist[1])
            self.assertTrue("Data written to binary file: '{}'".format(actual_filename) in self.mock_stdout.getvalue())

    def test_print_version(self):
        arguments = copy(DEFAULT_ARGS)
        arguments.version = True

        pymcuprog.pymcuprog(arguments)

        self.assertTrue("pymcuprog version" in self.mock_stdout.getvalue())
