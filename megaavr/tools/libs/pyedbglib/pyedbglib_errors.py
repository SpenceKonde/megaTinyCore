"""
pyedbglib specific exceptions
"""

class PyedbglibError(Exception):
    """
    Base class for all pyedbglib specific exceptions
    """

    def __init__(self, msg=None, code=0):
        super(PyedbglibError, self).__init__(msg)
        self.code = code

class PyedbglibNotSupportedError(PyedbglibError):
    """
    Signals that an attempted operation is not supported
    """

    def __init__(self, msg=None, code=0):
        super(PyedbglibNotSupportedError, self).__init__(msg)
        self.code = code
