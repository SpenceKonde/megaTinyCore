"""
Required device info for the ATSAMD21E18A devices
"""
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'name': 'atsamd21e18a',
    'architecture': 'CORTEX-M0PLUS',

    # Flash
    'flash_address_byte': 0,
    'flash_size_bytes': 0x40000,
    'flash_page_size_bytes': 0x40,
    'flash_write_size_bytes': 1,
    'flash_read_size_bytes': 1,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'flash_isolated_erase': False,

    # user_row
    'user_row_address_byte': 0x00804000,
    'user_row_size_bytes': 256,
    'user_row_page_size_bytes': 64,
    'user_row_read_size_bytes': 1,
    'user_row_write_size_bytes': 4,
    'user_row_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'user_row_isolated_erase': True,

    # calibration data AKA OTP4
    'calibration_row_address_byte': 0x00806020,
    'calibration_row_size_bytes': 64,
    'calibration_row_page_size_bytes': 64,
    'calibration_row_read_size_bytes': 1,
    'calibration_row_write_size_bytes': 0,
    'calibration_row_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'calibration_row_isolated_erase': False,
}
