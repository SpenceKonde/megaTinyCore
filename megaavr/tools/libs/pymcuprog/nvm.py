"""
NVM layer protocols
"""
# Python 3 compatibility for Python 2
from __future__ import print_function
from logging import getLogger

from .deviceinfo.deviceinfokeys import DeviceInfoKeys

def get_nvm_access_provider(transport, device_info, interface="", packpath=None, frequency=None, options=""):
    """
    Returns an NVM provider with the requested properties

    :param transport: transport layer object
    :param device_info: device info dict
    :param interface: physical interface for NVM
    :param packpath: path to pack
    :param frequency: interface clock
    :param options: special options
    :return: NVM access object
    """
    # Although it is considered best practice to have imports at top level, in this case it makes sense to have the
    # imports on the function level as in most cases only one import will be used.  Having all imports at the top
    # level will then be a waste of resources.
    #pylint: disable=import-outside-toplevel
    # There will be cyclic imports since the modules imported below containing NVM Access providers will import
    # from the current module since all NVM Access providers inherits from the NVM Access provider base classes
    # defined in the current module, but this should be ok since the imports below are late.
    #pylint: disable=cyclic-import
    accessprovider = None
    architecture = device_info[DeviceInfoKeys.ARCHITECTURE].lower()
    if DeviceInfoKeys.INTERFACE in device_info:
        interface = device_info[DeviceInfoKeys.INTERFACE].lower()

    if architecture in ['pic16', 'pic18', 'pic24']:
        from .nvmpic import NvmAccessProviderCmsisDapPic
        accessprovider = NvmAccessProviderCmsisDapPic(transport, device_info, packpath, options=options)
    elif architecture == 'avr8x':
        if isinstance(transport, str):
            if interface == 'updi':
                from .nvmserialupdi import NvmAccessProviderSerial
                accessprovider = NvmAccessProviderSerial(transport, device_info, baud=frequency, options=options)
        elif interface == 'updi':
            from .nvmupdi import NvmAccessProviderCmsisDapUpdi
            accessprovider = NvmAccessProviderCmsisDapUpdi(transport, device_info=device_info,
                                                           frequency=frequency, options=options)
    elif architecture == 'avr8':
        if interface == 'isp':
            if interface == "debugwire":
                from .nvmdebugwire import NvmAccessProviderCmsisDapDebugwire
                accessprovider = NvmAccessProviderCmsisDapDebugwire(transport, device_info)
            else:
                from .nvmspi import NvmAccessProviderCmsisDapSpi
                accessprovider = NvmAccessProviderCmsisDapSpi(transport, device_info)
    elif architecture == 'cortex-m0plus':
        from .nvmmzeroplus import NvmAccessProviderCmsisDapMZeroPlus
        accessprovider = NvmAccessProviderCmsisDapMZeroPlus(transport, device_info, frequency)
    elif architecture == 'avr32':
        from .nvmavr32 import NvmAccessProviderCmsisDapAvr32
        accessprovider = NvmAccessProviderCmsisDapAvr32(transport, device_info)

    return accessprovider

class NvmAccessProvider:
    """
    Wrapper for device info
    """

    def __init__(self, device_info):
        self.device_info = device_info
        self.logger = getLogger(__name__)

    def _log_incomplete_stack(self, device_stack):
        """
        Used to tell the user this device stack is not completed yet

        :param device_stack: User friendly name of target stack
        """
        self.logger.warning("")
        self.logger.warning("%s stack is in Alpha state", device_stack)
        self.logger.warning("Expect some features to be missing")
        self.logger.warning("")

    def start(self, user_interaction_callback=None):
        """
        Start (activate) session

        :param user_interaction_callback: Callback to be called when user interaction is required,
            for example when doing UPDI high-voltage activation with user target power toggle.
            This function could ask the user to toggle power and halt execution waiting for the user
            to respond (this is default behavior if the callback is None), or if the user is another
            script it could toggle power automatically and then return.
        """
        #pylint: disable=unused-argument
        self.logger.info("No specific initializer for this provider")

    def stop(self):
        """
        Stop (deactivate) session
        """
        self.logger.info("No specific de-initializer for this provider")

    def hold_in_reset(self):
        """
        Hold target in reset
        """
        self.logger.info("hold_in_reset not implemented for this provider")

    def release_from_reset(self):
        """
        Release target from reset
        """
        self.logger.info("release_from_reset not implemented for this provider")

class NvmAccessProviderCmsisDapTool(NvmAccessProvider):
    """
    General CMSIS-DAP Tool
    """

    def __init__(self, device_info):
        NvmAccessProvider.__init__(self, device_info)


class NvmAccessProviderCmsisDapAvr(NvmAccessProviderCmsisDapTool):
    """
    AVR CMSIS DAP Tool
    """

    def __init__(self, device_info):
        NvmAccessProviderCmsisDapTool.__init__(self, device_info)
