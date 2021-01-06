# The intention is to make the test names descriptive enough to not need any docstrings for most of them
#pylint: disable=missing-docstring
# Some tests require a lot of configurable parameters so there will be many local variables
#pylint: disable=too-many-locals
import unittest
import shutil
from collections import namedtuple
from intelhex import IntelHex

from pymcuprog.hexfileutils import write_memories_to_hex, write_memory_to_hex, read_memories_from_hex
from pymcuprog.hexfileutils import _write_hex_to_file
from pymcuprog.deviceinfo import deviceinfo
from pymcuprog.deviceinfo.memorynames import MemoryNames
from pymcuprog.deviceinfo.deviceinfokeys import DeviceMemoryInfoKeys
from pymcuprog.backend import Backend

TESTFILE_FOLDER = "pymcuprog//tests//temphexfiles//"

class TestHexfileutils(unittest.TestCase):
    def setUp(self):
        # Make sure no relics from previous tests mess up the current test
        try:
            shutil.rmtree(TESTFILE_FOLDER)
        except FileNotFoundError:
            # Folder is already gone
            pass

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
    def _generate_memorytuple(numbytes, memory_info):
        """
        Generate a namedtuple with two fields: data and memory_info

        :param numbytes: number of data bytes to generate (dummy data)
        :param memory_info: memory info dictionary as defined in pymcuprog.deviceinfo.deviceinfo
        :returns namedtuple with two fields: data and memory_info
        """
        mem_tuple = namedtuple('Memory', "data memory_info")
        mem_tuple.data = TestHexfileutils._generate_dummydata(numbytes)
        mem_tuple.memory_info = memory_info

        return mem_tuple

    def test_write_atmega4809_eeprom_uses_hexfile_address(self):
        filename = "{}eeprom.hex".format(TESTFILE_FOLDER)
        offset = 16
        numbytes = 8
        info = Backend.get_device_info('atmega4809')
        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        eeprom_info = device_memory_info.memory_info_by_name(MemoryNames.EEPROM)

        mem_tuple = self._generate_memorytuple(numbytes, eeprom_info)
        write_memory_to_hex(filename, mem_tuple, offset)

        hexfile = IntelHex()
        hexfile.fromfile(filename, format='hex')

        segments = hexfile.segments()

        segment = segments[0]
        start = segment[0]
        stop = segment[1]
        hexadr = eeprom_info[DeviceMemoryInfoKeys.HEXFILE_ADDRESS]
        self.assertEqual(start, hexadr+offset, msg="Unexpected start hex address")
        self.assertEqual(stop, hexadr+offset+numbytes, msg="Unexpected stop hex address")

    def test_write_and_read_flash_pic16f18446(self):
        """
        This test exercises the write_memory_to_hex function (single segment)
        """
        filename = "{}flash.hex".format(TESTFILE_FOLDER)
        offset = 16
        numbytes = 256
        info = Backend.get_device_info('pic16f18446')
        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        flash_info = device_memory_info.memory_info_by_name(MemoryNames.FLASH)

        mem_tuple = self._generate_memorytuple(numbytes, flash_info)

        write_memory_to_hex(filename, mem_tuple, offset=offset)
        segments_read = read_memories_from_hex(filename, device_memory_info)

        flash_read = segments_read[0]
        self.assertEqual(len(segments_read), 1, msg="There should be only one segment in the hex")
        self.assertEqual(flash_read.offset, offset, msg="Incorrect offset")
        self.assertEqual(flash_read.memory_info[DeviceMemoryInfoKeys.NAME],
                         MemoryNames.FLASH, msg="Incorrect memory name")
        self.assertEqual(flash_read.data, mem_tuple.data, msg="Incorrect data")

    def test_write_and_read_flash_eeprom_atmega4809(self):
        """
        This test exercises the write_memories_to_hex function (multiple segments)
        """
        filename = "{}flasheeprom.hex".format(TESTFILE_FOLDER)
        numbytes_flash = 256
        numbytes_eeprom = 8
        info = Backend.get_device_info('atmega4809')
        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        flash_info = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        eeprom_info = device_memory_info.memory_info_by_name(MemoryNames.EEPROM)

        mem_tuple_flash = self._generate_memorytuple(numbytes_flash, flash_info)
        mem_tuple_eeprom = self._generate_memorytuple(numbytes_eeprom, eeprom_info)
        memories = [mem_tuple_flash, mem_tuple_eeprom]

        write_memories_to_hex(filename, memories)
        segments_read = read_memories_from_hex(filename, device_memory_info)

        for segment_read in segments_read:
            memory_name = segment_read.memory_info[DeviceMemoryInfoKeys.NAME]
            # When writing multiple segments all will start from relative offset 0
            self.assertEqual(segment_read.offset, 0, msg="Incorrect offset for {}".format(memory_name))
            if memory_name == MemoryNames.FLASH:
                self.assertEqual(segment_read.data, mem_tuple_flash.data, msg="Incorrect FLASH data")
            elif memory_name == MemoryNames.EEPROM:
                self.assertEqual(segment_read.data, mem_tuple_eeprom.data, msg="Incorrect EEPROM data")
            else:
                self.fail("Unexpected memory: {}".format(memory_name))

    def test_write_and_read_flash_eeprom_pic16f18446(self):
        """
        This test exercises the write_memories_to_hex function (multiple segments)
        """
        filename = "{}flasheeprom.hex".format(TESTFILE_FOLDER)
        numbytes_flash = 256
        numbytes_eeprom = 8
        info = Backend.get_device_info('pic16f18446')
        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        flash_info = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        eeprom_info = device_memory_info.memory_info_by_name(MemoryNames.EEPROM)

        mem_tuple_flash = self._generate_memorytuple(numbytes_flash, flash_info)
        mem_tuple_eeprom = self._generate_memorytuple(numbytes_eeprom, eeprom_info)
        memories = [mem_tuple_flash, mem_tuple_eeprom]

        write_memories_to_hex(filename, memories)
        segments_read = read_memories_from_hex(filename, device_memory_info)

        for segment_read in segments_read:
            memory_name = segment_read.memory_info[DeviceMemoryInfoKeys.NAME]
            # When writing multiple segments all will start from relative offset 0
            self.assertEqual(segment_read.offset, 0, msg="Incorrect offset for {}".format(memory_name))
            if memory_name == MemoryNames.FLASH:
                self.assertEqual(segment_read.data, mem_tuple_flash.data, msg="Incorrect FLASH data")
            elif memory_name == MemoryNames.EEPROM:
                self.assertEqual(segment_read.data, mem_tuple_eeprom.data, msg="Incorrect EEPROM data")
            else:
                self.fail("Unexpected memory: {}".format(memory_name))

        # Sanity check to see that the phantom bytes actually were added by the write to hex
        hexfile = IntelHex()
        hexfile.fromfile(filename, format='hex')
        hexadr = eeprom_info[DeviceMemoryInfoKeys.HEXFILE_ADDRESS]
        data_read = hexfile.tobinarray(start=hexadr,
                                       end=hexadr + numbytes_eeprom*2 - 1)
        self.assertEqual(len(data_read),
                         numbytes_eeprom*2,
                         msg="EEPROM should include twice as many bytes as was written due to the phantom bytes")
        index = 0
        for index in range(numbytes_eeprom):
            self.assertEqual(data_read[index*2],
                             mem_tuple_eeprom.data[index],
                             msg="Incorrect EEROM data in written hex at index {}".format(index*2))
            self.assertEqual(data_read[index*2+1],
                             0x00,
                             msg="Incorrect phantom byte in written hex at index {}".format(index*2+1))

    def test_write_and_read_eeprom_pic16f18446(self):
        """
        This test exercises the write_memory_to_hex function (single segment)
        """
        filename = "{}eeprom.hex".format(TESTFILE_FOLDER)
        offset = 0
        info = Backend.get_device_info('pic16f18446')
        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        eeprom_info = device_memory_info.memory_info_by_name(MemoryNames.EEPROM)
        numbytes_eeprom = eeprom_info[DeviceMemoryInfoKeys.SIZE]

        mem_tuple_eeprom = self._generate_memorytuple(numbytes_eeprom, eeprom_info)

        write_memory_to_hex(filename, mem_tuple_eeprom, offset)
        segments_read = read_memories_from_hex(filename, device_memory_info)

        for segment_read in segments_read:
            memory_name = segment_read.memory_info[DeviceMemoryInfoKeys.NAME]
            # When writing multiple segments all will start from relative offset 0
            self.assertEqual(segment_read.offset, offset, msg="Incorrect offset for {}".format(memory_name))
            if memory_name == MemoryNames.EEPROM:
                self.assertEqual(segment_read.data, mem_tuple_eeprom.data, msg="Incorrect EEPROM data")
            else:
                self.fail("Unexpected memory: {}".format(memory_name))

        # Sanity check to see that the phantom bytes actually were added by the write to hex
        hexfile = IntelHex()
        hexfile.fromfile(filename, format='hex')
        hexadr = eeprom_info[DeviceMemoryInfoKeys.HEXFILE_ADDRESS]
        data_read = hexfile.tobinarray(start=hexadr,
                                       end=hexadr + numbytes_eeprom*2 - 1)
        self.assertEqual(len(data_read),
                         numbytes_eeprom*2,
                         msg="EEPROM should include twice as many bytes as was written due to the phantom bytes")
        index = 0
        for index in range(numbytes_eeprom):
            self.assertEqual(data_read[index*2],
                             mem_tuple_eeprom.data[index],
                             msg="Incorrect EEROM data in written hex at index {}".format(index*2))
            self.assertEqual(data_read[index*2+1],
                             0x00,
                             msg="Incorrect phantom byte in written hex at index {}".format(index*2+1))

    def test_read_when_file_does_not_exist_raises_filenotfounderror(self):
        with self.assertRaises(FileNotFoundError):
            read_memories_from_hex('nonexisting', {})

    def test_read_hex_with_several_segments_within_same_memory(self):
        # Just use a PIC device to avoid having to create fake device info
        devinfo = Backend.get_device_info('pic16f18446')
        device_memory_info = deviceinfo.DeviceMemoryInfo(devinfo)
        flashinfo = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        filename = "{}multi_flash.hex".format(TESTFILE_FOLDER)

        flash1_numbytes = 256
        flash1_offset = 0
        flash1_data = self._generate_dummydata(flash1_numbytes)

        flash2_numbytes = 512
        flash2_offset = 1024
        flash2_data = self._generate_dummydata(flash2_numbytes)

        # Make a hex file with the two flash segments
        ihex = IntelHex()
        hexindex = flash1_offset
        for databyte in flash1_data:
            ihex[hexindex] = databyte
            hexindex += 1
        hexindex = flash2_offset
        for databyte in flash2_data:
            ihex[hexindex] = databyte
            hexindex += 1
        _write_hex_to_file(ihex, filename)

        segments_read = read_memories_from_hex(filename, device_memory_info)

        self.assertEqual(len(segments_read), 2, msg="Too many segments read from the hex")
        for segment_read in segments_read:
            memory_name = segment_read.memory_info[DeviceMemoryInfoKeys.NAME]
            self.assertEqual(memory_name, flashinfo[DeviceMemoryInfoKeys.NAME])
            if segment_read.offset == flash1_offset:
                self.assertEqual(segment_read.data, flash1_data, "First flash segment data mistmatch")
            elif segment_read.offset == flash2_offset:
                self.assertEqual(segment_read.data, flash2_data, "Second flash segment data mistmatch")
            else:
                self.fail("Segment with invalid offset: {}".format(segment_read.offset))

    def test_write_and_read_several_memories_in_one_hex_segment(self):
        """
        This test hacks the memory information for PIC16F18446 to pretend flash and eeprom memories are next to each
        other so that it becomes possible to test writing and reading a hex with several memories in the same hex
        segment.
        """
        filename = "{}consecutive_memories.hex".format(TESTFILE_FOLDER)
        info = Backend.get_device_info('pic16f18446')

        # Hack info to make flash and eeprom segments smaller and next to each other
        flash_size_key = MemoryNames.FLASH + '_' + DeviceMemoryInfoKeys.SIZE
        flash_size_words_key = flash_size_key + '_words'
        flash_size_bytes_key = flash_size_key + '_bytes'
        if flash_size_words_key in info:
            flash_size_words_original = info[flash_size_words_key]
            info[flash_size_words_key] = 1024
        else:
            flash_size_bytes_original = info[flash_size_bytes_key]
            info[flash_size_bytes_key] = 1024*2
        eeprom_address_key = MemoryNames.EEPROM + '_' + DeviceMemoryInfoKeys.ADDRESS
        eeprom_address_word_key = eeprom_address_key + '_word'
        eeprom_address_byte_key = eeprom_address_key + '_byte'
        if eeprom_address_word_key in info:
            eeprom_address_word_original = info[eeprom_address_word_key]
            info[eeprom_address_word_key] = 1024
        else:
            eeprom_address_byte_original = info[eeprom_address_byte_key]
            info[eeprom_address_byte_key] = 1024*2

        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        flash_info = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        eeprom_info = device_memory_info.memory_info_by_name(MemoryNames.EEPROM)

        mem_tuple_flash = self._generate_memorytuple(flash_info[DeviceMemoryInfoKeys.SIZE], flash_info)
        mem_tuple_eeprom = self._generate_memorytuple(eeprom_info[DeviceMemoryInfoKeys.SIZE], eeprom_info)
        memories = [mem_tuple_flash, mem_tuple_eeprom]

        write_memories_to_hex(filename, memories)
        segments_read = read_memories_from_hex(filename, device_memory_info)

        for segment_read in segments_read:
            memory_name = segment_read.memory_info[DeviceMemoryInfoKeys.NAME]
            # When writing multiple segments all will start from relative offset 0
            self.assertEqual(segment_read.offset, 0, msg="Incorrect offset for {}".format(memory_name))
            if memory_name == MemoryNames.FLASH:
                self.assertEqual(segment_read.data, mem_tuple_flash.data, msg="Incorrect FLASH data")
            elif memory_name == MemoryNames.EEPROM:
                self.assertEqual(segment_read.data, mem_tuple_eeprom.data, msg="Incorrect EEPROM data")
            else:
                self.fail("Unexpected memory: {}".format(memory_name))

        # Write back original values to not mess up later tests
        if flash_size_words_key in info:
            info[flash_size_words_key] = flash_size_words_original
        else:
            info[flash_size_bytes_key] = flash_size_bytes_original
        if eeprom_address_word_key in info:
            info[eeprom_address_word_key] = eeprom_address_word_original
        else:
            info[eeprom_address_byte_key] = eeprom_address_byte_original

    def test_write_memory_when_data_is_outside_any_memory_raises_indexerror(self):
        """
        This test exercises the write_memory_to_hex function (single memory)
        """
        info = Backend.get_device_info('attiny817')
        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        flash_info = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        numbytes = flash_info[DeviceMemoryInfoKeys.SIZE] + 1
        filename = "{}outside_flash.hex".format(TESTFILE_FOLDER)

        mem_tuple_flash = self._generate_memorytuple(numbytes, flash_info)

        with self.assertRaises(IndexError):
            write_memory_to_hex(filename, mem_tuple_flash, 0)

    def test_write_memories_when_data_is_outside_any_memory_raises_indexerror(self):
        """
        This test exercises the write_memories_to_hex function (multiple memories)
        """
        info = Backend.get_device_info('attiny817')
        device_memory_info = deviceinfo.DeviceMemoryInfo(info)
        flash_info = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        eeprom_info = device_memory_info.memory_info_by_name(MemoryNames.EEPROM)
        numbytes_flash = flash_info[DeviceMemoryInfoKeys.SIZE] + 1
        numbytes_eeprom = eeprom_info[DeviceMemoryInfoKeys.SIZE]
        filename = "{}outside_flash_multi.hex".format(TESTFILE_FOLDER)

        mem_tuple_flash = self._generate_memorytuple(numbytes_flash, flash_info)
        mem_tuple_eeprom = self._generate_memorytuple(numbytes_eeprom, eeprom_info)
        memories = [mem_tuple_flash, mem_tuple_eeprom]

        with self.assertRaises(IndexError):
            write_memories_to_hex(filename, memories)


    def test_read_hex_when_data_is_outside_any_memory_raises_indexerror(self):
        # Just use a PIC device to avoid having to create fake device info
        devinfo = Backend.get_device_info('pic16f18446')
        device_memory_info = deviceinfo.DeviceMemoryInfo(devinfo)
        flashinfo = device_memory_info.memory_info_by_name(MemoryNames.FLASH)
        filename = "{}out_of_range.hex".format(TESTFILE_FOLDER)
        numbytes = 16
        data = self._generate_dummydata(numbytes)

        # Make a hex file with some data inside flash memory and some data just after the
        # flash memory (outside any memory boundary)
        ihex = IntelHex()
        hexindex = flashinfo[DeviceMemoryInfoKeys.HEXFILE_ADDRESS] + flashinfo[DeviceMemoryInfoKeys.SIZE] - 4
        for databyte in data:
            ihex[hexindex] = databyte
            hexindex += 1
        _write_hex_to_file(ihex, filename)

        with self.assertRaises(IndexError):
            read_memories_from_hex(filename, device_memory_info)
