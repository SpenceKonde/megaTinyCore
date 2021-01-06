"""
Required device info for the PIC24FJ128GA705 devices
"""
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'name': 'pic24fj128ga705',
    'device_id': '750B',
    'architecture': 'PIC24',
    'interface' : 'icsp',

    # Flash
    'flash_address_byte': 0,
    'flash_size_words': 0x16000,  # 44K * 1024 * 2
    'flash_page_size_bytes': 512,
    'flash_write_size_bytes': 16,
    'flash_read_size_bytes': 16,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    # Configuration words are integrated in the flash, but since they are not represented
    # as a separate memory it is correct to state that flash can be erased in isolation
    'flash_isolated_erase': True,

    # ICD memory
    'icd_address_byte': 0x800000 * 2,
    'icd_size_bytes': 512 * 4,  # 2KiB
    'icd_page_size_bytes': 512,
    'icd_write_size_bytes': 16,
    'icd_read_size_bytes': 16,
    'icd_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'icd_isolated_erase': True,
}
