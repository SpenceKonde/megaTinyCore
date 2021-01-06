"""
Required device info for the PIC18F16Q40 devices
"""
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'name': 'pic18f16q40',
    'architecture': 'PIC18',
    'device_id': 0x75A0,
    # This device does not use an address as parameter for the bulk erase

    # Flash
    'flash_address_byte': 0,
    'flash_size_bytes': 64*1024,  # 64KB
    'flash_erase_size_words': 128,  # Sector erase
    'flash_page_size_words': 1,  # No page buffer
    'flash_write_size_words': 1,
    'flash_read_size_words': 1,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'flash_isolated_erase': False,

    # User ID
    'user_id_address_byte': 0x200000,
    'user_id_size_words': 32,
    'user_id_page_size_words': 1,
    'user_id_write_size_words': 1,
    'user_id_read_size_words': 1,
    'user_id_erase_address_byte': 0x300000,  # not used kept for compatibility
    'user_id_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'user_id_isolated_erase': False,

    # Config words
    'config_words_address_byte': 0x300000,
    'config_words_size_words': 5,
    'config_words_page_size_words': 1,
    'config_words_write_size_words': 1,
    'config_words_read_size_words': 1,
    'config_words_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'config_words_isolated_erase': False,

    # EEPROM
    'eeprom_address_byte': 0x380000,
    'eeprom_size_bytes': 512,
    'eeprom_page_size_bytes': 1,
    'eeprom_write_size_bytes': 1,
    'eeprom_read_size_bytes': 1,
    'eeprom_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'eeprom_isolated_erase': False,

    # ICD memory
    'icd_address_byte': 0x240000,
    'icd_size_bytes': 128*4*2, # 1KiB
    'icd_page_size_words': 128,# No page buffer so this is for sector erase
    'icd_write_size_words': 1,
    'icd_read_size_words': 1,
    'icd_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'icd_isolated_erase': False,
}
