"""
Device Specific Classes which use AVR32Protocol implementation
"""
from pyedbglib.protocols.avr32protocol import Avr32Protocol

class Avr32Device(object):
    """
    Generic AVR32 device wrapper (maps to avr32 protocol)
    """
    def __init__(self, transport, reset_domains=5):
        """
        :param transport: Transport layer object
        :param reset_domains: Number of bits in the RESET register.
            This number is device dependant
        """
        self.protocol = Avr32Protocol(transport)
        self.interface = Avr32Protocol.AVR32_PHY_INTF_JTAG
        self.reset_domains = reset_domains

    def setup_prog_session(self, interface="jtag"):
        """
        Setup a programming session on AVR32

        :param interface: jtag or awire
        :return:
        """
        if interface == "awire":
            self.interface = Avr32Protocol.AVR32_PHY_INTF_AWIRE
        else:
            self.interface = Avr32Protocol.AVR32_PHY_INTF_JTAG
            # Set clock frequency
            self.protocol.set_le16(Avr32Protocol.AVR32_CONTEXT_PHYSICAL, Avr32Protocol.AVR32_PHYSICAL_JTAG_CLOCK, 8000)

        # Set interface
        self.protocol.set_byte(Avr32Protocol.AVR32_CONTEXT_PHYSICAL, Avr32Protocol.AVR32_PHYSICAL_PHYSICAL,
                               self.interface)

        # Set number of reset domains
        self.protocol.set_byte(Avr32Protocol.AVR32_CONTEXT_DEVICE, Avr32Protocol.AVR32_RESET_DOMAINS,
                               self.reset_domains)

    def activate_physical(self):
        """
        Activate the physical interface
        """
        return self.protocol.activate_physical()

    def deactivate_physical(self):
        """
        Deactivate the physical interface
        """
        return self.protocol.deactivate_physical()
