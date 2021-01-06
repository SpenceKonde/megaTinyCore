"""Generating string representations of variables for nice printouts"""

def bytelist_to_hex_string(bytelist):
    """
    :param bytelist: list of byte values
    :return: String representation of the bytelist with each item as a byte value on the format 0xXX
    """
    return '[' + ', '.join("0x%02X" % x for x in bytelist) + ']'
