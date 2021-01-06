"""Interface to GEN4 script engine, based on Asynchronous Transport Interface"""

from logging import getLogger
from ..protocols.ati import AsynchronousTransportInterface
from ..protocols.ati import get_ati_header
from ..protocols.ati import ATI_EXEC_GEN4_SCRIPT
from ..protocols.ati import ATI_RESPONSE_BUFFER_SIZE

from ..pyedbglib_errors import PyedbglibNotSupportedError
from ..util import binary

GEN4_ENVELOPE_VERSION_MAJOR = 1
GEN4_ENVELOPE_VERSION_MINOR = 0

GEN4_ENVELOPE_RESPONSE_VERSION = 1

GEN4_RSP_VERSION_FIELD = 0
GEN4_RSP_SCRIPT_STATUS_FIELD = 1
GEN4_RSP_ENGINE_STATUS_FIELD = 2
GEN4_RSP_EXECTIME_FIELD = 3
GEN4_RSP_BYTES_SENT_FIELD = 5
GEN4_RSP_BYTES_RECEIVED_FIELD = 7
GEN4_RSP_RESULT_FIELD = 9

DATA_SOURCE_UNDEFINED = 0xFF
DATA_DEST_UNDEFINED = 0xFF
SCRIPT_SOURCE_INLINE = 0xFF

class Gen4Exception(Exception):
    """Custom Exception for GEN4 scripts"""

    def __init__(self, msg=None, code=0):
        super(Gen4Exception, self).__init__(msg)
        self.code = code


class Gen4ControllerCommand(object):
    """
    Gen4ControllerCommand wraps a communication exchange with the debugger.

    Fields are populated by the client, and a bytestream is then generated and sent to the debugger for execution.
    """

    def __init__(self, content=None):
        # Data source ID
        self.data_source = DATA_SOURCE_UNDEFINED
        # Data destination ID
        self.data_dest = DATA_DEST_UNDEFINED
        # "Inline" scripts
        self.script_source = SCRIPT_SOURCE_INLINE
        # Parameter array
        self.parameters = []
        # Content (script)
        self.content = content

    def set_data_source(self, source_id):
        """
        Sets the data source buffer

        :param source_id: data source buffer ID
        """
        self.data_source = source_id

    def set_data_dest(self, destination_id):
        """
        Sets the data dest buffer

        :param destination_id: data dest buffer ID
        """
        self.data_dest = destination_id

    def set_script_source(self, src_id):
        """Sets the script source"""
        if src_id != SCRIPT_SOURCE_INLINE:
            raise PyedbglibNotSupportedError("Only inline scripts are currently supported!")
        self.script_source = src_id

    def add_parameter(self, value_array):
        """Adds a parameter to the command"""
        self.parameters.append(value_array)

    def generate_bytestream(self):
        """
        Turn the envelope into a byte-stream

        :return: byte-stream
        """
        # Start empty
        stream = bytearray()
        # Source ID
        stream.extend([self.data_source])
        # Dest ID
        stream.extend([self.data_dest])
        # Param block
        stream.extend(bytearray([len(self.parameters)]))
        for parameter in self.parameters:
            stream.extend(parameter)
        # Content pointer
        stream.extend([self.script_source])
        # Content itself
        stream.extend(self.content)
        return stream


class Gen4Controller(AsynchronousTransportInterface):
    """Wrapper for accessing GEN4 "scripts" in 5G FW."""

    def __init__(self, transport):
        AsynchronousTransportInterface.__init__(self, transport)
        self.logger = getLogger(__name__)

    def new_command(self, content=None):
        """
        Make a new command object for this controller class

        :param content: content to pack into command
        :return: command object instance
        """
        return Gen4ControllerCommand(content)

    def start_script_execution(self, script):
        """
        Starts a script executing and returns immediately

        :param script: array containing binary script byte-code to execute
        """
        cmd = get_ati_header(ATI_EXEC_GEN4_SCRIPT)
        cmd.extend(bytearray([GEN4_ENVELOPE_VERSION_MAJOR, GEN4_ENVELOPE_VERSION_MINOR]))
        cmd.extend([1])  # 1 section only
        cmd.extend(binary.pack_le16(len(script)))
        cmd.extend(script)
        self.write_command_buffer(cmd)

    def receive_script_execution_response(self):
        """
        Read response from script execution

        :return:
            retval - array of 32-bits integers returned from the script execution
        """
        resp = self.read_response_buffer(ATI_RESPONSE_BUFFER_SIZE)
        return resp[1:]

    def execute(self, script):
        """
        Atomic execution

        :param script: array containing binary script byte-code to execute
        :return: results
        """
        self.start_script_execution(script)
        raw_results = self.receive_script_execution_response()

        rsp_version = raw_results[GEN4_RSP_VERSION_FIELD]
        if rsp_version > GEN4_ENVELOPE_RESPONSE_VERSION:
            self.logger.error("Unsupported response version (%d)", rsp_version)

        script_status = raw_results[GEN4_RSP_SCRIPT_STATUS_FIELD]
        self.logger.debug("Script status: %d", script_status)
        if script_status != 0x00:
            raise Gen4Exception("Script failed, script status: 0x{:02X}".format(script_status))

        engine_status = raw_results[GEN4_RSP_ENGINE_STATUS_FIELD]
        self.logger.debug("Engine status: %d", engine_status)

        execution_time = binary.unpack_le16(raw_results[GEN4_RSP_EXECTIME_FIELD:GEN4_RSP_EXECTIME_FIELD + 2])
        self.logger.debug("Execution time: %d", execution_time)

        data_sent = binary.unpack_le16(raw_results[GEN4_RSP_BYTES_SENT_FIELD:GEN4_RSP_BYTES_SENT_FIELD + 2])
        self.logger.debug("Data sent: %d", data_sent)

        data_received = binary.unpack_le16(raw_results[GEN4_RSP_BYTES_RECEIVED_FIELD:GEN4_RSP_BYTES_RECEIVED_FIELD + 2])
        self.logger.debug("Data received: %d", data_received)

        result = binary.unpack_le32(raw_results[GEN4_RSP_RESULT_FIELD:GEN4_RSP_RESULT_FIELD + 4])
        self.logger.debug("Result: %08X", result)

        return result
