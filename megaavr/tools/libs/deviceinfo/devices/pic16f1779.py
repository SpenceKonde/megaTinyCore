"""
Required device info for the PIC16F1779 devices
"""
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'name': 'pic16f1779',
    'architecture': 'PIC16',
    # Will erase Flash, User ID and Config words
    'default_bulk_erase_address_word': 0x8000,

    # Flash
    'flash_address_word': 0,
    'flash_size_words': 16*1024, # 16KW
    'flash_page_size_words': 32,
    'flash_write_size_words': 1,
    'flash_read_size_words': 1,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'flash_isolated_erase': False,

    # User ID
    'user_id_address_word': 0x8000,
    'user_id_size_words': 4,
    'user_id_page_size_words': 1,
    'user_id_write_size_words': 1,
    'user_id_read_size_words': 1,
    'user_id_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'user_id_isolated_erase': False,

    # Config words
    'config_words_address_word': 0x8007,
    'config_words_size_words': 2,
    'config_words_page_size_words': 1,
    'config_words_write_size_words': 1,
    'config_words_read_size_words': 1,
    'config_words_erase_address_word': 0,
    'config_words_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'config_words_isolated_erase': False,
}
