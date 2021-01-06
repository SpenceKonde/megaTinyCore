#pylint: disable=too-few-public-methods
"""
Memory name definitions
"""

class MemoryNameAliases(object):
    """
    Memory names that are actually not real memories but an alias for several memories
    """
    ALL = 'all'

class MemoryNames(object):
    """
    Memory names corresponding to target device memories
    """
    # Real memories
    FLASH = 'flash'
    CONFIG_WORD = 'config_words'
    USER_ID = 'user_id'
    USER_ROW = 'user_row'
    EEPROM = 'eeprom'
    FUSES = 'fuses'
    CALIBRATION_ROW = 'calibration_row'
    ICD = 'icd'
    LOCKBITS = 'lockbits'
    SIGNATURES = 'signatures'
    INTERNAL_SRAM = 'internal_sram'

    @classmethod
    def get_all(cls):
        """
        Get a list of all memories representing actual device memories

        :return List of all memory names representing actual device memories
        """
        all_memories = []
        for attribute in dir(cls):
            if not attribute.startswith('__') and not callable(getattr(cls, attribute)):
                all_memories.append(getattr(cls, attribute))

        return all_memories
