"""Wrapper for the Asynchronous Transport Interface (ATI)"""

from logging import getLogger
from .dapwrapper import DapWrapper

# ATI frame header fields
ATI_FRAME_VENDOR_COMMAND_ID = 0
ATI_FRAME_FLAGS = 1
ATI_FRAME_LENGTH = 2
ATI_FRAME_PAYLOAD = 4

# ATI control bits
ATI_CTRL_BIT_READNWRITE = 7  # Read flag
ATI_CTRL_BIT_FRAME_EOF = 6  # End of frame
ATI_CTRL_BIT_FRAME_SOF = 5  # Start of frame
ATI_CTRL_BITS_TYPE = 3  # Frame type
ATI_CTRL_BITS_ID = 0  # Buffer ID

ATI_CTRL_TYPE_DATA = (0x00 << ATI_CTRL_BITS_TYPE)  # Data buffer
ATI_CTRL_TYPE_METADATA = (0x01 << ATI_CTRL_BITS_TYPE)  # Metadata buffer
ATI_CTRL_TYPE_CMDRSP = (0x02 << ATI_CTRL_BITS_TYPE)  # Command/response buffer (synchronised)
ATI_CTRL_TYPE_SYS = (0x03 << ATI_CTRL_BITS_TYPE)  # System request

ATI_FAILURE_FRAME = bytearray([ATI_FRAME_VENDOR_COMMAND_ID, 0x01])
ATI_OK_FRAME = bytearray([ATI_FRAME_VENDOR_COMMAND_ID, 0x02])

VENDOR_COMMAND_ATI = 0x94

# CMD/RSP specific stuff

ATI_QUERY = 0

# Default buffer sizes
ATI_DATA_BUFFER_SIZE = 512
ATI_RESPONSE_BUFFER_SIZE = 512

ATI_ENVELOPE_VERSION = 1
ATI_ENVELOPE_VERSION_VARIANT_DEFAULT = 0

# Supported functional modes
ATI_EXEC_GEN4_SCRIPT = 0x10
ATI_EXEC_PIC_PRIMITIVE = 0x30


def get_ati_header(handler, handler_variant=0):
    """
    Generates an ATI header for an ATI consumer

    :param handler: Which handler to invoke
    :param handler_variant: Variant (flags) of that handler
    :return: ATI header frame (bytearray)
    """
    return bytearray([ATI_ENVELOPE_VERSION, ATI_ENVELOPE_VERSION_VARIANT_DEFAULT, handler, handler_variant])


