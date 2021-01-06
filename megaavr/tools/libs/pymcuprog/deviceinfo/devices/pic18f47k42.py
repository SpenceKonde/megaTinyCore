"""
Required device info for the PIC18F47K42 devices
"""
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'name': 'pic18f47k42',
    'architecture': 'PIC18',
    # Will erase Flash, User ID, Config words and EEPROM (if CPD_bar config bit is disabled)
    'default_bulk_erase_address_byte': 0x300000,

    # Flash
    'flash_address_byte': 0,
    'flash_size_bytes': 128*1024,  # 128KB
    'flash_page_size_bytes': 128,
    'flash_write_size_words': 1,
    'flash_read_size_words': 1,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'flash_isolated_erase': False,

    # User ID
    'user_id_address_byte': 0x200000,
    'user_id_size_words': 8,
    'user_id_page_size_words': 1,
    'user_id_write_size_words': 1,
    'user_id_read_size_words': 1,
    'user_id_erase_address_byte': 0x300000,
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
    'eeprom_address_byte': 0x310000,
    'eeprom_size_bytes': 1024,
    'eeprom_page_size_bytes': 1,
    'eeprom_write_size_bytes': 1,
    'eeprom_read_size_bytes': 1,
    'eeprom_chiperase_effect': ChiperaseEffect.CONDITIONALLY_ERASED_PIC,
    'eeprom_isolated_erase': True,
}
