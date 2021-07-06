"""
Backend interface for the pymcuprog utility.

This module is the boundary between the Command Line Interface (CLI) part and
the backend part that does the actual job.  Any external utility or script that
needs access to the functionality provided by pymcuprog should connect to the
interface provided by this backend module
"""
# Python 3 compatibility for Python 2
from __future__ import print_function

import os
from logging import getLogger

# pyedbglib dependencies
from pyedbglib.hidtransport.hidtransportfactory import hid_transport
from pyedbglib.hidtransport.hidtransportbase import HidTransportBase
from pyedbglib.protocols import housekeepingprotocol
from pyedbglib.protocols.jtagice3protocol import Jtagice3ResponseError

from .pymcuprog_errors import PymcuprogToolConfigurationError, PymcuprogToolConnectionError
from .pymcuprog_errors import PymcuprogNotSupportedError, PymcuprogEraseError
from .pymcuprog_errors import PymcuprogSessionConfigError, PymcuprogSessionError
from .programmer import Programmer
from .deviceinfo import deviceinfo
from .deviceinfo.memorynames import MemoryNames
from .deviceinfo.memorynames import MemoryNameAliases
from .deviceinfo.eraseflags import ChiperaseEffect
from .deviceinfo.deviceinfokeys import DeviceInfoKeys, DeviceMemoryInfoKeys
from .toolconnection import ToolUsbHidConnection, ToolSerialConnection
from .utils import read_tool_info
from .utils import read_target_voltage, read_supply_voltage_setpoint, read_usb_voltage
from .utils import set_supply_voltage_setpoint
from .hexfileutils import read_memories_from_hex

# Files in devices folder not representing devices
NON_DEVICEFILES = ["__init__.py"]
DEVICE_FOLDER = os.path.dirname(os.path.abspath(__file__)) + "//deviceinfo//devices"

# This class is a collection of parameters so no need for any methods
#pylint: disable=too-few-public-methods
class SessionConfig(object):
    """
    Collection of all parameters needed when configuring a programming session

    Used as input parameter for the start_session function
    """
    device = None
    interface = None
    # For some interfaces this is baud in bits per second and for other interfaces this is clock frequency in Hz
    interface_speed = None
    # Path to python devicesupportscripts for PIC devices
    packpath = None

    # Content and format of special_options will depend on the device stack implementation.
    # Normally these options are not in use.
    special_options = None

    def __init__(self, device):
        """
        device name is mandatory
        """
        self.device = device

