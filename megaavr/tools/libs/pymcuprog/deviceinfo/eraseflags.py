"""
Definitions of erase related flags for the device models
"""
import inspect

from pymcuprog.utils import enum

# Flag used to specify if a memory type will be erased by a chip erase (AVR) or the widest/default bulk erase (PIC)
ChiperaseEffect = enum(
    ALWAYS_ERASED='always erased',
    CONDITIONALLY_ERASED_AVR='conditionally erased (depending upon EESAVE fuse setting)',
    CONDITIONALLY_ERASED_PIC='conditionally erased (depending upon Code Protect configuration bit(s) settings)',
    NOT_ERASED='not erased')

def get_list_of_chiperase_effects():
    """Return a list of all ChiperaseEffect values"""
    chiperase_effect_attributes = inspect.getmembers(ChiperaseEffect, lambda a: not inspect.isroutine(a))
    chiperase_effect_values = []
    for attribute in chiperase_effect_attributes:
        # Builtin routines always starts and ends with double underscore (__)
        if not (attribute[0].startswith('__') and attribute[0].endswith('__')):
            # Only the attribute values are returned
            chiperase_effect_values.append(attribute[1])

    return chiperase_effect_values
