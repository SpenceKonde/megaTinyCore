"""
Factory for HID transport connections.

Currently supports only Cython/HIDAPI
"""

import platform
from logging import getLogger
from ..pyedbglib_errors import PyedbglibNotSupportedError


def hid_transport(library="hidapi"):
    """
    Dispatch a transport layer for the OS in question

    The transport layer is typically used to connect to a tool and then it is passed in as a parameter when creating
    protocol objects.  An example where the transport layer is used to create an instance of the housekeepingprotocol
    for communication with the nEDBG debugger::

        from pyedbglib.hidtransport.hidtransportfactory import hid_transport
        transport = hid_transport()
        connect_status = False
        try:
            connect_status = transport.connect(serial_number='', product='nedbg')
        except IOError as error:
            print("Unable to connect to USB device ({})".format(error))

        if not connect_status:
            print("Unable to connect to USB device")

        housekeeper = housekeepingprotocol.Jtagice3HousekeepingProtocol(transport)

    :param library: Transport library to use, currently only 'hidapi' is supported which will use the libusb hidapi
    :type library: string
    :returns: Instance of transport layer object
    :rtype: class:cyhidapi:CyHidApiTransport
    """
    logger = getLogger(__name__)
    operating_system = platform.system().lower()
    logger.debug("HID transport using library '{:s}' on OS '{:s}'".format(library, operating_system))

    # HID API is the primary transport
    if library == 'hidapi':
        hid_api_supported_os = ['windows', 'darwin', 'linux', 'linux2']
        if operating_system in hid_api_supported_os:
            from .cyhidapi import CyHidApiTransport
            return CyHidApiTransport()

        msg = "System '{0:s}' not implemented for library '{1:s}'".format(operating_system, library)
        logger.error(msg)
        raise PyedbglibNotSupportedError(msg)

    # Other transports may include cmsis-dap DLL, atusbhid (dll or so) etc
    msg = "Transport library '{0}' not implemented.".format(library)
    logger.error(msg)
    raise PyedbglibNotSupportedError(msg)
