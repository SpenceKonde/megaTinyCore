"""
Required device info for the ATtiny84A devices
"""
from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'name': 'attiny84a',
    'architecture': 'avr8',

    # Flash
    'flash_address_byte': 0,
    'flash_size_bytes': 0x2000,
    'flash_page_size_bytes': 0x40,
    'flash_write_size_bytes': 2,
    'flash_read_size_bytes': 2,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'flash_isolated_erase': False,

    # signatures
    'signatures_address_byte': 0,
    'signatures_size_bytes': 3,
    'signatures_page_size_bytes': 1,
    'signatures_read_size_bytes': 1,
    'signatures_write_size_bytes': 0,
    'signatures_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'signatures_isolated_erase': False,

    # calibration
    'calibration_row_address_byte': 0,
    'calibration_row_size_bytes': 1,
    'calibration_row_page_size_bytes': 1,
    'calibration_row_read_size_bytes': 1,
    'calibration_row_write_size_bytes': 1,
    'calibration_row_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'calibration_row_isolated_erase': False,

    # fuses
    'fuses_address_byte': 0,
    'fuses_size_bytes': 0x0003,
    'fuses_page_size_bytes': 1,
    'fuses_read_size_bytes': 1,
    'fuses_write_size_bytes': 1,
    'fuses_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'fuses_isolated_erase': False,

    # lockbits
    'lockbits_address_byte': 0,
    'lockbits_size_bytes': 0x0001,
    'lockbits_page_size_bytes': 1,
    'lockbits_write_size_bytes': 1,
    'lockbits_read_size_bytes': 1,
    'lockbits_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'lockbits_isolated_erase': False,

    # eeprom
    'eeprom_address_byte': 0x0000,
    'eeprom_size_bytes': 0x0200,
    'eeprom_page_size_bytes': 0x04,
    'eeprom_read_size_bytes': 1,
    'eeprom_write_size_bytes': 1,
    'eeprom_chiperase_effect': ChiperaseEffect.CONDITIONALLY_ERASED_AVR,
    'eeprom_isolated_erase': False,

    # Some extra AVR specific fields
    'interface': 'ISP',
    'device_id': 0x1E930C,
}
