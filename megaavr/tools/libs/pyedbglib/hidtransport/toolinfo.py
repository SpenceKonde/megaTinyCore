"""Gathering of all known Microchip CMSIS-DAP debuggers and default EP sizes"""

from logging import getLogger

# List of known useful HID/CMSIS-DAP tools
# 3G tools:
USB_TOOL_DEVICE_PRODUCT_ID_JTAGICE3 = 0x2140
USB_TOOL_DEVICE_PRODUCT_ID_ATMELICE = 0x2141
USB_TOOL_DEVICE_PRODUCT_ID_POWERDEBUGGER = 0x2144
USB_TOOL_DEVICE_PRODUCT_ID_EDBG_A = 0x2111
USB_TOOL_DEVICE_PRODUCT_ID_ZERO = 0x2157
USB_TOOL_DEVICE_PRODUCT_ID_MASS_STORAGE = 0x2169
USB_TOOL_DEVICE_PRODUCT_ID_PUBLIC_EDBG_C = 0x216A
USB_TOOL_DEVICE_PRODUCT_ID_KRAKEN = 0x2170

# 4G tools:
USB_TOOL_DEVICE_PRODUCT_ID_MEDBG = 0x2145

# 5G tools:
USB_TOOL_DEVICE_PRODUCT_ID_NEDBG_HID_MSD_DGI_CDC = 0x2175
USB_TOOL_DEVICE_PRODUCT_ID_PICKIT4_HID_CDC = 0x2177
USB_TOOL_DEVICE_PRODUCT_ID_SNAP_HID_CDC = 0x2180

# The Product String Names are used to identify the tool based on the USB
# device product strings (i.e. these names are usually just a subset of the
# actual product strings)
TOOL_SHORTNAME_TO_USB_PRODUCT_STRING = {
    'atmelice': "Atmel-ICE",
    'powerdebugger': "Power Debugger",
    'pickit4': "MPLAB PICkit 4",
    'snap': "MPLAB Snap",
    'nedbg': "nEDBG",
    'jtagice3': "JTAGICE3",
    'medbg': "mEDBG",
    'edbg': "EDBG",
}

def get_default_report_size(pid):
    """
    Retrieve default EP report size based on known PIDs

    :param pid: product ID
    :return: packet size
    """
    logger = getLogger(__name__)
    hid_tools = [
        # 3G
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_JTAGICE3, 'default_report_size': 512},
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_ATMELICE, 'default_report_size': 512},
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_POWERDEBUGGER, 'default_report_size': 512},
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_EDBG_A, 'default_report_size': 512},
        # 4G
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_MEDBG, 'default_report_size': 64},
        # 5G
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_NEDBG_HID_MSD_DGI_CDC, 'default_report_size': 64},
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_PICKIT4_HID_CDC, 'default_report_size': 64},
        {'pid': USB_TOOL_DEVICE_PRODUCT_ID_SNAP_HID_CDC, 'default_report_size': 64}]

    logger.debug("Looking up report size for pid 0x{:04X}".format(pid))
    for tool in hid_tools:
        if tool['pid'] == pid:
            logger.debug("Default report size is {:d}".format(tool['default_report_size']))
            return tool['default_report_size']
    logger.debug("PID not found! Reverting to 64b.")
    return 64

def tool_shortname_to_product_string_name(shortname):
    """
    Mapping for common short names of tools to product string name

    The intention is that this function is always run on the tool name and that the conversion
    only happens if the name is a known shortname. If the shortname is not known of if the name
    provided is already a valid Product string name then the provided shortname parameter will
    just be returned unchanged. So if the name already is a correct Product string name it is
    still safe to run this conversion funtion on it.

    :param shortname: shortname typically used by atbackend (powerdebugger, atmelice etc.)
    :return: String to look for in USB product strings to identify the tool
    """
    logger = getLogger(__name__)

    if shortname is None:
        logger.debug("Tool shortname is None")
        # This is also valid as the user might have provided no tool name, but the conversion function
        # should still be valid
        return shortname

    shortname_lower = shortname.lower()
    if shortname_lower not in TOOL_SHORTNAME_TO_USB_PRODUCT_STRING:
        logger.debug("%s is not a known tool shortname", shortname)
        # ...but it could be a valid Product string name already so no reason to report an error
        return shortname

    return TOOL_SHORTNAME_TO_USB_PRODUCT_STRING[shortname_lower]
