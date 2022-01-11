#!/usr/bin/env python

"""
Suspend the execution of your program for a given amount of time.
This works similarly to ``time.sleep``, but uses your computers timestamp to track time, versus a counter.
"""

"""
The MIT License (MIT)

Copyright (c) 2013 Jeremy Gillick

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
"""

import sys
from datetime import datetime
import time as pytime
from time import sleep
if sys.version_info[0] >= 3:
            from datetime import timezone

def until(time):
    """
    Pause your program until a specific end time.
    'time' is either a valid datetime object or unix timestamp in seconds (i.e. seconds since Unix epoch)
    """
    end = time

    # Convert datetime to unix timestamp and adjust for locality
    if isinstance(time, datetime):
        # If we're on Python 3 and the user specified a timezone, convert to UTC and get tje timestamp.
        if sys.version_info[0] >= 3 and time.tzinfo:
            end = time.astimezone(timezone.utc).timestamp()
        else:
            zoneDiff = pytime.time() - (datetime.now()- datetime(1970, 1, 1)).total_seconds()
            end = (time - datetime(1970, 1, 1)).total_seconds() + zoneDiff

    # Type check
    if not isinstance(end, (int, float)):
        raise Exception('The time parameter is not a number or datetime object')

    # Now we wait
    while True:
        now = pytime.time()
        diff = end - now

        #
        # Time is up!
        #
        if diff <= 0:
            break
       # else:
            # 'logarithmic' sleeping to minimize loop iterations
            # sleep(diff / 2)


def milliseconds(num):
    """
    Pause for this many milliseconds
    """
    seconds(num / 1000.0)


def seconds(num):
    """
    Pause for this many seconds
    """
    now = pytime.time()
    end = now + num
    until(end)


def time(num):
    """
    Same as PauseFor.seconds()
    """
    seconds(num)


def minutes(num):
    """
    Pause for this many minutes
    """
    seconds(60 * num)


def hours(num):
    """
    Pause for this many hours
    """
    minutes(60 * num)


def days(num):
    """
    Pause for this many days
    """
    hours(24 * num)


def weeks(num):
    """
    Pause for this many weeks
    """
    days(7 * num)
