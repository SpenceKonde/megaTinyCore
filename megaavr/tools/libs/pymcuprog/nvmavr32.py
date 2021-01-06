"""
AVR32 NVM implementation
"""
from abc import abstractmethod

from .nvm import NvmAccessProviderCmsisDapAvr
from .avr32target import Avr32Device
from .pymcuprog_errors import PymcuprogError
from .deviceinfo.deviceinfokeys import DeviceInfoKeysAvr32

class NvmAccessProviderCmsisDapAvr32(NvmAccessProviderCmsisDapAvr):
    """
    AVR32 programmer
    """

    def __init__(self, transport, device_info, interface="jtag"):
        NvmAccessProviderCmsisDapAvr.__init__(self, device_info)

        self._log_incomplete_stack('AVR32')

        if DeviceInfoKeysAvr32.RESET_DOMAINS in device_info:
            self.avr = Avr32Device(transport, device_info[DeviceInfoKeysAvr32.RESET_DOMAINS])
        else:
            self.avr = Avr32Device(transport)
        self.avr.setup_prog_session(interface)

    def __del__(self):
        pass

    def read_device_id(self):
        """
        Read the device info

        :returns: Device ID raw bytes (Little endian)
        """
        resp = self.avr.activate_physical()
        self.logger.info("JTAG ID read: %02X%02X%02X%02X", resp[3], resp[2], resp[1], resp[0])
        self.avr.deactivate_physical()
        return bytearray(resp[0:4])

    @abstractmethod
    def read(self, memory_info, offset, numbytes):
        """
        Read the memory in chunks

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset in the memory type
        :param numbytes: number of bytes to read
        :return: array of bytes read
        """
        raise PymcuprogError("Implement read")

    @abstractmethod
    def write(self, memory_info, offset, data):
        """
        Write the memory with data

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset within the memory type
        :param data: the data to program
        :return: None
        """
        raise PymcuprogError("Implement write")

    @abstractmethod
    def erase(self, memory_info=None, address=None):
        """
        Do an erase of the device
        """
        raise PymcuprogError("Implement erase")

    def stop(self):
        """
        Stop programming session
        """
        return
