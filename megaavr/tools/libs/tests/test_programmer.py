# The intention is to make the test names descriptive enough to not need any docstrings for most of them
#pylint: disable=missing-docstring
# It seems better to have all tests for one module in the same file than to split across multiple files,
# so accepting many public methods and many lines makes sense
#pylint: disable=too-many-lines
#pylint: disable=too-many-public-methods
# Some tests require a lot of configurable parameters so there will be many local variables
#pylint: disable=too-many-locals
import unittest
from mock import MagicMock
from mock import patch
from mock import call

from pymcuprog.programmer import Programmer
from pymcuprog.deviceinfo.memorynames import MemoryNames, MemoryNameAliases
from pymcuprog.deviceinfo import deviceinfo
from pymcuprog.deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys
from pymcuprog.pymcuprog_errors import PymcuprogNotSupportedError

class TestProgrammer(unittest.TestCase):
    def setUp(self):
        self.programmer = Programmer(None)

    def _mock_nvmaccessprovider(self):
        """
        Mock out the nvmaccessprovider instance returned by get_nvm_access_provider in pymcuprog.programmer

        :returns: nvmaccessprovider mock
        """
        mock_get_nvm_access_provider_patch = patch("pymcuprog.programmer.get_nvm_access_provider")
        self.addCleanup(mock_get_nvm_access_provider_patch.stop)
        mock_get_nvm_access_provider = mock_get_nvm_access_provider_patch.start()
        mock_nvmaccessprovider = MagicMock()
        mock_get_nvm_access_provider.return_value = mock_nvmaccessprovider

        return mock_nvmaccessprovider

    def _mock_devicememoryinfo(self):
        """
        Mock the DeviceMemoryInfo instance of pymcuprog.programmer

        :returns: DeviceMemoryInfo instance mock
        """
        mock_deviceinfo_patch = patch("pymcuprog.programmer.deviceinfo")
        self.addCleanup(mock_deviceinfo_patch.stop)
        mock_deviceinfo = mock_deviceinfo_patch.start()
        mock_devicememoryinfo = MagicMock()
        mock_deviceinfo.DeviceMemoryInfo.return_value = mock_devicememoryinfo

        return mock_devicememoryinfo

    @staticmethod
    def _generate_dummydata(numbytes):
        """
        Generate dummy data to be used for read and write tests.

        Generated data will be an 8-bit counter
        """
        data = bytearray([])
        for count in range(numbytes):
            data.append(count & 0xFF)

        return data

    @staticmethod
    def _read_memory_stub(memory, offset, numbytes):
        """
        Stub to use when mocking a device model read_memory function.

        The stub will return data according to the numbytes parameter
        """
        #pylint: disable=unused-argument
        return TestProgrammer._generate_dummydata(numbytes)

    @staticmethod
    def _get_memory_info(device, memory_name):
        device_info = deviceinfo.getdeviceinfo(device)
        device_meminfo = deviceinfo.DeviceMemoryInfo(device_info)
        return device_meminfo.memory_info_by_name(memory_name)

    def test_erase_propagates_all_parameters(self):
        memoryname = 'some_memory'
        address = 256

        mock_devicememoryinfo = self._mock_devicememoryinfo()

        mock_devicememoryinfo.memory_info_by_name.return_value = memoryname

        mock_nvmaccessprovider = self._mock_nvmaccessprovider()

        self.programmer.load_device('dummydevice')
        self.programmer.setup_device()
        self.programmer.erase(memoryname, address)

        mock_nvmaccessprovider.erase.assert_called_with(memory_info=memoryname, address=address)

    def test_erase_when_memory_is_all_calls_erase_with_memory_info_and_address_as_none(self):
        """
        If memory_info is set to None when calling erase on the device models the default erase will
        be used.  This default erase will erase all memories (i.e. chip erase for AVR or bulk erase
        with default bulk erase address for PIC).
        """
        self._mock_devicememoryinfo()
        mock_nvmaccessprovider = self._mock_nvmaccessprovider()

        self.programmer.load_device('dummydevice')
        self.programmer.setup_device()
        self.programmer.erase(MemoryNameAliases.ALL, 0)

        mock_nvmaccessprovider.erase.assert_called_with(memory_info=None, address=None)

    def test_erase_when_memory_is_undefined_raises_valueerror(self):
        """
        If memory_info is set to None when calling erase on the device models the default erase will
        be used.  This default erase will erase all memories (i.e. chip erase for AVR or bulk erase
        with default bulk erase address for PIC).
        """
        self._mock_nvmaccessprovider()

        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.erase('undefined', 0)

    def test_read_flash(self):
        offset = 0x1000
        numbytes = 256
        memory_name = MemoryNames.FLASH
        # Just pick any device
        device = 'atmega4809'
        # Generate dummy data for the read to return
        data_expected = self._generate_dummydata(numbytes)

        mock_nvmaccessprovider = self._mock_nvmaccessprovider()
        mock_nvmaccessprovider.read.return_value = data_expected
        meminfo_expected = self._get_memory_info(device, memory_name)

        self.programmer.load_device(device)
        self.programmer.setup_device()
        mem_read_list = self.programmer.read_memory(memory_name=memory_name, offset=offset, numbytes=numbytes)

        data_read = mem_read_list[0].data
        meminfo_read = mem_read_list[0].memory_info

        self.assertEqual(data_read, data_expected)
        self.assertEqual(meminfo_read[DeviceMemoryInfoKeys.NAME], memory_name)
        self.assertEqual(meminfo_read, meminfo_expected)
        mock_nvmaccessprovider.read.assert_called_with(meminfo_expected, offset, numbytes)

    def test_read_when_offset_is_outside_memory_boundary_raises_valueerror(self):
        numbytes = 256
        # ATmega4809 has 0xC000 bytes of flash, make sure the memory read is one byte outside the memory range
        offset = 0xC000 - numbytes + 1
        memory_name = MemoryNames.FLASH

        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.read_memory(memory_name=memory_name, offset=offset, numbytes=numbytes)

    def test_read_undefined_memory_raises_valueerror(self):
        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.read_memory(memory_name='undefined', offset=0, numbytes=256)

    def test_read_negative_offset_raises_valueerror(self):
        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.read_memory(memory_name=MemoryNames.FLASH, offset=-10, numbytes=256)

    def test_read_negative_numbytes_raises_valuerror(self):
        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.read_memory(memory_name=MemoryNames.FLASH, offset=0, numbytes=-256)

    def test_read_numbytes_0_reads_complete_memory_starting_at_offset(self):
        # Pick any offset, but do not use 0 to check that the offset is taken into account
        offset = 0x1000
        numbytes = 0
        memory_name = MemoryNames.FLASH
        # Just pick any device
        device = 'atmega4809'

        memory_info_expected = self._get_memory_info(device, memory_name)

        # Generate dummy data for the read to return
        data_expected = self._generate_dummydata(memory_info_expected[DeviceMemoryInfoKeys.SIZE]-offset)

        mock_nvmaccessprovider = self._mock_nvmaccessprovider()
        mock_nvmaccessprovider.read.side_effect = self._read_memory_stub

        # Just pick any device
        self.programmer.load_device(device)
        self.programmer.setup_device()
        mem_read_list = self.programmer.read_memory(memory_name=memory_name, offset=offset, numbytes=numbytes)

        data_read = mem_read_list[0].data
        meminfo_read = mem_read_list[0].memory_info

        self.assertEqual(data_read, data_expected)
        self.assertEqual(meminfo_read[DeviceMemoryInfoKeys.NAME], memory_name)
        mock_nvmaccessprovider.read.assert_called_with(memory_info_expected,
                                                       offset,
                                                       memory_info_expected[DeviceMemoryInfoKeys.SIZE]-offset)

    def test_read_all_reads_all_memories(self):
        # Pick any offset, but do not use 0 to check that the offset is ignored
        offset = 0x1000
        # Pick any number of bytes to check that numbytes is ignored
        numbytes = 256
        # Just pick any device with more than one memory segment
        device = 'atmega4809'

        device_info = deviceinfo.getdeviceinfo(device)
        device_meminfo = deviceinfo.DeviceMemoryInfo(device_info)

        mock_nvmaccessprovider = self._mock_nvmaccessprovider()
        mock_nvmaccessprovider.read.side_effect = self._read_memory_stub

        self.programmer.load_device(device)
        self.programmer.setup_device()
        memory_read_list = self.programmer.read_memory(memory_name=MemoryNameAliases.ALL,
                                                       offset=offset,
                                                       numbytes=numbytes)

        memories = device_meminfo.mem_by_name.keys()
        read_calls = []
        for memory_name_expected in memories:
            meminfo_expected = device_meminfo.memory_info_by_name(memory_name_expected)

            found_memory = False
            for memory_read_tuple in memory_read_list:
                if memory_read_tuple.memory_info[DeviceMemoryInfoKeys.NAME] == memory_name_expected:
                    data_expected = self._generate_dummydata(meminfo_expected[DeviceMemoryInfoKeys.SIZE])
                    found_memory = True
                    data_read = memory_read_tuple.data
                    meminfo_read = memory_read_tuple.memory_info

                    self.assertEqual(data_read, data_expected)
                    self.assertEqual(meminfo_read, meminfo_expected)
                    read_calls.append(call.read(meminfo_expected, 0, meminfo_expected[DeviceMemoryInfoKeys.SIZE]))

            self.assertTrue(found_memory, msg="Did not find {} memory in returned data".format(memory_name_expected))

        mock_nvmaccessprovider.assert_has_calls(read_calls)

    def test_write_flash(self):
        offset = 0x1000
        numbytes = 256
        memory_name = MemoryNames.FLASH
        # Just pick any device
        device = 'atmega4809'
        # Generate dummy data for the write call
        data_expected = self._generate_dummydata(numbytes)

        mock_nvmaccessprovider = self._mock_nvmaccessprovider()

        meminfo_expected = self._get_memory_info(device, memory_name)

        self.programmer.load_device(device)
        self.programmer.setup_device()
        self.programmer.write_memory(data=data_expected, memory_name=memory_name, offset=offset)

        mock_nvmaccessprovider.write.assert_called_with(meminfo_expected, offset, data_expected)

    def test_write_when_write_is_unsupported_raises_pymcuprognotsupportederror(self):
        device = 'atsamd21e18a'
        memory_name = MemoryNames.CALIBRATION_ROW

        self._mock_nvmaccessprovider()
        self.programmer.load_device(device)
        self.programmer.setup_device()
        with self.assertRaises(PymcuprogNotSupportedError):
            self.programmer.write_memory(data=[], memory_name=memory_name, offset=0)

    def test_write_when_offset_is_outside_memory_boundary_raises_valueerror(self):
        numbytes = 256
        device = 'atmega4809'
        # ATmega4809 has 0xC000 bytes of flash, make sure the memory written is one byte outside the memory range
        offset = 0xC000 - numbytes + 1
        memory_name = MemoryNames.FLASH
        data = self._generate_dummydata(numbytes)

        self._mock_nvmaccessprovider()
        self.programmer.load_device(device)
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.write_memory(data=data, memory_name=memory_name, offset=offset)

    def test_write_undefined_memory_raises_valueerror(self):
        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.write_memory(data=bytearray([0x00, 0x01]), memory_name='undefined', offset=0)

    def test_write_negative_offset_raises_valueerror(self):
        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.write_memory(data=bytearray([0x00, 0x01]), memory_name=MemoryNames.FLASH, offset=-10)

    def test_verify_flash_success(self):
        offset = 0x1000
        numbytes = 256
        memory_name = MemoryNames.FLASH
        # Just pick any device
        device = 'atmega4809'
        # Generate dummy data for the verify call
        data_expected = self._generate_dummydata(numbytes)

        mock_nvmaccessprovider = self._mock_nvmaccessprovider()
        mock_nvmaccessprovider.read.return_value = data_expected
        meminfo_expected = self._get_memory_info(device, memory_name)

        self.programmer.load_device(device)
        self.programmer.setup_device()
        verify_result = self.programmer.verify_memory(data=data_expected, memory_name=memory_name, offset=offset)

        mock_nvmaccessprovider.read.assert_called_with(meminfo_expected, offset, numbytes)
        self.assertTrue(verify_result, msg="Verify did not return success")

    def test_verify_flash_failure(self):
        offset = 0x1000
        numbytes = 256
        memory_name = MemoryNames.FLASH
        # Just pick any device
        device = 'atmega4809'
        # Generate dummy data to be used as input for the verify call
        data_input = self._generate_dummydata(numbytes)
        # Generate different data to be used as output from the device model read
        data_read_output = bytearray([0x00]*numbytes)

        mock_nvmaccessprovider = self._mock_nvmaccessprovider()
        mock_nvmaccessprovider.read.return_value = data_read_output
        meminfo_expected = self._get_memory_info(device, memory_name)

        self.programmer.load_device(device)
        self.programmer.setup_device()
        verify_result = self.programmer.verify_memory(data=data_input, memory_name=memory_name, offset=offset)

        mock_nvmaccessprovider.read.assert_called_with(meminfo_expected, offset, numbytes)
        self.assertFalse(verify_result, msg="Verify did not return failure")

    def test_verify_when_offset_is_outside_memory_boundary_raises_valueerror(self):
        numbytes = 256
        device = 'atmega4809'
        # ATmega4809 has 0xC000 bytes of flash, make sure the memory written is one byte outside the memory range
        offset = 0xC000 - numbytes + 1
        memory_name = MemoryNames.FLASH
        data = self._generate_dummydata(numbytes)

        self._mock_nvmaccessprovider()
        self.programmer.load_device(device)
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.verify_memory(data=data, memory_name=memory_name, offset=offset)

    def test_verify_undefined_memory_raises_valueerror(self):
        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.verify_memory(data=bytearray([0x00, 0x01]), memory_name='undefined', offset=0)

    def test_verify_negative_offset_raises_valueerror(self):
        self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        with self.assertRaises(ValueError):
            self.programmer.verify_memory(data=bytearray([0x00, 0x01]), memory_name=MemoryNames.FLASH, offset=-10)

    def test_hold_in_reset_propagates(self):
        mock_nvmaccessprovider = self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        self.programmer.hold_in_reset()

        mock_nvmaccessprovider.hold_in_reset.assert_called()

    def test_release_from_reset_propagates(self):
        mock_nvmaccessprovider = self._mock_nvmaccessprovider()
        # Just pick any device
        self.programmer.load_device('atmega4809')
        self.programmer.setup_device()
        self.programmer.release_from_reset()

        mock_nvmaccessprovider.release_from_reset.assert_called()
