"""JTAGICE3 protocol mappings"""

from logging import getLogger

from .avrcmsisdap import AvrCommand
from ..util import binary
from ..util import print_helpers
from ..pyedbglib_errors import PyedbglibError


class Jtagice3Command(AvrCommand):
    """
    Sends a "JTAGICE3" command frame, and received a response

        JTAGICE3 protocol header is formatted:
            JTAGICE3_TOKEN      0x0E
            PROTOCOL_VERSION    0
            SEQUENCE_NUMBER_L
            SEQUENCE_NUMBER_H
            HANDLER_ID
            PAYLOAD

        Response format is:
            JTAGICE3_TOKEN      0x0E
            SEQUENCE_NUMBER_L   echo
            SEQUENCE_NUMBER_H   echo
            HANDLER_ID
            PAYLOAD
    """

    # JTAGICE3 protocol token
    JTAGICE3_TOKEN = 0x0E
    JTAGICE3_PROTOCOL_VERSION = 0x00

    # Handlers within JTAGICE3 protocol
    HANDLER_DISCOVERY = 0x00
    HANDLER_HOUSEKEEPING = 0x01
    HANDLER_SPI = 0x11
    HANDLER_AVR8_GENERIC = 0x12
    HANDLER_AVR32_GENERIC = 0x13
    HANDLER_TPI = 0x14
    HANDLER_EDBG = 0x20
    HANDLER_COPROCESSOR = 0x21
    HANDLER_POWER = 0x22
    HANDLER_SELFTEST = 0x81

    def __init__(self, transport, handler):
        super(Jtagice3Command, self).__init__(transport)
        self.logger = getLogger(__name__)
        self.logger.debug("Created JTAGICE3 command")
        self.handler = handler
        self.sequence_id = 0

    def validate_response(self, response):
        """
        Validates the response form the debugger

        :param response: raw response bytes
        """
        self.logger.debug("Checking response (%s)", print_helpers.bytelist_to_hex_string(response))

        # Check length first
        if len(response) < 5:
            raise PyedbglibError("Invalid response length ({:d}).".format(len(response)))

        # Check token
        if response[0] != self.JTAGICE3_TOKEN:
            raise PyedbglibError("Invalid token (0x{:02X}) in response.".format(response[0]))

        # Check sequence
        sequence = response[1] + (response[2] << 8)
        if self.sequence_id != sequence:
            raise PyedbglibError(
                "Invalid sequence in response (0x{:04X} vs 0x{:04X}).".format(self.sequence_id, sequence))

            # Check handler
        if response[3] != self.handler:
            raise PyedbglibError("Invalid handler (0x{:02X}) in response.".format(response[3]))

    def jtagice3_command_response_raw(self, command):
        """
        Sends a JTAGICE3 command and receives the corresponding response

        :param command:
        :return:
        """
        # Header
        header = bytearray([self.JTAGICE3_TOKEN, self.JTAGICE3_PROTOCOL_VERSION, self.sequence_id & 0xFF,
                            (self.sequence_id >> 8) & 0xFF, self.handler])

        # Send command, receive response
        packet = header + bytearray(command)
        response = self.avr_command_response(packet)
        return response

    def jtagice3_command_response(self, command):
        """
        Sends a JTAGICE3 command and receives the corresponding response, and validates it

        :param command:
        :return:
        """
        response = self.jtagice3_command_response_raw(command)

        # Increment sequence number
        self.sequence_id += 1
        if self.sequence_id > 0xFFFE:
            self.sequence_id = 1

        # Peel and return
        return response[4:]


class Jtagice3ResponseError(Exception):
    """Exception type for JTAGICE3 responses"""

    def __init__(self, msg, code):
        super(Jtagice3ResponseError, self).__init__(msg)
        # self.message = msg
        self.code = code


