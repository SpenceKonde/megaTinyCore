"""Base class for all HID transport mechanisms."""

from logging import getLogger
from . import toolinfo


class HidTool(object):
    """
    Holds transport and DAP properties of a CMSIS-DAP debugger.

    Used to select the debugger to use if multiple debuggers are connected.
    """

    # pylint: disable=too-many-instance-attributes, too-many-arguments
    # These are primary keys used to identify the debugger.

    def __init__(self, vendor_id, product_id, serial_number, product_string="", manufacturer_string=""):
        self.logger = getLogger(__name__)
        self.interface_number = -1
        self.vendor_id = vendor_id
        self.product_id = product_id
        self.serial_number = serial_number
        self.product_string = product_string
        self.manufacturer_string = manufacturer_string
        self.firmware_version = ""
        self.device_vendor_id = ""
        self.device_name = ""
        self.packet_size = 64

    def set_packet_size(self, packet_size):
        """
        Sets the packet size

        :param packet_size: bytes per packet
        """
        self.packet_size = packet_size

    def set_product_string(self, product_string):
        """
        Sets the product string

        :param product_string: product name string
        """
        self.product_string = product_string


class HidTransportBase(object):
    """Base class for HID transports"""

    def __init__(self):
        self.logger = getLogger(__name__)
        self.devices = []
        self.device = None
        self.detect_devices()
        self.connected = False

    def __del__(self):
        # Make sure we always disconnect the HID connection
        self.disconnect()

    def detect_devices(self):
        """Raise error as this method needs to be overridden."""
        raise NotImplementedError("method needs to be defined by sub-class")

    def get_matching_tools(self, serial_number_substring='', product=None):
        """
        Returns a list of tools matching the given serial_number_substring and product.

        :param serial_number_substring: can be an empty string or a subset of a serial number. Not case sensitive
            This function will do matching of the last part of the devices serial numbers to
            the serial_number_substring. Examples:
            '123' will match "MCHP3252000000043123" but not "MCP32520001230000000"
            '' will match any serial number
        :param product: product type to connect to. If None any tool matching the serial_number_substring
            will be returned
        :return: List of matching tools
        """
        # Support systems which use None as the standard for a unspecified USB serial
        if serial_number_substring is None:
            serial_number_substring = ""

        # Making serial_number_substring case insensitive
        serial_number_substring = serial_number_substring.lower()

        # Support tool shortnames
        toolname_in_product_string = toolinfo.tool_shortname_to_product_string_name(product)
        if toolname_in_product_string is not None:
            # Making product name case insensitive
            toolname_in_product_string = toolname_in_product_string.lower()

        matching_devices = []
        for device in self.devices:
            if toolname_in_product_string is None or device.product_string.lower().startswith(
                    toolname_in_product_string):
                if device.serial_number.lower().endswith(serial_number_substring):
                    matching_devices.append(device)

        return matching_devices

    def connect(self, serial_number=None, product=None):
        """
        Makes a HID connection to a debugger

        :param serial_number: instance serial number to connect to
        :param product: product type to connect to
        :return: True if successfully connected to a tool, False if not
        """
        if self.connected:
            return True

        device_count = len(self.devices)
        self.logger.debug("{:d} devices available".format(device_count))
        if device_count == 0:
            self.logger.error("No CMSIS-DAP devices found.")
            return False

        matching_devices = self.get_matching_tools(serial_number_substring=serial_number, product=product)
        number_of_matching_devices = len(matching_devices)

        # Did we find exactly 1 tool?
        if number_of_matching_devices != 1:
            log_str = "Found {:d} daps matching the filter serial = \"{}\" and product = \"{}\""
            self.logger.debug(log_str.format(number_of_matching_devices, serial_number, product))
            if number_of_matching_devices > 1:
                self.logger.error("Too many products found. Please specify one of:")
                for device in self.devices:
                    self.logger.error(" > {:s} {:s}".format(device.product_string,
                                                            device.serial_number))
            return False

        # Everything is peachy, connect to the tool
        self.device = matching_devices[0]
        self.hid_connect(self.device)
        self.logger.debug("Connected OK")
        self.connected = True
        packet_size = toolinfo.get_default_report_size(self.device.product_id)
        self.device.set_packet_size(packet_size)
        self.hid_info()
        return True

    def disconnect(self):
        """Release the HID connection"""
        if self.connected:
            self.hid_disconnect()
        self.connected = False

    def hid_connect(self, device):
        """Raise error as this method needs to be overridden."""
        raise NotImplementedError("method needs to be defined by sub-class")

    def hid_info(self):
        """Raise error as this method needs to be overridden."""
        raise NotImplementedError("method needs to be defined by sub-class")

    def hid_disconnect(self):
        """Raise error as this method needs to be overridden."""
        raise NotImplementedError("method needs to be defined by sub-class")

    def get_report_size(self):
        """
        Get the packet size in bytes

        :return: bytes per packet/report
        """
        return self.device.packet_size
