"""Wrapper for any protocol over CMSIS-DAP"""

from logging import getLogger


class DapWrapper(object):
    """Base class for any CMSIS-DAP protocol wrapper"""

    def __init__(self, transport):
        self.logger = getLogger(__name__)
        self.transport = transport
        self.logger.debug("Created DapWrapper")

    def dap_command_response(self, packet):
        """
        Send a command, receive a response

        :param packet: bytes to send
        :return: response received
        """
        return self.transport.hid_transfer(packet)

    def dap_command_write(self, packet):
        """
        Send a packet

        :param packet: packed data to sent
        :return: bytes sent
        """
        return self.transport.hid_write(packet)

    def dap_command_read(self):
        """
        Receive data

        :return: data received
        """
        return self.transport.hid_read()
