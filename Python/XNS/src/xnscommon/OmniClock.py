from sys import exit
from datetime import datetime

from xnsdisplay import cout

class OmniClock:

    def __init__(self):
        self.clock_start = datetime.now()
        self.counters = {}

    '''          Tell if counters have been made           '''
    def hasCounters(self):
        if len(self.counters.keys()) > 0:
            return True
        return False

    '''         Create a new counter                       '''
    def newCounter(self, title):
        for x in self.counters.keys():
            if title == x:
                cout("fail", "OmniClock->newCounter : Counters must be uniques in their title!")
                exit(1)
        self.counters[title] = (len(self.counters), datetime.now())

    def newCounterFromDateTime(self, title, dTime):
        for x in self.counters.keys():
            if title == x:
                cout("fail", "OmniClock->newCounter : Counters must be uniques in their title!")
                exit(1)
        self.counters[title] = (len(self.counters), dTime)
        

    '''         Convert a time delta to minutes, seconds    '''
    def timeConversion(self, timeDelta):
        return divmod(timeDelta.days * 86400 + timeDelta.seconds, 60)

    '''         Calculate the time delta of two clocks      '''
    def timeDelta(self, clock_1, clock_2):
        try:
            c1 = self.counters[clock_1]
            c2 = self.counters[clock_2]
        except KeyError:
            cout("fail", "OmniClock->timeDelta : An input clock doesn't exist!")
            exit(1)

        td = abs(c1[1] - c2[1])
        return td

    '''         Determine the minutes between two clocks   '''
    def minutesDelta(self, clock_1, clock_2):
        return self.timeConversion(self.timeDelta(clock_1, clock_2))[0]

    '''         Determine the seconds between two clocks   '''
    def secondsDelta(self, clock_1, clock_2):
        return self.timeConversion(self.timeDelta(clock_1, clock_2))[1]

    '''         Determine how many minutes since creation  '''
    def minutesPassed(self, clock):
        self.newCounter("__OMNI_CLOCK_SELF_SET__")
        m = self.minutesDelta("__OMNI_CLOCK_SELF_SET__", clock)
        del self.counters["__OMNI_CLOCK_SELF_SET__"]

      #  print("Minutes passed : ", m)

        return m

    '''         Determine how many seconds since creation  '''
    def secondsPassed(self, clock):
        self.newCounter("__OMNI_CLOCK_SELF_SET__")
        s = self.secondsDelta("__OMNI_CLOCK_SELF_SET__", clock)
        del self.counters["__OMNI_CLOCK_SELF_SET__"]
        return s

    '''         Convert a string from a datetime and send  '''
    def minutesPassedFromString(self, tstring, format):
        self.newCounter("__OMNI_CLOCK_SELF_SET__0")
        self.newCounterFromDateTime("__OMNI_CLOCK_SELF_SET__", datetime.strptime(tstring, format))
        m = self.minutesDelta("__OMNI_CLOCK_SELF_SET__0", "__OMNI_CLOCK_SELF_SET__")
        del self.counters["__OMNI_CLOCK_SELF_SET__"]
        del self.counters["__OMNI_CLOCK_SELF_SET__0"]
        return m


    '''         Convert a string from a datetime and send  '''
    def secondsPassedFromString(self, tstring, format):
        self.newCounterFromDateTime("__OMNI_CLOCK_SELF_SET__", datetime.strptime(tstring, format))
        s = self.secondsDelta("__OMNI_CLOCK_SELF_SET__", "__OMNI_CLOCK_SELF_SET__")
        del self.counters["__OMNI_CLOCK_SELF_SET__"]
        del self.counters["__OMNI_CLOCK_SELF_SET__0"]
        return s

    '''         Remove a clock from the counters list      '''
    def removeClock(self, clock):
        del self.counters[clock]
        
    '''         Reset a clock to .now()                    '''
    def resetClock(self, clock):
        try:
            self.counters[clock] = (len(self.counters), datetime.now())
        except KeyError:
            cout("fail", "OmniClock->resetClock : Invalid input clock")
            exit(1)

    