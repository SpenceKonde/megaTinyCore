"""DAP information wrapper for any CMSIS-DAP tool"""

from logging import getLogger
from .dapwrapper import DapWrapper

ID_DAP_INFO = 0x00

DAP_ID_VENDOR = 1
DAP_ID_PRODUCT = 2
DAP_ID_SER_NUM = 3
DAP_ID_FW_VER = 4
DAP_ID_DEVICE_VENDOR = 5
DAP_ID_DEVICE_NAME = 6
DAP_ID_CAPABILITIES = 0xF0
DAP_ID_PACKET_COUNT = 0xFE
DAP_ID_PACKET_SIZE = 0xFF


class DapCommand(DapWrapper):
    """DAP information wrapper for any CMSIS-DAP tool"""

    def __init__(self, transport):
        super(DapCommand, self).__init__(transport)
        self.logger = getLogger(__name__)
        self.logger.debug("Created DAP command wrapper")

    def get_all_info(self):
        """
        Fetches a set of useful information from the DAP

        :return: dictionary of parameters
        """
        dapinfo = {'vendor': str(self.info(DAP_ID_VENDOR)),
                   'product': str(self.info(DAP_ID_PRODUCT)),
                   'serialnumber': str(self.info(DAP_ID_SER_NUM)),
                   'firmwareversion': str(self.info(DAP_ID_FW_VER)),
                   'devicevendor': str(self.info(DAP_ID_DEVICE_VENDOR)),
                   'devicename': str(self.info(DAP_ID_DEVICE_NAME)),
                   'capabilities': self.info(DAP_ID_CAPABILITIES),
                   'packetcount': self.info(DAP_ID_PACKET_COUNT),
                   'packetsize': self.info(DAP_ID_PACKET_SIZE)}
        return dapinfo

    def info(self, dap_id):
        """
        Fetch specific DAP information given by dap_id

        :param dap_id: code for different DAP_ID_* fields
        :return: bytearray with dap information
        """
        packet = [ID_DAP_INFO, dap_id]
        response = self.dap_command_response(packet)
        if response[0] == ID_DAP_INFO:
            return bytearray(response[2:response[1] + 2])
        return bytearray([])