# To achieve a single entry point for users of the backend part of pymcuprog it is accepted to exceed the maximum
# number of methods.
#pylint: disable=too-many-public-methods
class Backend(object):
    """
    Backend interface of the pymcuprog utility.
    This class provides access to all the functionality provided by pymcuprog
    """
    API_VERSION = '2.0'

    def __init__(self):
        # Hook onto logger
        self.logger = getLogger(__name__)
        self.transport = None
        self.connected_to_tool = False
        self.session_active = False
        self.programmer = None
        self.device_memory_info = None
        self.housekeeper = None

    def get_api_version(self):
        """
        Returns the current pymcuprog API version
        """
        return self.API_VERSION

    @staticmethod
    def get_supported_devices():
        """
        Return a list of devices supported by pymcuprog.

        This will be the list of devices with a corresponding device file
        :returns: List of device names
        """
        devices = []
        for filename in os.listdir(DEVICE_FOLDER):
            if filename not in NON_DEVICEFILES and filename.endswith('.py'):
                devices.append(filename.split('.py')[0])

        return devices

    @staticmethod
    def get_available_hid_tools(serialnumber_substring='', tool_name=None):
        """
        Return a list of Microchip USB HID tools (debuggers) connected to the host

        :param serialnumber_substring: can be an empty string or a subset of a serial number. Not case sensitive
            This function will do matching of the last part of the devices serial numbers to
            the serialnumber_substring. Examples:
            '123' will match "MCHP3252000000043123" but not "MCP32520001230000000"
            '' will match any serial number
        :param tool_name: tool type to connect to. If None any tool matching the serialnumber_substring
            will be returned
        :returns: List of pyedbglib.hidtransport.hidtransportbase.HidTool objects
        """
        # Just use a temporary transport as the request is only to report connected Microchip HID tools,
        # not to connect to any of them
        transport = hid_transport()

        return transport.get_matching_tools(serialnumber_substring, tool_name)

    def connect_to_tool(self, toolconnection):
        """
        Connect to a tool

        The tool can either be a USB HID tool or a serial port.
        :param ToolConnection: This is an instance of one of the ToolConnection sub-classes.  This object wraps
            parameters needed to identify which tool to connect to like tool name and USB serial or serial port
            name (e.g. 'COM1').

            For USB HID tools there are some special handling:
            - If both tool name and usb_serial are None any tool will be picked.
            - If usb_serial is None any tool matching the tool name will be picked
            - If tool name is None any tool matching the usb_serial will be picked
            - If more than one tool is connected that matches the tool name and usb_serial parameters a
              PymcuprogToolConnectionError exception will be raised.

        :raises: PymcuprogToolConnectionError if more than one matching tool is found or if no matching tool is found
        :raises: PymcuprogToolConfigurationError if the toolconnection configuration is incorrect
        """
        if isinstance(toolconnection, ToolSerialConnection):
            # For serial port connection no connection action is needed, just need to store the
            # Serial port number to be used (e.g. 'COM1')
            self.transport = toolconnection.serialport
        elif isinstance(toolconnection, ToolUsbHidConnection):
            self.transport = hid_transport()
            connect_status = False
            try:
                connect_status = self.transport.connect(serial_number=toolconnection.serialnumber,
                                                        product=toolconnection.tool_name)
            except IOError as error:
                raise PymcuprogToolConnectionError("Unable to connect to USB device ({})".format(error))

            if not connect_status:
                raise PymcuprogToolConnectionError("Unable to connect to USB device")

            self.housekeeper = housekeepingprotocol.Jtagice3HousekeepingProtocol(self.transport)
            self.housekeeper.start_session()

        else:
            raise PymcuprogToolConfigurationError("Unknown toolconnection argument type: {})".
                                                  format(type(toolconnection)))

        self.connected_to_tool = True

    def disconnect_from_tool(self):
        """
        Disconnect the connected tool

        If no tool is connected nothing is done (i.e. no exception raised when not connected)
        """
        if self._is_connected_to_hid_tool():
            self.housekeeper.end_session()
            self.transport.disconnect()

        self.connected_to_tool = False

    def read_tool_info(self):
        """
        Interrogates tool (debugger) for useful info

        :returns: Dictionary with various info about the connected debugger

        :raises PymcuprogToolConnectionError if not connected to any USB HID tool (connect_to_tool not run)
        """
        self._is_hid_tool_not_connected_raise()

        return read_tool_info(self.housekeeper)

    def read_kit_device(self):
        """
        Read out the device name from kit configuration.

        If the connected tool does not have any kit configuration
        (i.e. the tool is not an onboard debugger) None will be returned.
        connect_to_tool must have been called before calling read_kit_device, but start_session is not necessary.
        Typically read_kit_device is used to get the device name required to configure a session before calling
        start_session.
        :returns: Name of target device as given by the kit, None if the tool does not have any device configured.

        :raises PymcuprogToolConnectionError if not connected to any USB HID tool (connect_to_tool not run)
        """
        self._is_hid_tool_not_connected_raise()

        dap_info = read_tool_info(self.housekeeper)

        device_name = dap_info['device_name'].lower()

        if device_name == '':
            device_name = None

        return device_name

    def read_target_voltage(self):
        """
        Read target voltage

        :returns: Measured target voltage

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogNotSupportedError if the tool does not have supply capabilities
        """
        self._is_hid_tool_not_connected_raise()

        try:
            voltage = read_target_voltage(self.housekeeper)
        except Jtagice3ResponseError:
            raise PymcuprogNotSupportedError("Connected debugger/board does not have target voltage read capability")

        return voltage

    def read_supply_voltage_setpoint(self):
        """
        Read tool power supply voltage setpoint

        :returns: Tool power supply voltage setpoint

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogNotSupportedError if the tool does not have supply capabilities
        """
        self._is_hid_tool_not_connected_raise()

        try:
            voltage = read_supply_voltage_setpoint(self.housekeeper)
        except Jtagice3ResponseError:
            raise PymcuprogNotSupportedError("Connected debugger/board does not have supply voltage capability.")

        return voltage

    def read_usb_voltage(self):
        """
        Read USB voltage

        :returns: Measured USB voltage

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogNotSupportedError if the tool can't measure USB voltage
        """
        self._is_hid_tool_not_connected_raise()

        try:
            voltage = read_usb_voltage(self.housekeeper)
        except Jtagice3ResponseError:
            raise PymcuprogNotSupportedError("Connected debugger/board does not have USB voltage read capability.")

        return voltage

    def set_supply_voltage_setpoint(self, setpoint):
        """
        Set tool power supply voltage setpoint

        :param setpoint: Power supply setpoint

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogNotSupportedError if the tool does not have supply capabilities
        :raises: ValueError if the setpoint is out of range
        """
        self._is_hid_tool_not_connected_raise()

        set_supply_voltage_setpoint(self.housekeeper, setpoint)


    def reboot_tool(self):
        """
        Trigger a reboot of the tool (debugger)

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        """
        self._is_hid_tool_not_connected_raise()

        self.housekeeper.end_session(reset_tool=True)

        # A tool reboot will automatically disconnect the tool.  Calling self.disconnect_from_tool
        # would just fail as it would try to talk to a tool while it is rebooting
        self.connected_to_tool = False

    @staticmethod
    def get_device_info(device):
        """
        Get info about a device

        :param device: Name of the device
        :returns: dictionary with device info as defined in the device files in pymcuprog.deviceinfo.devices

        :raises: PymcuprogNotSupportedError if device is not supported
        """
        try:
            info = deviceinfo.getdeviceinfo(device)
        except ModuleNotFoundError:
            raise PymcuprogNotSupportedError("No device info for device: {}".format(device))

        return info

    def start_session(self, sessionconfig, user_interaction_callback=None):
        """
        Start a programming session.

        This function will build the device model stack and initialize the tool for a
        programming session.  If a session is already started calling start_session will do an end_session and start
        a new session from scratch.

        Note connect_to_tool must have been called before start_session is called.  If not an exception will be thrown.

        :param sessionconfig: SessionConfig object wrapping the parameters configuring the session
        :param user_interaction_callback: Callback to be called when user interaction is required,
            for example when doing UPDI high-voltage activation with user target power toggle.
            This function could ask the user to toggle power and halt execution waiting for the user
            to respond (this is default behavior if the callback is None), or if the user is another
            script it could toggle power automatically and then return.

        :raises: PymcuprogSessionConfigError if starting the session failed due to incorrectly configured session
        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogDeviceLockedError if unable to start the session due to the device being locked
        :raises: PymcuprogNotSupportedError if configured device is not supported
        """
        self._is_tool_not_connected_raise()

        # Check that all session configuration parameters required are in place
        if sessionconfig.device is None or sessionconfig.device == '':
            raise PymcuprogSessionConfigError("Device must be specified")

        if self.session_active:
            # A session is already active so it must be ended before starting a new session
            self.end_session()

        # Setup the programmer
        self.programmer = Programmer(self.transport)

        if sessionconfig.special_options is not None:
            self.programmer.set_options(sessionconfig.special_options)

        # Try to build the stack for this device
        self.programmer.load_device(sessionconfig.device)

        self.programmer.setup_device(
            sessionconfig.interface,
            sessionconfig.packpath,
            sessionconfig.interface_speed)

        # Make contact
        self.programmer.start(user_interaction_callback=user_interaction_callback)

        # Get device memory info
        self.device_memory_info = self.programmer.get_device_memory_info()

        self.session_active = True

    def end_session(self):
        """
        End a programming session

        This will take down the device model stack and stop the programming session on the tool.  However the tool will
        not be disconnected and it will be possible to do another start_session without another connect_to_tool call.
        If no session has been started this function will do nothing (i.e. it won't fail even if a session has
        not been started)
        """
        if self.session_active:
            # Lower the flag first to ensure it is updated as the rest of this function might fail with an exception
            # for example if UPDI were disabled during the session
            self.session_active = False
            self.programmer.stop()

    def read_device_id(self):
        """
        Read out the device id

        :return Byte array with device ID as raw byte values.  Number of bytes will depend upon target type

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        return self.programmer.read_device_id()


    def erase(self, memory_name=MemoryNameAliases.ALL, address=None):
        """
        Erase target device memory

        If a single memory is specified it will only be erased if it won't affect other memories
        :param memory: name of memory to erase.  To unlock a device use the MemoryNameAliases.ALL
            MemoryNameAliases.ALL run the widest erase:
                - For PIC the widest bulk erase will be run.
                - For AVR a chip erase will be run
                - The following memories will not be erased:
                    - AVR fuses
                    - EEPROM if EESAVE fuse is set for AVR
                    - EEPROM if the target device does not support EEPROM erase
                    - EEPROM if Data Code Protection (CPD_n) is not enabled for PIC
                    - PIC ICD memory (special memory used for Debug Executives)
        :param address: optional address for erase command.  If address is None the complete memory
            segment will be erased.  Note that the address parameter will just propagate through the stack down to the
            device dependent implementation (devicesupportscripts for PIC and firmware for AVR).  Normal use is to
            leave the address as None.

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        :raises: ValueError if the specified memory is not defined for the target device
        :raises: PymcuprogEraseError if the memory can't be erased or if the memory can't be erased without affecting
            other memories
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        if memory_name is not None and memory_name != MemoryNameAliases.ALL:
            if not self.is_isolated_erase_possible(memory_name):
                message = "{} memory can't be erased or can't be erased without side effect".format(memory_name)
                raise PymcuprogEraseError(message)

        self.programmer.erase(memory_name, address)

    def is_isolated_erase_possible(self, memory_name):
        """
        Can the memory be erased without affecting other memories?

        :param memory_name: name of memory
        :return: True only if the memory can be erased without side effects, False if memory can't be erased at all or
            if erasing it will erase other memories too.

        :raises ValueError if memory is not defined for the configured device
        """
        # The device model must have been loaded upfront
        self._is_session_not_active_raise()

        meminfo = self.device_memory_info.memory_info_by_name(memory_name)
        isolated_erase_key = DeviceMemoryInfoKeys.ISOLATED_ERASE
        if isolated_erase_key in meminfo:
            return meminfo[isolated_erase_key] is True

        self.logger.error('%s flag not found for %s memory', isolated_erase_key, memory_name)
        return False

    def get_chiperase_effect(self, memory_name):
        """
        Get the effect of a chip erase (widest bulk erase) on the given memory

        :param memory_name: name of memory
        :return: One of the values defined by deviceinfo.eraseflags.ChiperaseEffect depending upon the settings in the
            device model for the configured device.  If the chiperase_effect flag is missing in the device model
            ChiperaseEffect.NOT_ERASED will be returned.

        :raises ValueError if memory is not defined for the configured device
        """
        # The device model must have been loaded upfront
        self._is_session_not_active_raise()

        meminfo = self.device_memory_info.memory_info_by_name(memory_name)
        chiperase_effect_key = DeviceMemoryInfoKeys.CHIPERASE_EFFECT
        if chiperase_effect_key in meminfo:
            return meminfo[chiperase_effect_key]

        self.logger.error('%s flag not found for %s memory', chiperase_effect_key, memory_name)
        return ChiperaseEffect.NOT_ERASED

    def read_memory(self, memory_name=MemoryNameAliases.ALL, offset_byte=0, numbytes=0, max_chunk_size=None):
        """
        Read target device memory

        :param memory_name: Name of memory as defined in memorynames.py.  MemoryNameAliases.ALL reads all memories
            defined in the device model (numbytes and offset_byte will be ignored).
        :param offset_byte: Byte offset within memory to start reading at.
        :param numbytes: Number of bytes to read.  0 means read all memory locations from offset_byte and until end
            of memory
        :return: list of namedtuples with two fields: data and memory_info.  data contains a byte array of
            raw data bytes and memory_info is a dictionary with memory information (as defined in
            deviceinfo.deviceinfo.DeviceMemoryInfo).  Normally the list will contain one item, but when
            memory_name parameter is MemoryNameAliases.ALL there will be one namedtuple item per memory
            type read.

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        :raises: ValueError if trying to read outside the specified memory
        :raises: ValueError if the specified memory is not defined for the target device
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        return self.programmer.read_memory(memory_name=memory_name, offset=offset_byte, numbytes=numbytes, max_chunk_size=max_chunk_size)

    def write_memory(self, data, memory_name=MemoryNames.FLASH, offset_byte=0, blocksize=0, pagewrite_delay=0):
        """
        Write target device memory

        :param memory_name: Name of memory as defined in memorynames.py
        :param offset_byte: Byte offset within memory to start writing to.
        :param data: bytearray of raw data bytes to write
        :param blocksize: max number of bytes to send at a time. Ignored if 0 or omitted, and not passed
            to write_memory; only serialupdi supports this.

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        :raises: ValueError if trying to write outside the specified memory
        :raises: ValueError if the specified memory is not defined for the target device
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()
        if blocksize == 0:
            self.programmer.write_memory(data=data, memory_name=memory_name, offset=offset_byte, pagewrite_delay=pagewrite_delay)
        else:
            self.programmer.write_memory(data=data, memory_name=memory_name, offset=offset_byte, blocksize=blocksize, pagewrite_delay=pagewrite_delay)

    def verify_memory(self, data, memory_name=MemoryNames.FLASH, offset_byte=0, max_read_chunk=None):
        """
        Verify target device memory

        :param memory_name: Name of memory as defined in DeviceMemoryInfo (deviceinfo.py)
        :param offset_byte: Byte offset within memory to start verifying at.
        :param data: bytearray of raw data bytes to verify against
        :return: boolean compare status

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        :raises: ValueError if trying to verify outside the specified memory
        :raises: ValueError if the specified memory is not defined for the target device
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        return self.programmer.verify_memory(data=data, memory_name=memory_name, offset=offset_byte, max_read_chunk=max_read_chunk)

    def hold_in_reset(self):
        """
        Hold target device in reset

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        self.programmer.hold_in_reset()

    def release_from_reset(self):
        """
        Release target device from reset

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        self.programmer.release_from_reset()

        # Releasing the target from reset will take it out of programming mode.  In other words the session
        # is partly taken down.  To keep housekeeping right and to take down the stack properly end_session
        # must be called
        self.end_session()

    def write_hex_to_target(self, hexfile):
        """
        Write hexfile to target device

        Note no erase will be run (i.e. memory is assumed to already be erased)

        :param hexfile: name of file to write
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        hex_memories = read_memories_from_hex(os.path.abspath(hexfile), self.device_memory_info)
        for segment in hex_memories:
            memory_name = segment.memory_info[DeviceInfoKeys.NAME]
            self.logger.debug("Writing %s...", memory_name)
            self.write_memory(segment.data, memory_name, segment.offset)

    def verify_hex(self, hexfile):
        """
        Verify target memory content against hexfile

        :param hexfile: name of file to verify against
        :return: boolean compare status

        :raises: PymcuprogToolConnectionError if not connected to any tool (connect_to_tool not run)
        :raises: PymcuprogSessionError if a session has not been started (session_start not run)
        """
        self._is_tool_not_connected_raise()
        self._is_session_not_active_raise()

        hex_memories = read_memories_from_hex(os.path.abspath(hexfile), self.device_memory_info)
        verify_ok = True
        for segment in hex_memories:
            memory_name = segment.memory_info[DeviceInfoKeys.NAME]
            self.logger.debug("Verifying %s...", memory_name)
            segment_ok = self.verify_memory(segment.data, memory_name, segment.offset, max_read_chunk=max_read_chunk)
            if segment_ok:
                self.logger.debug("OK!")
            else:
                verify_ok = False

        return verify_ok

    def _is_tool_not_connected_raise(self):
        """
        Check if any tool is connected and if not raise an exception

        :raises: PymcuprogToolConnectionError if not connected to any tool
        """
        if not self._is_connected_to_hid_tool() and not self._is_connected_to_serialport():
            raise PymcuprogToolConnectionError("Not connected to any tool")

    def _is_hid_tool_not_connected_raise(self):
        """
        Check if a USB HID tool is connected and if not raise an exception

        :raises: PymcuprogToolConnectionError if not connected to any tool
        """
        if not self._is_connected_to_hid_tool():
            raise PymcuprogToolConnectionError("Not connected to any USB HID debugger")

    def _is_connected_to_hid_tool(self):
        """
        Check if a connection to a USB HID tool is active
        """
        return self.connected_to_tool and isinstance(self.transport, HidTransportBase)

    def _is_connected_to_serialport(self):
        """
        Check if a connection to a Serial port is active
        """
        # For Serial port communication transport is only set to a string with the name of the serial port
        # to use (e.g. 'COM1').
        return self.connected_to_tool and isinstance(self.transport, str)

    def _is_session_not_active_raise(self):
        """
        Check if a programming session is active and if not raise an exception

        :raises: PymcuprogSessionError if programming session not active
        """
        if not self.session_active:
            raise PymcuprogSessionError("No programming session active")
