#!/usr/bin/env python
#
from __future__ import print_function

import sys
import math
import unittest
from datetime import datetime
from datetime import timedelta
import time

sys.path.insert(0, '..')
import __init__ as pause

class TestPauseFor(unittest.TestCase):
    """ Test the delay function of PauseFor """

    def setUp(self):
        pass

    def test_past(self):
        """ test_past
        Test a time that has already passed
        """

        # Using seconds()
        start = time.time()
        pause.seconds(-5)
        end = time.time()
        self.assertEqual(int(end - start), 0)

        # Using until()
        start = time.time()
        pause.until(time.time() - 10)
        end = time.time()
        self.assertEqual(int(end - start), 0)

    def test_milliseconds(self):
        """ test_milliseconds
        Test 500 millisecond delay
        """
        start = time.time()
        pause.milliseconds(500)
        end = time.time()
        diff = end - start
        target = abs(diff - 0.5)

        #
        # True if it's within 0.1 of the target time
        #
        print('Milliseconds came within {0} seconds of 0.5'.format(target))
        valid = (target <= 0.1)
        self.assertTrue(valid)

    def test_seconds(self):
        """ test_seconds
        Test 5 second delay
        """
        now = time.time()
        pause.seconds(5)
        end = time.time()

        # True if 5 seconds have passed
        diff = int(end - now)
        self.assertEqual(diff, 5)

    def test_time(self):
        """ test_time
        Test 5 second delay
        """
        now = time.time()
        pause.seconds(5)
        end = time.time()

        # True if 5 seconds have passed
        diff = int(end - now)
        self.assertEqual(diff, 5)

    def test_minutes(self):
        """ test_minutes
        Test 1 minute delay
        """
        now = time.time()
        pause.minutes(1)
        end = time.time()

        # True if 1 minute has passed
        diff = int((end - now) / 60)
        self.assertEqual(diff, 1)

    def test_weeks(self):
        """ test_weeks
        Use weeks to pause for 2 seconds.
        This should effectively test days() and hours(), since the weeks() goes through both of those functions.
        """
        now = time.time()
        pause.weeks(float((1.0 / 7.0 / 24.0 / 60.0 / 60.0) * 2.0))
        end = time.time()

        # True if 2 seconds has passed
        diff = int(end - now)
        self.assertEqual(diff, 2)

    def test_datetime(self):
        """ test_datetime
        Test 7 seconds, with a datetime object
        """
        startDate = datetime.now()
        toDate = startDate + timedelta(seconds=7)
        pause.until(toDate)
        now = datetime.now()

        # True if at least 7 seconds has passed
        diff = now - startDate
        self.assertEqual(diff.seconds, 7)

    def test_timezone(self):
        """ test_datetime
        Test 7 seconds, with a datetime object
        """
        if sys.version_info[0] >= 3:
            from datetime import timezone
			# Apply a timezone offset, Line Islands Time for fun
            startDate = datetime.now(timezone(timedelta(hours=14), 'LINT'))
            toDate = startDate + timedelta(seconds=7)
            pause.until(toDate)
            now = datetime.now(timezone.utc)

            # True if at least 7 seconds has passed
            diff = now - startDate
            self.assertEqual(diff.seconds, 7)

    def test_timestamp(self):
        """ test_timestamp
        Test 6 seconds, with a unix timestamp
        """
        toTime = time.time() + 6
        start = time.time()
        pause.until(toTime)

        # True if it came within 0.1 of a second
        end = time.time()
        diff = int(end - start)
        self.assertEqual(diff, 6)

if __name__ == '__main__':
    unittest.main()