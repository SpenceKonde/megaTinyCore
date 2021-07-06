"""
Link layer in UPDI protocol stack
"""
from logging import getLogger

from pymcuprog.pymcuprog_errors import PymcuprogError
from . import constants

class UpdiDatalink:
    """
    UPDI data link class handles the UPDI data protocol within the device
    """

    LDCS_RESPONSE_BYTES = 1

    def __init__(self):
        self.logger = getLogger(__name__)
        self.updi_phy = None

    def set_physical(self, physical):
        """
        Inject a serial-port based physical layer for use by this DL
        """
        self.updi_phy = physical

    def _init_session_parameters(self):
        """
        Set the inter-byte delay bit and disable collision detection
        """
        self.stcs(constants.UPDI_CS_CTRLB, 1 << constants.UPDI_CTRLB_CCDETDIS_BIT)
        self.stcs(constants.UPDI_CS_CTRLA, 0x06)

    def init_datalink(self):
        """
        Init DL layer
        """
        self._init_session_parameters()
        # Check
        if not self._check_datalink():
            # Send double break if all is not well, and re-check
            self.updi_phy.send_double_break()
            self._init_session_parameters()
            if not self._check_datalink():
                raise PymcuprogError("UPDI initialisation failed")

    def change_baud(self, baud):
        if self.updi_phy is not None:
            self.stcs(constants.UPDI_CS_CTRLA, 0x06)
            if baud <= 115200:
                self.stcs(constants.UPDI_ASI_CTRLA, 0x03)
            elif baud > 230400:
                self.stcs(constants.UPDI_ASI_CTRLA, 0x01)
            else:
                self.stcs(constants.UPDI_ASI_CTRLA, 0x02)
            self.updi_phy.change_baud(baud)

    def _check_datalink(self):
        """
        Check UPDI by loading CS STATUSA
        """
        try:
            if self.ldcs(constants.UPDI_CS_STATUSA) != 0:
                self.logger.info("UPDI init OK")
                return True
        except PymcuprogError:
            self.logger.warning("Check failed")
            return False
        self.logger.info("UPDI not OK - reinitialisation required")
        return False

    def ldcs(self, address):
        """
        Load data from Control/Status space
        :param address: address to load
        """
        self.logger.debug("LDCS from 0x%02X", address)
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_LDCS | (address & 0x0F)])
        response = self.updi_phy.receive(self.LDCS_RESPONSE_BYTES)
        numbytes_received = len(response)
        if numbytes_received != self.LDCS_RESPONSE_BYTES:
            raise PymcuprogError("Unexpected number of bytes in response: "
                                 "{} byte(s) expected {} byte(s)".format(numbytes_received, self.LDCS_RESPONSE_BYTES))

        return response[0]

    def stcs(self, address, value):
        """
        Store a value to Control/Status space
        :param address: address to store to
        :param value: value to write
        """
        self.logger.debug("STCS to 0x%02X", address)
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_STCS | (address & 0x0F), value])

    def ld_ptr_inc(self, size):
        """
        Loads a number of bytes from the pointer location with pointer post-increment
        :param size: number of bytes to load
        :return: values read
        """
        self.logger.debug("LD8 from ptr++")
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_LD | constants.UPDI_PTR_INC |
                            constants.UPDI_DATA_8])
        return self.updi_phy.receive(size)

    def ld_ptr_inc16(self, words):
        """
        Load a 16-bit word value from the pointer location with pointer post-increment.
        For improved performance of serialupdi for Arduino, send the REP instruction in the same command as LD
        :param words: number of words to load
        :return: values read
        """
        self.logger.debug("LD16 from ptr++")
        # combine REP, words with ld *ptr++
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_REPEAT | constants.UPDI_REPEAT_BYTE,
                            (words - 1) & 0xFF, constants.UPDI_PHY_SYNC, constants.UPDI_LD | constants.UPDI_PTR_INC |
                            constants.UPDI_DATA_16])
        return self.updi_phy.receive(words << 1)

    def st_ptr_inc(self, data):
        """
        Store data to the pointer location with pointer post-increment
        :param data: data to store
        """
        self.logger.debug("ST8 to *ptr++")
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_ST | constants.UPDI_PTR_INC | constants.UPDI_DATA_8,
                            data[0]])
        response = self.updi_phy.receive(1)

        if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
            raise PymcuprogError("ACK error with st_ptr_inc")

        num = 1
        while num < len(data):
            self.updi_phy.send([data[num]])
            response = self.updi_phy.receive(1)

            if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
                raise PymcuprogError("Error with st_ptr_inc")
            num += 1

    def st_ptr_inc16(self, data):
        """
        Store a 16-bit word value to the pointer location with pointer post-increment
        :param data: data to store
        """
        self.logger.debug("ST16 to *ptr++")
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_ST | constants.UPDI_PTR_INC |
                            constants.UPDI_DATA_16, data[0], data[1]])
        response = self.updi_phy.receive(1)

        if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
            raise PymcuprogError("ACK error with st_ptr_inc16")

        num = 2
        while num < len(data):
            self.updi_phy.send([data[num], data[num + 1]])
            response = self.updi_phy.receive(1)

            if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
                raise PymcuprogError("Error with st_ptr_inc16")
            num += 2

    def st_ptr_inc16_RSD(self, data, blocksize):
        """
        Store a 16-bit word value to the pointer location with pointer post-increment
        :param data: data to store
        :blocksize: max number of bytes being sent, None for all.
                    Warning: This does not strictly honor blocksize for values < 6
                    We always glob together the STCS(RSD) and REP commands.
                    But this should pose no problems for compatibility, because your serial adapter can't deal with 6b chunks,
                    none of pymcuprog would work!
        """
        self.logger.debug("ST16 to *ptr++ with RSD, data length: 0x%03X in blocks of:  %d", len(data), blocksize)

        #for performance we glob everything together into one USB transfer....
        repnumber= ((len(data) >> 1) -1)
        data = [*data, *[constants.UPDI_PHY_SYNC, constants.UPDI_STCS | constants.UPDI_CS_CTRLA, 0x06]]

        if blocksize is None:
            # Send whole thing at once stcs + repeat + st + (data + stcs)
            blocksize = 3 + 3 + 2 + len(data)
        num = 0
        firstpacket = []
        if blocksize < 10 :
            # very small block size - we send pair of 2-byte commands first.
            firstpacket = [*[constants.UPDI_PHY_SYNC, constants.UPDI_STCS | constants.UPDI_CS_CTRLA, 0x0E],
                            *[constants.UPDI_PHY_SYNC, constants.UPDI_REPEAT | constants.UPDI_REPEAT_BYTE, (repnumber & 0xFF)]]
            data = [*[constants.UPDI_PHY_SYNC, constants.UPDI_ST | constants.UPDI_PTR_INC |constants.UPDI_DATA_16], *data]
            num = 0
        else:
            firstpacket = [*[constants.UPDI_PHY_SYNC, constants.UPDI_STCS | constants.UPDI_CS_CTRLA , 0x0E],
                            *[constants.UPDI_PHY_SYNC, constants.UPDI_REPEAT | constants.UPDI_REPEAT_BYTE, (repnumber & 0xFF)],
                            *[constants.UPDI_PHY_SYNC, constants.UPDI_ST | constants.UPDI_PTR_INC | constants.UPDI_DATA_16],
                            *data[:blocksize - 8]]
            num = blocksize - 8
            if len(firstpacket) == 64 and blocksize != 64:
                firstpacket = firstpacket[:32]
                num = 32-8
                # workaround bug in D11C as serial adapter compiled with the USB implementation used in the mattairtech core;
                # this chokes on any block of exactly (!!) 64 bytes. Nobody seems to understand the reason for this bizzare issue.
                # The D11C as serial adapter is important because the fablab at MIT uses them heavilly, and wishes to upload Arduino sketches to tinyAVR 0/1/2-series
                # and AVR-Dx-series parts with them. It was desirable from their perspective to have an more accessible firmware (like that Arduino one) as opposed
                # to a pure-C one. Hence, a workaround is implemented to avoid triggering it. Thankfully, 64-byte blocks being written are not as natural as one might
                # expect for a power-of-two, since we combine the data with the commands on either side.
                # This workaround will only be invoked under unusual conditions, specifically if one of these is true:
                #   A. The last page of the hex file has a length of 53 bytes exactly OR
                #   B. The last page of the hex file has a length modulo the write chunk size, of 53 bytes exactly OR
                #   C. A write chunk is specified which is exactly 53 bytes shorter than the page OR
                #   D. The write chunk is specified as 153 when writing to a part with a page size of 512 OR
                #   E. Situations like D in the event that a future product permits REPEAT with 2 bytes of data to support pages larger than 512b.
                # Conditions C-E will invoke it on the last chunk of every page; For E, here are 6 cursed numbers for 1024b pages with 2 byte repeats, and 2 for 2048b pages
                # (in addition to pagelength - 52), however, there is no specific reason to expect such a part will be made. In any event, for those conditions, the
                # performance impact is on the order of 1-4ms per page, the same as the the penalty for each chunk that a write is divided into, hence the speed penalty of
                # using that write chunk size will be (1-4ms * ceil(page size/chunk size) + 1 instead of 1-4ms * ceil(page size/chunk size), which is always smaller than the
                # penalty already being accepted for the write chunking.
                # Conditions A and B will happen at most once per upload; thus the performance penalty would be acceptable even if they happened frequently. However
                # avr-gcc rarely, if ever, generates odd size hex files. I was not able to get it to by tweaking the sketch I was compiling, so they will occur on
                # considerably less than 1/64th of the time, if it is even possible for avr-gcc to generate such a file, which is unclear.
                # In summary, the performance impact is small but noticable in remote corner cases (where chunk size is chosen randomly, or intentionally in an effort to provoke
                # this bug, and is negligible (at worst 4 ms for 1 out of 32 uploads, assuming upload size modulo page size are evenly distributed and even ) in unlikely cases
                # (requiring an odd write chunk size such that B could be provoked from an even-length hex file) as well as -possibly- case A if there is indeed a programming
                # construct that results in a non-even length hex file. It is (barely) worth noting that for a hypothetical part that used a 2 byte REPEAT argument, this could
                # be provoked with an even sketch size, and so would impose a penalty of less than 4ms on 1 out of 32 uploads.
                # In light of the fact write chunking is intended as a workaround for an ill-mannered serial adapter in the first place (ex: HT42 workaround in DxCore), this
                # performance impact is not a concern.
                # If a write chunk size of 64 is specified, we will not activate this workaround.
                # -Spence 6/29/21
        self.updi_phy.send( firstpacket )
        # if finite block size, this is used.
        while num < len(data):
            data_slice = data[num:num+blocksize]
            if len(data_slice) == 64 and blocksize != 64:
                data_slice=data[num:num+32] # workaround as above.
            self.updi_phy.send(data_slice)
            num += len(data_slice)

    def repeat(self, repeats):
        """
        Store a value to the repeat counter
        :param repeats: number of repeats requested
        """
        self.logger.debug("Repeat %d", repeats)
        if (repeats - 1) > constants.UPDI_MAX_REPEAT_SIZE:
            self.logger.error("Invalid repeat count of %d", repeats)
            raise Exception("Invalid repeat count!")
        repeats -= 1
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_REPEAT | constants.UPDI_REPEAT_BYTE,
                            repeats & 0xFF])

    def read_sib(self):
        """
        Read the SIB
        """
        return self.updi_phy.sib()

    def key(self, size, key):
        """
        Write a key
        :param size: size of key (0=64B, 1=128B, 2=256B)
        :param key: key value
        """
        self.logger.debug("Writing key")
        if len(key) != 8 << size:
            raise PymcuprogError("Invalid KEY length!")
        self.updi_phy.send([constants.UPDI_PHY_SYNC, constants.UPDI_KEY | constants.UPDI_KEY_KEY | size])
        self.updi_phy.send(list(reversed(list(key))))

    def _st_data_phase(self, values):
        """
        Performs data phase of transaction:
        receive ACK
        send data
        :param values: bytearray of value(s) to send
        """
        response = self.updi_phy.receive(1)
        if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
            raise PymcuprogError("Error with st")

        self.updi_phy.send(values)
        response = self.updi_phy.receive(1)
        if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
            raise PymcuprogError("Error with st")


