"""
Required device info for the EDBG / UC3A3 devices
"""
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'name': 'uc3a3256',
    'architecture': 'avr32',
    'reset_domains': 5,

    # Flash
    'flash_address_byte': 0,
    'flash_size_bytes': 0x40000,
    'flash_page_size_bytes': 512,
    'flash_write_size_bytes': 1,
    'flash_read_size_bytes': 1,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'flash_isolated_erase': False,
}
