"""
Simple timer helper for UPDI stack
"""
import time

#pylint: disable=too-few-public-methods
class Timeout:
    """
    Simple timeout helper in milliseconds.
    """

    def __init__(self, timeout_ms):
        """
        Start the expired counter instantly
        :param timeout_ms: milliseconds to count
        """

        self.timeout_ms = timeout_ms
        self.start_time = time.time()

    def expired(self):
        """
        Check if the timeout has expired
        """
        return time.time() - self.start_time > self.timeout_ms / 1000.0