class UpdiDatalink16bit(UpdiDatalink):
    """
    UPDI data link layer in 16-bit version
    This means that all addresses and pointers contain 2 bytes
    """

    def __init__(self):
        UpdiDatalink.__init__(self)
        self.logger = getLogger(__name__)

    # pylint: disable=invalid-name
    def ld(self, address):
        """
        Load a single byte direct from a 16-bit address
        :param address: address to load from
        :return: value read
        """
        self.logger.info("LD from 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_LDS | constants.UPDI_ADDRESS_16 | constants.UPDI_DATA_8,
             address & 0xFF, (address >> 8) & 0xFF])
        return self.updi_phy.receive(1)[0]

    def ld16(self, address):
        """
        Load a 16-bit word directly from a 16-bit address
        :param address: address to load from
        :return: values read
        """
        self.logger.info("LD from 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_LDS | constants.UPDI_ADDRESS_16 | constants.UPDI_DATA_16,
             address & 0xFF, (address >> 8) & 0xFF])
        return self.updi_phy.receive(2)

    # pylint: disable=invalid-name
    def st(self, address, value):
        """
        Store a single byte value directly to a 16-bit address
        :param address: address to write to
        :param value: value to write
        """
        self.logger.info("ST to 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_STS | constants.UPDI_ADDRESS_16 | constants.UPDI_DATA_8,
             address & 0xFF, (address >> 8) & 0xFF])
        return self._st_data_phase([value & 0xFF])

    def st16(self, address, value):
        """
        Store a 16-bit word value directly to a 16-bit address
        :param address: address to write to
        :param value: value to write
        """
        self.logger.info("ST to 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_STS | constants.UPDI_ADDRESS_16 | constants.UPDI_DATA_16,
             address & 0xFF, (address >> 8) & 0xFF])
        return self._st_data_phase([value & 0xFF, (value >> 8) & 0xFF])

    def st_ptr(self, address):
        """
        Set the pointer location
        :param address: address to write
        """
        self.logger.info("ST to ptr")
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_ST | constants.UPDI_PTR_ADDRESS | constants.UPDI_DATA_16,
             address & 0xFF, (address >> 8) & 0xFF])
        response = self.updi_phy.receive(1)
        if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
            raise PymcuprogError("Error with st_ptr")


