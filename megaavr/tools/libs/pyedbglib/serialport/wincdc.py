"""
Class with helper functions for CDC virtual com ports on Windows.
This code taken from pyatmeltcftester project and minimally adapted for python3.
This code still needs to be Python2 compatible.
This module should not be used directly, use SerialPortMap instead
for cross-platform functionalty.
"""

from __future__ import print_function
import sys
import os
from logging import getLogger

if os.name == "nt":
    if sys.version.startswith("3."):
        import winreg
    else:
        import _winreg as winreg
else:
    # If Sphinx is running there is no need to raise any import errors as Sphinx is just generating documentation
    if 'sphinx' not in sys.modules:
        raise ImportError("This CDC module is for Windows only")


class CDC:
    """
    Windows-only implementation of hid device to CDC serial port map.
    """
    def __init__(self):
        """
        Hook onto logger
        """
        self.logger = getLogger(__name__)

    def func_name(self):
        """
        Get function name
        """
        return "%s::%s" % (__name__, sys._getframe(1).f_code.co_name)

    def iter_keys_as_str(self, key):
        """
        Iterate over subkeys of a key returning subkey as string
        """
        for i in range(winreg.QueryInfoKey(key)[0]):
            yield winreg.EnumKey(key, i)

    def iter_keys(self, key):
        """
        Iterate over subkeys of a key
        """
        for i in range(winreg.QueryInfoKey(key)[0]):
            yield winreg.OpenKey(key, winreg.EnumKey(key, i))

    def iter_vals(self, key):
        """
        Iterate over values of a key
        """
        for i in range(winreg.QueryInfoKey(key)[1]):
            yield winreg.EnumValue(key, i)

    def find_cdc_port(self, tool, serial):
        """
        Find virtual serial port name based on tool name and serial number
        """
        if tool == 'edbgc':
            tool = 'edbg'
        if tool == 'nedbg':
            tool = 'Curiosity'

        winreg.Enum = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r'SYSTEM\CurrentControlSet\Enum')
        usb_devs = winreg.OpenKey(winreg.Enum, 'USB')

        parentids = []
        # Iterate over all devices to find the parent ID of the one with the correct serial number
        for key in self.iter_keys(usb_devs):
            for subkey in self.iter_keys(key):
                try:
                    devparams = winreg.OpenKey(subkey, "Device Parameters")
                    symbname = winreg.QueryValueEx(devparams, 'SymbolicName')[0]
                    if serial in symbname:
                        parentids += [winreg.QueryValueEx(subkey, 'ParentIdPrefix')[0]]
                except:
                    pass

        # Iterate over all devices to find the COM port number matching the previously found parent ID
        for key in self.iter_keys(usb_devs):
            for subkey_string in self.iter_keys_as_str(key):
                subkey = winreg.OpenKey(key, subkey_string)
                for val in self.iter_vals(subkey):
                    if val[0] == 'FriendlyName' and '{} virtual com port'.format(tool.lower()) in val[1].lower():
                        for parentid in parentids:
                            if parentid in subkey_string:
                                self.logger.info("Found CDC port:")
                                self.logger.info("%s", val[1])
                                return val[1].split('(')[1].split(')')[0]

        return None