class Jtagice3Protocol(Jtagice3Command):
    """
    Base class for all protocols in the JTAGICE3 family.

    All sub-protocols support query, get and set commands.
    """

    # Command versioning
    CMD_VERSION0 = 0
    CMD_VERSION1 = 1

    # All handler share these functions:
    CMD_QUERY = 0x00
    CMD_SET = 0x01
    CMD_GET = 0x02

    # And these base responses
    PROTOCOL_OK = 0x80
    PROTOCOL_LIST = 0x81
    PROTOCOL_DATA = 0x84
    PROTOCOL_FAILED = 0xA0
    # PROTOCOL_FAILED_WITH_DATA = 0xA1

    # Failure codes
    FAILURE_OK = 0

    #  CMD_SET and CMD_GET failure codes
    SETGET_FAILURE_OK = 0x00
    SETGET_FAILURE_NOT_IMPLEMENTED = 0x10
    SETGET_FAILURE_NOT_SUPPORTED = 0x11
    SETGET_FAILURE_INVALID_CLOCK_SPEED = 0x20
    SETGET_FAILURE_ILLEGAL_STATE = 0x21
    SETGET_FAILURE_JTAGM_INIT_ERROR = 0x22
    SETGET_FAILURE_INVALID_VALUE = 0x23
    SETGET_FAILURE_HANDLER_ERROR = 0x30

    """Mapping JTAGICE3 error codes to more human friendly strings"""
    JTAGICE3_ERRORS = {0: 'SUCCESS'}

    def __init__(self, transport, handler, supports_trailing_status=True):
        super(Jtagice3Protocol, self).__init__(transport, handler)
        self.logger = getLogger(__name__)
        self.logger.debug("Created JTAGICE3 protocol")
        self.supports_trailing_status = supports_trailing_status

    def check_response(self, response, expected=None):
        """
        Checks the response for known errors

        :param response: response bytes
        :param expected: expected response
        :return: data from response
        """
        status, data = self.peel_response(response, expected)
        if not status:
            error_message = self.error_as_string(data[0])
            msg = "JTAGICE3 error response code 0x{:02X}: '{:s}' ".format(data[0], error_message)
            self.logger.error(msg)
            raise Jtagice3ResponseError(error_message, data[0])

        return data

    def error_as_string(self, code):
        """
        Get the response error as a string (error code translated to descriptive string)

        :param code: error code
        :return: error code as descriptive string
        """
        try:
            return self.JTAGICE3_ERRORS[code]
        except KeyError:
            return "Unknown error!"

    def peel_response(self, response, expected=None):
        """
        Process the response, extracting error codes and data

        :param response: raw response bytes
        :param expected: expected response
        :return: status, data
        """
        return_list = False, [0xFF]
        # Special handling
        if expected is not None and response[0] == expected:
            return_list = True, response[2:]
        else:
            if response[0] == self.PROTOCOL_OK:
                return_list = True, []
            elif response[0] == self.PROTOCOL_LIST:
                return_list = True, response[2:]
            elif response[0] == self.PROTOCOL_DATA:
                # Trailing status is not included on some handlers
                if self.supports_trailing_status and response[-1] == self.FAILURE_OK:
                    return_list = True, response[2:-1]
                else:
                    return_list = False, [response[-1]]
            elif response[0] == self.PROTOCOL_FAILED:
                return_list = False, [response[2]]

        return return_list

    def query(self, context):
        """
        Queries functionality using the QUERY API

        :param context: Query context
        :return: List of supported entries
        """
        self.logger.debug("Query to context 0x{:02X}".format(context))
        resp = self.jtagice3_command_response([self.CMD_QUERY, self.CMD_VERSION0, context])
        status, data = self.peel_response(resp)
        if not status:
            msg = "Unable to QUERY (failure code 0x{:02X})".format(data[0])
            raise PyedbglibError(msg)
        return data

    def set_byte(self, context, offset, value):
        """
        Sets a single byte parameter

        :param context: context (address) to set
        :param offset: offset address to set
        :param value: value to set
        :return:
        """
        self._set_protocol(context, offset, bytearray([value]))

    def set_le16(self, context, offset, value):
        """
        Sets a little-endian 16-bit parameter

        :param context: context (address) to set
        :param offset: offset address to set
        :param value: value to set
        """
        self._set_protocol(context, offset, binary.pack_le16(value))

    def set_le32(self, context, offset, value):
        """
        Sets a little-endian 32-bit parameter

        :param context: context (address) to set
        :param offset: offset address to set
        :param value: value to set
        """
        self._set_protocol(context, offset, binary.pack_le32(value))

    def _set_protocol(self, context, offset, data):
        """
        Generic function for setting parameters

        :param context: context (address) to set
        :param offset: offset address to set
        :param data: values to set
        """
        self.logger.debug("JTAGICE3::set {:d} byte(s) to context {:d} offset {:d}".format(len(data),
                                                                                          context,
                                                                                          offset))
        resp = self.jtagice3_command_response(
            bytearray([self.CMD_SET, self.CMD_VERSION0, context, offset, len(data)]) + data)
        resp_status, resp_data = self.peel_response(resp)
        if not resp_status:
            msg = "Unable to SET (failure code 0x{:02X})".format(resp_data[0])
            raise PyedbglibError(msg)

    def get_byte(self, context, offset):
        """
        Get a single-byte parameter

        :param context: context (address) to set
        :param offset: offset address to set
        :return: value read
        """
        data = self._get_protocol(context, offset, 1)
        return data[0]

    def get_le16(self, context, offset):
        """
        Get a little-endian 16-bit parameter

        :param context: context (address) to set
        :param offset: offset address to set
        :return: value read
        """
        data = self._get_protocol(context, offset, 2)
        return binary.unpack_le16(data)

    def get_le32(self, context, offset):
        """
        Get a little-endian 32-bit parameter

        :param context: context (address) to set
        :param offset: offset address to set
        :return: value read
        """
        data = self._get_protocol(context, offset, 4)
        return binary.unpack_le32(data)

    def _get_protocol(self, context, offset, numbytes):
        """
        Generic function to get a parameter

        :param context: context (address) to set
        :param offset: offset address to set
        :param numbytes: number of bytes to get
        :return: value read
        """
        self.logger.debug("JTAGICE3::get {:d} byte(s) from context {:d} offset {:d}".format(numbytes, context, offset))
        resp = self.jtagice3_command_response([self.CMD_GET, self.CMD_VERSION0, context, offset, numbytes])
        status, data = self.peel_response(resp)
        if not status:
            msg = "Unable to GET (failure code 0x{:02X})".format(data[0])
            raise Jtagice3ResponseError(msg, data)
        return data
