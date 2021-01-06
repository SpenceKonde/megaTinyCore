"""
CMSIS-DAP wrapper for custom commands (using vendor extensions)
This mechanism is used to pass JTAGICE3-style commands for AVR devices
over the CMSIS-DAP interface
"""
import time
from logging import getLogger
from ..util.binary import unpack_be16
from ..util import print_helpers
from .cmsisdap import CmsisDapUnit


class AvrCommandError(Exception):
    """
    Exception type for AVR command-response wrapping
    """
    pass


class AvrCommand(CmsisDapUnit):
    """
    Wraps AVR command and responses
    """

    # Vendor Commands used to transport AVR over CMSIS-DAP
    AVR_COMMAND = 0x80
    AVR_RESPONSE = 0x81
    AVR_EVENT = 0x82
    AVR_MORE_FRAGMENTS = 0x00
    AVR_FINAL_FRAGMENT = 0x01

    # Retry delay on AVR receive frame
    AVR_RETRY_DELAY_MS = 50

    def __init__(self, transport, no_timeouts=False):
        self.no_timeouts = no_timeouts
        self.timeout = 1000
        CmsisDapUnit.__init__(self, transport)
        self.ep_size = transport.get_report_size()
        self.logger = getLogger(__name__)
        self.logger.debug("Created AVR command on DAP wrapper")

    def poll_events(self):
        """
        Polling for events from AVRs

        :return: response from events
        """
        self.logger.debug("Polling AVR events")
        resp = self.dap_command_response(bytearray([self.AVR_EVENT]))
        return resp

    def _avr_response_receive_frame(self):
        retries = int(self.timeout / self.AVR_RETRY_DELAY_MS)
        # Get the delay in seconds
        delay = self.AVR_RETRY_DELAY_MS / 1000
        while retries or self.no_timeouts:
            resp = self.dap_command_response(bytearray([self.AVR_RESPONSE]))
            if resp[0] != self.AVR_RESPONSE:
                # Response received is not valid.  Abort.
                raise AvrCommandError("AVR response DAP command failed; invalid token: 0x{:02X}".format(resp[0]))
            if resp[1] != 0x00:
                return resp
            self.logger.debug("Resp: %s", print_helpers.bytelist_to_hex_string(resp))

            # Delay in seconds
            time.sleep(delay)
            retries -= 1
        raise AvrCommandError("AVR response timeout")

    # Chops command up into fragments
    def _fragment_command_packet(self, command_packet):
        packets_total = int((len(command_packet) / (self.ep_size - 4)) + 1)
        self.logger.debug("Fragmenting AVR command into {:d} chunks".format(packets_total))
        fragments = []
        for i in range(0, packets_total):
            command_fragment = bytearray([self.AVR_COMMAND, ((i + 1) << 4) + packets_total])
            if (len(command_packet) - (i * (self.ep_size - 4))) > (self.ep_size - 4):
                length = self.ep_size - 4
            else:
                length = len(command_packet) - (i * (self.ep_size - 4))

            command_fragment.append(int(length >> 8))
            command_fragment.append(int(length & 0xFF))

            for j in range(0, self.ep_size - 4):
                if j < length:
                    command_fragment.append(command_packet[i * (self.ep_size - 4) + j])
                else:
                    command_fragment.append(0x00)

            fragments.append(command_fragment)
        return fragments

    # Sends an AVR command and waits for response
    def avr_command_response(self, command):
        """
        Sends an AVR command and receives a response

        :param command: Command bytes to send
        :return: Response bytes received
        """
        fragments = self._fragment_command_packet(command)
        self.logger.debug("Sending AVR command")
        for fragment in fragments:
            self.logger.debug("Sending AVR command 0x{:02X}".format(fragment[0]))
            resp = self.dap_command_response(fragment)
            if resp[0] != self.AVR_COMMAND:
                raise AvrCommandError("AVR command DAP command failed; invalid token: 0x{:02X}".format(resp[0]))
            if fragment == fragments[-1]:
                if resp[1] != self.AVR_FINAL_FRAGMENT:
                    raise AvrCommandError(
                        "AVR command DAP command failed; invalid final fragment ack: 0x{:02X}".format(resp[1]))
            else:
                if resp[1] != self.AVR_MORE_FRAGMENTS:
                    raise AvrCommandError(
                        "AVR command DAP command failed; invalid non-final fragment ack: 0x{:02X}".format(resp[1]))

        # Receive response
        fragment_info, _, response = self._avr_response_receive_fragment()
        packets_remaining = (fragment_info & 0xF) - 1
        for _ in range(0, packets_remaining):
            fragment_info, _, data = self._avr_response_receive_fragment()
            response.extend(data)
        return response

    def _avr_response_receive_fragment(self):
        fragment = []
        # Receive a frame
        response = self._avr_response_receive_frame()

        # Get the payload size from the header information
        size = unpack_be16(response[2:4])

        # The message header is 4 bytes, where the last two hold the size of the payload
        if len(response) < (4 + size):
            raise AvrCommandError("Response size does not match the header information.")

        # Extract data
        for i in range(0, size):
            fragment.append(response[4 + i])

        fragment_info = response[1]
        return fragment_info, size, fragment
