"""
This subclass of pyserial's serial.Serial class is intended to work exactly
like the original, with the exception that the constructor has an extra argument
'open_timeout' (default = 30). The open() method will do the specified number
of retries to wait for a CDC serial port device to be created/accessible.
"""

from __future__ import print_function
from logging import getLogger
from time import sleep
from serial import Serial
from serial.serialutil import SerialException


class SerialCDC(Serial):
    """
    Serial class extension supporting open_timeout

    Desired definition is
    def __init__(self, \*args, open_timeout=30, \*\*kwargs):
    but that doesn't work with python2.
    """
    def __init__(self, *args, **kwargs):
        """
        Adding new parameter open_timeout
        """
        self.open_timeout = kwargs.pop("open_timeout", 30)
        self.logger = getLogger(__name__)

        # Port name is first positional arg, replace it with None to avoid
        # having it opened by parent's __init__()
        port = args[0] if args else None
        superargs = (None,) + args[1:]

        # Run parent's constructor, without any port to open
        # This must work with both python 2 and 3, hence the super() arguments
        super(SerialCDC, self).__init__(*superargs, **kwargs)

        # Now open the port if specified
        if port:
            self.port = port
            self.open()


    def open(self):
        """
        Overriding parent's open(), with timeout.
        """
        if self.open_timeout <= 0:
            # Just do parent's open and let any exception propagate to caller
            super(SerialCDC, self).open()
            return

        for to in reversed(range(self.open_timeout)):
            try:
                super(SerialCDC, self).open()
                return
            except SerialException as e:
                self.logger.info("Waiting for opening serial port %s (%d)", self.port, to)
                self.logger.debug(e)
                if to == 0:   # Propagate exception to caller when timed out
                    raise e
                sleep(1)
