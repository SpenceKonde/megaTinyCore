
"""
Required device info for the avr64du32 devices
The following data was collected from device pack Microchip.AVR-DU_DEV_DFP 1.0.49
"""
"""
Spence: Note that Microchip.AVR-DU_DEV_DFP 1.0.49 doesn't appear to be publically
available - this file came from the pymcuprog repo, implying that internal testing
and development was ongoing, the first sign of life from DU since the product brief
had a brief moment of availability before vanishing. It was later confirmed that
development is ongoing, and this Microchip employee indicated that they recognize
the importance of bringing a modern AVR with native USB to market in a timely manner.

Now - where are them 64 bytes of "boot row"? I don't see them in the memory list.
This may have been an artifact of a naive script, though I imagine that they were
fully aware that it wasn't being included here, but they weren't testing that yet.
(This could also be strategically holding back information - the DB headers were
available before the DB shipped - with only the heading of the OPAMP section, and
a few bitfield names that snuck through (or were left in as teasers). (I'd say, oh
2:1 odds in favor of it being a script artifact (and >90% chance they knew and did
not need it yet)).
"""

from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
    'interface': 'UPDI',
    'name': 'avr64du32',
    'architecture': 'avr8x',

    # eeprom
    'eeprom_address_byte': 0x1400,
    'eeprom_size_bytes': 0x100,
    'eeprom_page_size_bytes': 0x1,
    'eeprom_read_size_bytes': 0x01,
    'eeprom_write_size_bytes': 0x01,
    'eeprom_chiperase_effect': ChiperaseEffect.CONDITIONALLY_ERASED_AVR,
    'eeprom_isolated_erase': True,

    # flash
    'flash_address_byte': 0x00800000,
    'flash_size_bytes': 0x10000,
    'flash_page_size_bytes': 0x200,
    'flash_read_size_bytes': 0x02,
    'flash_write_size_bytes': 0x200,
    'flash_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'flash_isolated_erase': True,

    # fuses
    'fuses_address_byte': 0x1050,
    'fuses_size_bytes': 0x09,
    'fuses_page_size_bytes': 0x01,
    'fuses_read_size_bytes': 0x01,
    'fuses_write_size_bytes': 0x01,
    'fuses_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'fuses_isolated_erase': False,

    # internal_sram
    'internal_sram_address_byte': 0x6000,
    'internal_sram_size_bytes': 0x2000,
    'internal_sram_page_size_bytes': 0x01,
    'internal_sram_read_size_bytes': 0x01,
    'internal_sram_write_size_bytes': 0x01,
    'internal_sram_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'internal_sram_isolated_erase': False,

    # lockbits
    'lockbits_address_byte': 0x1040,
    'lockbits_size_bytes': 0x4,
    'lockbits_page_size_bytes': 0x01,
    'lockbits_read_size_bytes': 0x01,
    'lockbits_write_size_bytes': 0x01,
    'lockbits_chiperase_effect': ChiperaseEffect.ALWAYS_ERASED,
    'lockbits_isolated_erase': False,

    # signatures
    'signatures_address_byte': 0x1080,
    'signatures_size_bytes': 0x40,
    'signatures_page_size_bytes': 0x01,
    'signatures_read_size_bytes': 0x01,
    'signatures_write_size_bytes': 0x00,
    'signatures_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'signatures_isolated_erase': False,

    # user_row
    'user_row_address_byte': 0x1200,
    'user_row_size_bytes': 0x200,
    'user_row_page_size_bytes': 0x200,
    'user_row_read_size_bytes': 0x01,
    'user_row_write_size_bytes': 0x01,
    'user_row_chiperase_effect': ChiperaseEffect.NOT_ERASED,
    'user_row_isolated_erase': True,

    # Some extra AVR specific fields
    'nvmctrl_base': 0x00001000,
    'syscfg_base': 0x00000F00,
    'ocd_base': 0x00000F80,
    'address_size': '24-bit',
    'prog_clock_khz': 1800,
    'hv_implementation': 2,
    'device_id': 0x1E9621,

}