class UpdiDatalink24bit(UpdiDatalink):
    """
    UPDI data link layer in 24-bit version
    This means that all addresses and pointers contain 3 bytes
    """

    def __init__(self):
        UpdiDatalink.__init__(self)
        self.logger = getLogger(__name__)

    # pylint: disable=invalid-name
    def ld(self, address):
        """
        Load a single byte direct from a 24-bit address
        :param address: address to load from
        :return: value read
        """
        self.logger.info("LD from 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_LDS | constants.UPDI_ADDRESS_24 | constants.UPDI_DATA_8,
             address & 0xFF, (address >> 8) & 0xFF, (address >> 16) & 0xFF])
        return self.updi_phy.receive(1)[0]

    def ld16(self, address):
        """
        Load a 16-bit word directly from a 24-bit address
        :param address: address to load from
        :return: values read
        """
        self.logger.info("LD from 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_LDS | constants.UPDI_ADDRESS_24 | constants.UPDI_DATA_16,
             address & 0xFF, (address >> 8) & 0xFF, (address >> 16) & 0xFF])
        return self.updi_phy.receive(2)

    # pylint: disable=invalid-name
    def st(self, address, value):
        """
        Store a single byte value directly to a 24-bit address
        :param address: address to write to
        :param value: value to write
        """
        self.logger.info("ST to 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_STS | constants.UPDI_ADDRESS_24 | constants.UPDI_DATA_8,
             address & 0xFF, (address >> 8) & 0xFF, (address >> 16) & 0xFF])
        return self._st_data_phase([value & 0xFF])

    def st16(self, address, value):
        """
        Store a 16-bit word value directly to a 24-bit address
        :param address: address to write to
        :param value: value to write
        """
        self.logger.info("ST to 0x{0:06X}".format(address))
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_STS | constants.UPDI_ADDRESS_24 | constants.UPDI_DATA_16,
             address & 0xFF, (address >> 8) & 0xFF, (address >> 16) & 0xFF])
        return self._st_data_phase([value & 0xFF, (value >> 8) & 0xFF])

    def st_ptr(self, address):
        """
        Set the pointer location
        :param address: address to write
        """
        self.logger.info("ST to ptr")
        self.updi_phy.send(
            [constants.UPDI_PHY_SYNC, constants.UPDI_ST | constants.UPDI_PTR_ADDRESS | constants.UPDI_DATA_24,
             address & 0xFF, (address >> 8) & 0xFF, (address >> 16) & 0xFF])
        response = self.updi_phy.receive(1)
        if len(response) != 1 or response[0] != constants.UPDI_PHY_ACK:
            raise PymcuprogError("Error with st_ptr")