class AsynchronousTransportInterface(DapWrapper):
    """Generic wrapper class for the Asynchronous Transport Interface (ATI)"""

    def __init__(self, transport):
        self.transport = transport
        super(AsynchronousTransportInterface, self).__init__(self.transport)
        self.logger = getLogger(__name__)
        # TODO: The buffer size should be queried from the tool implementation.
        self.data_buffer_size = ATI_DATA_BUFFER_SIZE
        self.fragment_size = self.transport.get_report_size()

    def write_metadata_buffer(self, buffer_id, data):
        """
        Write data to metadata buffer

        :param buffer_id: ID of metadata buffer to write to
        :param data: Data bytes to be written to the buffer (bytearray)
        """
        return self.write_buffer(buffer_id, data, buffer_type=ATI_CTRL_TYPE_METADATA)

    def read_metadata_buffer(self, buffer_id, num_bytes=None):
        """
        Read data from metadata buffer

        :param buffer_id: ID of metadata buffer to read from
        :param num_bytes: Number of bytes to read from buffer
        :return: bytearray of databytes read from buffer
        """
        return self.read_buffer(buffer_id, num_bytes, buffer_type=ATI_CTRL_TYPE_METADATA)

    def write_data_buffer(self, buffer_id, data):
        """
        Write data to data buffer

        :param buffer_id: ID of data buffer to write to
        :param data: bytearray of data bytes to write to buffer
        """
        return self.write_buffer(buffer_id, data, buffer_type=ATI_CTRL_TYPE_DATA)

    def read_data_buffer(self, buffer_id, num_bytes=None):
        """
        Read data from data buffer

        :param buffer_id: ID of data buffer to read from
        :param num_bytes: Number of bytes to read from buffer
        :return: bytearray of data bytes read from buffer
        """
        return self.read_buffer(buffer_id, num_bytes, buffer_type=ATI_CTRL_TYPE_DATA)

    def write_command_buffer(self, data):
        """
        Write data to command buffer

        :param data: bytearray of data bytes to write to the buffer
        """
        return self.write_buffer(0, data, buffer_type=ATI_CTRL_TYPE_CMDRSP)

    def read_response_buffer(self, num_bytes=None):
        """
        Read data from the response buffer

        :param num_bytes: Number of bytes to read from buffer
        :return: bytearray of data bytes read from the buffer
        """
        return self.read_buffer(0, num_bytes, buffer_type=ATI_CTRL_TYPE_CMDRSP)

    def send_fragment(self, buffer_id, flags, data):
        """
        Send a fragment of data to a buffer. Limited by USB endpoint size

        :param buffer_id: ID of buffer to send data to
        :param flags: BIT:  | 7    |  6    |  5    |  4       3    |  2       1       0 |
                      USE:  | read |  EOF  |  SOF  |  buffer_type  |      Buffer ID     |
        :param data: bytearray of data bytes to write to the buffer
        """
        self.logger.info("Writing fragment to buffer %d (%d bytes)", buffer_id, len(data))
        frame = bytearray(ATI_FRAME_PAYLOAD)
        frame[ATI_FRAME_VENDOR_COMMAND_ID] = VENDOR_COMMAND_ATI
        frame[ATI_FRAME_FLAGS] = (0 << ATI_CTRL_BIT_READNWRITE) | flags | buffer_id & 0x07
        frame[ATI_FRAME_LENGTH] = (len(data) >> 8)
        frame[ATI_FRAME_LENGTH + 1] = (len(data) & 0xFF)
        frame.extend(data)
        resp = ATI_FAILURE_FRAME
        # Flags = 1 means not ready yet, Flags = 2 means ok, data was received
        while resp[0] != ATI_OK_FRAME[0] or resp[1] != ATI_OK_FRAME[1]:
            resp = self.dap_command_response(frame)
            self.logger.debug("Resp[0]: 0x%02X; Resp[1]: 0x%02X", resp[0], resp[1])

    def write_buffer(self, buffer_id, data, buffer_type=ATI_CTRL_TYPE_DATA):
        """
        Write data to a buffer. Will handle chopping of data to suit USB endpoint size

        :param buffer_id: ID of buffer to write data to
        :param data: bytearray of data bytes to be written
        :param buffer_type: ATI_CTRL_TYPE_DATA, ATI_CTRL_TYPE_METADATA,
                     ATI_CTRL_TYPE_CMDRSP or ATI_CTRL_TYPE_SYS
        """
        self.logger.info("Writing buffer %d (%d bytes)", buffer_id, len(data))
        flags = buffer_type | (1 << ATI_CTRL_BIT_FRAME_SOF)
        bytes_to_send = self.fragment_size - ATI_FRAME_PAYLOAD
        while data:
            if bytes_to_send >= len(data):
                bytes_to_send = len(data)
                flags |= (1 << ATI_CTRL_BIT_FRAME_EOF)
            # Extract a chunk
            data_frame = data[:bytes_to_send]
            self.send_fragment(buffer_id, flags, data_frame)
            flags = buffer_type
            data = data[bytes_to_send:]

    def receive_fragment(self, buffer_id, flags, bytes_to_receive):
        """
        Receive data fragment from buffer. Limited by USB endpoint size

        :param buffer_id: ID of buffer to receive from
        :param flags: BIT:  | 7    |  6    |  5    |  4       3    |  2       1       0 |
                      USE:  | read |  EOF  |  SOF  |  buffer_type  |      Buffer ID     |
        :param bytes_to_receive: Number of bytes to read from the buffer
        :return: bytearray of data bytes read from the buffer
        """
        self.logger.info("Fetching fragment from buffer %d (%d bytes)", buffer_id, bytes_to_receive)
        frame = bytearray(ATI_FRAME_PAYLOAD)
        frame[ATI_FRAME_VENDOR_COMMAND_ID] = VENDOR_COMMAND_ATI
        frame[ATI_FRAME_FLAGS] = (1 << ATI_CTRL_BIT_READNWRITE) | flags | buffer_id & 0x07
        frame[ATI_FRAME_LENGTH] = (bytes_to_receive >> 8)
        frame[ATI_FRAME_LENGTH + 1] = (bytes_to_receive & 0xFF)
        resp = ATI_FAILURE_FRAME
        # Flags = 0 means more data, flags = 2 means EOF and flags = 1 means data not ready yet
        while resp[0] != VENDOR_COMMAND_ATI or resp[1] != 0x00:
            resp = self.dap_command_response(frame)
            self.logger.debug("Resp[0]: 0x%02X; Resp[1]: 0x%02X", resp[0], resp[1])

        bytes_received = (resp[ATI_FRAME_LENGTH] << 8) + resp[ATI_FRAME_LENGTH + 1]
        data = resp[ATI_FRAME_PAYLOAD:ATI_FRAME_PAYLOAD + bytes_received]
        return data

    def read_buffer(self, buffer_id, num_bytes=None, buffer_type=ATI_CTRL_TYPE_DATA):
        """
        Read data from a buffer. Will handle chopping of data reads to suit USB endpoint size

        :param buffer_id: ID of buffer to read from
        :param num_bytes: Number of bytes to read from the buffer
        :param buffer_type: ATI_CTRL_TYPE_DATA, ATI_CTRL_TYPE_METADATA,
                     ATI_CTRL_TYPE_CMDRSP or ATI_CTRL_TYPE_SYS
        :return: bytearray of data bytes read from the buffer
        """
        if num_bytes is None:
            num_bytes = self.data_buffer_size
        self.logger.info("Reading buffer %d (%d bytes)", buffer_id, num_bytes)
        flags = buffer_type | (1 << ATI_CTRL_BIT_FRAME_SOF)
        bytes_to_receive = self.fragment_size - ATI_FRAME_PAYLOAD
        data_read = bytearray()
        while num_bytes:
            if bytes_to_receive >= num_bytes:
                bytes_to_receive = num_bytes
                flags |= (1 << ATI_CTRL_BIT_FRAME_EOF)
            data = self.receive_fragment(buffer_id, flags, bytes_to_receive)
            # Check for short packet (or EOF?)
            if len(data) != bytes_to_receive:
                num_bytes = 0
            else:
                num_bytes -= bytes_to_receive

            flags = buffer_type
            data_read += data
        return data_read
