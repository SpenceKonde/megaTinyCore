"""
Pymcuprog specific exceptions
"""

class PymcuprogError(Exception):
    """
    Base class for all Pymcuprog specific exceptions
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogError, self).__init__(msg)
        self.code = code

class PymcuprogToolConfigurationError(PymcuprogError):
    """
    Signals that a tool was incorrectly configured
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogToolConfigurationError, self).__init__(msg)
        self.code = code

class PymcuprogToolConnectionError(PymcuprogError):
    """
    Signals that an attempted connect failed
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogToolConnectionError, self).__init__(msg)
        self.code = code

class PymcuprogNotSupportedError(PymcuprogError):
    """
    Signals that an attempted operation is not supported
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogNotSupportedError, self).__init__(msg)
        self.code = code

class PymcuprogSessionError(PymcuprogError):
    """
    Signals that a session is not active
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogSessionError, self).__init__(msg)
        self.code = code

class PymcuprogSessionConfigError(PymcuprogError):
    """
    Signals that a session is not configured correctly
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogSessionConfigError, self).__init__(msg)
        self.code = code


class PymcuprogDeviceLockedError(PymcuprogError):
    """
    Signals that the device is locked and a chip erase is required to unlock it
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogDeviceLockedError, self).__init__(msg)
        self.code = code

class PymcuprogEraseError(PymcuprogError):
    """
    Signals that an erase can't be executed

    Either the erase is not possible or the erase can't be executed without side effects,
    i.e. erasing more memories than requested
    """

    def __init__(self, msg=None, code=0):
        super(PymcuprogEraseError, self).__init__(msg)
        self.code = code
