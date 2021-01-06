"""
Python MCU programmer utility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

pymcuprog is a utility for programming various Microchip MCU devices using Microchip CMSIS-DAP based debuggers

pymcuprog can be used as a library using its "backend API".  For example:

Setup logging - pymcuprog uses the Python logging module
    >>> import logging
    >>> logging.basicConfig(format="%(levelname)s: %(message)s", level=logging.WARNING)

Configure the session:
    >>> from pymcuprog.backend import SessionConfig
    >>> sessionconfig = SessionConfig("atmega4808")

Instantiate USB transport (only 1 tool connected)
    >>> from pymcuprog.toolconnection import ToolUsbHidConnection
    >>> transport = ToolUsbHidConnection()

Instantiate backend
    >>> from pymcuprog.backend import Backend
    >>> backend = Backend()

Connect to tool using transport
    >>> backend.connect_to_tool(transport)

Start the session
    >>> backend.start_session(sessionconfig)

Read the target device_id
    >>> device_id = backend.read_device_id()
    >>> print ("Device ID is {0:06X}".format(int.from_bytes(d, byteorder="little")))

Print the pymcuprog package version:
    >>> from pymcuprog.version import VERSION as pymcuprog_version
    >>> print("pymcuprog version {}".format(pymcuprog_version))

In addition, the CLI-backend API is versioned for convenience:
    >>> print("pymcuprog backend API version: {}".format(backend.get_api_version()))

Logging
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This package uses the Python logging module for publishing log messages to library users.
A basic configuration can be used (see example), but for best results a more thorough configuration is
recommended in order to control the verbosity of output from dependencies in the stack which also use logging.
See logging.yaml which is included in the package (although only used for CLI)

Dependencies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pymcuprog depends on pyedbglib for its transport protocol.
pyedbglib requires a USB transport library like libusb.  See pyedbglib package for more information.

Supported devices and tools
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Note: pymcuprog is primarily intended for use with PKOB nano (nEDBG) debuggers which
are found on Curiosity Nano kits and other development boards.  This means that it is
continuously tested with a selection of AVR devices with UPDI interface as well as a
selection of PIC devices.  However since the protocol is compatible between all
EDBG-based debuggers (pyedbglib) it is possible to use pymcuprog with a wide range of
debuggers and devices, although not all device families/interfaces have been implemented.

The following Atmel/Microchip debuggers are supported:
    * JTAGICE3 (only firmware version 3.x)
    * Atmel-ICE
    * Power Debugger
    * EDBG
    * mEDBG
    * PKOB nano (nEDBG)
    * MPLAB PICkit 4 ICD (only when in 'AVR mode')
    * MPLAB Snap ICD (only when in 'AVR mode')

Not all functionality is provided on all boards.  See device support below.

The following device-types are supported:
    * All UPDI devices, whether mounted on kits or standalone
    * PIC devices mounted on Curiosity Nano kits, or similar board with PKOB nano (nEDBG) debugger
    * Other devices (eg ATmega328P, ATsamd21e18a) may be partially supported for experimental purposes
"""
import logging
logging.getLogger(__name__).addHandler(logging.NullHandler())
