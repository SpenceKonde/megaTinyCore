"""Interface to primitive sequence executer, based on Asynchronous Transport interface"""

from logging import getLogger
from ..protocols.ati import AsynchronousTransportInterface
from ..protocols.ati import get_ati_header
from ..protocols.ati import ATI_EXEC_PIC_PRIMITIVE
from ..protocols.ati import ATI_RESPONSE_BUFFER_SIZE
from ..pyedbglib_errors import PyedbglibNotSupportedError
from ..util import binary

PRIMITIVE_ENVELOPE_VERSION_MAJOR = 1
PRIMITIVE_ENVELOPE_VERSION_MINOR = 0

DATA_SOURCE_UNDEFINED = 0xFF
DATA_DEST_UNDEFINED = 0xFF
PRIMITIVE_SOURCE_INLINE = 0xFF


class PrimitiveControllerCommand(object):
    """
    PrimitiveControllerCommand wraps a communication exchange with the debugger.

    Fields are populated by the client, and a bytestream is then generated and sent to the debugger for execution.
    """

    def __init__(self, content=None):
        # Data source ID
        self.data_source = DATA_SOURCE_UNDEFINED
        # Data destination ID
        self.data_dest = DATA_DEST_UNDEFINED
        # "Inline" primitives
        self.prim_source = PRIMITIVE_SOURCE_INLINE
        # Parameter array
        self.parameters = []
        # Content (primitives)
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

    def set_primitive_source(self, src_id):
        """Sets the primitive source"""
        if src_id != PRIMITIVE_SOURCE_INLINE:
            raise PyedbglibNotSupportedError("Only inline primitives are currently supported!")
        self.prim_source = src_id

    def add_parameter(self, param_id, value, flags=0):
        """Adds a parameter to the command"""
        param = bytearray()
        param.extend(param_id)
        param.extend(flags)
        param.extend(binary.pack_le32(value))
        self.parameters.append(param)
        raise PyedbglibNotSupportedError("Parameters are not yet supported!")

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
        # Content pointer (not used)
        stream.extend([0xFF])  # Content inline
        # Content itself
        stream.extend(self.content)
        return stream


class PrimitiveController(AsynchronousTransportInterface):
    """Wrapper for accessing primitives and sequences thereof in 5G FW."""

    def __init__(self, transport):
        AsynchronousTransportInterface.__init__(self, transport)
        self.logger = getLogger(__name__)

    def new_command(self, content=None):
        """Create a new controller command"""
        return PrimitiveControllerCommand(content)

    def start_primitive_execution(self, primitive_blocks):
        """Starts a primitive executing and returns immediately"""
        cmd = get_ati_header(ATI_EXEC_PIC_PRIMITIVE)
        cmd.extend(bytearray([PRIMITIVE_ENVELOPE_VERSION_MAJOR, PRIMITIVE_ENVELOPE_VERSION_MINOR]))
        cmd.extend([len(primitive_blocks)])  # number of blocks
        for block in primitive_blocks:
            cmd.extend([len(block)])
            cmd.extend(block)
        self.write_command_buffer(cmd)

    def receive_primitive_execution_response(self):
        """
        Read response from primitive execution

        :return:
            retval - array of 32-bits integer returned from the primitive execution blocks
        """
        resp = self.read_response_buffer(ATI_RESPONSE_BUFFER_SIZE)
        return resp[1:]

    def execute(self, primitive_blocks):
        """
        Atomic execution

        :param primitive_blocks: blocks to execute
        :return: results
        """
        blocks = len(primitive_blocks)
        self.start_primitive_execution(primitive_blocks)
        raw_results = self.receive_primitive_execution_response()
        results = []
        for i in range(blocks):
            results.append(raw_results[i * 4:i * 4 + 4])
        return results

    def execute_single_block(self, primitive_block):
        """
        Atomic execution

        :param primitive_block: block to execute
        :return: results
        """
        return self.execute([primitive_block])[0]
