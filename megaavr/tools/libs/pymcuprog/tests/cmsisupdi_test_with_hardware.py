"""
Test for UPDI over CMSIS-DAP

Requires an UPDI device with at least 1k flash and 64b EEPROM
"""
import os
from random import randint
from intelhex import IntelHex
# import pytest

FLASHSIZE = 1 * 1024
EEPROMSIZE = 64
USERROWSIZE = 32
TESTFILE = "testtemp.hex"


def do_cli_test(teststring):
    """
    Executes a CLI command directly on the OS
    """
    retcode = os.system(teststring)
    if retcode != 0:
        raise Exception("Failed!")


def generate_test_hexfile(filename, size):
    """
    Generates a random hex file

    :param filename: location and name to generate
    :param size: number of bytes of data
    """
    hexfile = IntelHex()
    # Add some data to the hexfile
    for i in range(size):
        hexfile[i] = randint(0, 255)
    # Turn the hexfile object into an actual file
    hexfile.write_hex_file(filename)


def test_cli(tmpdir, capfd):
    """
    Runs the test suite
    """
    test_filename = os.path.normpath(tmpdir + '/' + TESTFILE)

    do_cli_test("python pymcuprog.py ping")
    assert "Ping response: 1E" in capfd.readouterr().out

    do_cli_test("python pymcuprog.py erase -m eeprom")
    assert "Erasing eeprom\r\nErased" in capfd.readouterr().out

    do_cli_test("python pymcuprog.py read -m eeprom -b {} -o 0".format(EEPROMSIZE))
    assert "0x0000: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF" in capfd.readouterr().out

    do_cli_test("python pymcuprog.py erase -m user_row")
    assert "Erasing user_row\r\nErased" in capfd.readouterr().out

    do_cli_test("python pymcuprog.py read -m user_row -b {} -o 0".format(USERROWSIZE))
    assert "0x0000: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF" in capfd.readouterr().out

    do_cli_test("python pymcuprog.py erase")
    assert "Erasing...\r\nErased" in capfd.readouterr().out

    generate_test_hexfile(test_filename, FLASHSIZE)
    do_cli_test("python pymcuprog.py write -f {}".format(test_filename))
    assert "Writing...\r\nReading...\r\nVerifying...\r\nOK" in capfd.readouterr().out

    generate_test_hexfile(test_filename, EEPROMSIZE)
    do_cli_test("python pymcuprog.py write -m eeprom -f {}".format(test_filename))
    assert "Writing...\r\nReading...\r\nVerifying...\r\nOK" in capfd.readouterr().out
