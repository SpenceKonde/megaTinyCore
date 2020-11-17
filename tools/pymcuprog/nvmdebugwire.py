"""
DW NVM implementation
"""
from pyedbglib.protocols.jtagice3protocol import Jtagice3ResponseError

from .nvm import NvmAccessProviderCmsisDapAvr
from .avr8target import TinyAvrTarget

class NvmAccessProviderCmsisDapDebugwire(NvmAccessProviderCmsisDapAvr):
    """
    NVM Access the DW way
    """

    def __init__(self, transport, device_info):
        NvmAccessProviderCmsisDapAvr.__init__(self, device_info)

        self._log_incomplete_stack('AVR-debugWIRE')

        self.avr = TinyAvrTarget(transport)
        self.avr.setup_debug_session()

    def __del__(self):
        pass

    def read_device_id(self):
        """
        Read the device info

        :returns: Device ID raw bytes (Little endian)
        """
        try:
            resp = self.avr.activate_physical()
        except Jtagice3ResponseError:
            self.logger.error("Unable to activate debugWIRE.  Maybe ISP is active?")
            return "Error"
        self.logger.info("ID read: %02X%02X", resp[1], resp[0])
        self.avr.deactivate_physical()
        return bytearray(resp[0:2])

    def erase(self, memory_info=None, address=None):
        """
        Do a chip erase of the device
        """
        _dummy = memory_info
        _dummy = address
        self.logger.info("Unable to erase device using debugWIRE")

    @staticmethod
    def write(memory_info, offset, data):
        """
        Write the memory with data

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset within the memory type
        :param data: the data to program
        :return: None
        """
        _dummy = memory_info
        _dummy = offset
        _dummy = data
        # return None

    @staticmethod
    def read(memory_info, offset, numbytes):
        """
        Read the memory in chunks

        :param memory_info: dictionary for the memory as provided by the DeviceMemoryInfo class
        :param offset: relative offset in the memory type
        :param numbytes: number of bytes to read
        :return: array of bytes read
        """
        _dummy = memory_info
        _dummy = offset
        _dummy = numbytes
        return []
