#! /usr/bin/env python3

"""
    Example usage of Demiurge class
"""

__author__ = 'Josh Bosley'

import sys, time
from demiurge import Demiurge

class MyDemiurge(Demiurge):
    def run(self):
        while True:
            print("Demiurge is Alive!!")
            time.sleep(10)

def main():

    if len(sys.argv) != 2:
        print('Usage: {} [summon|banish|reset]'.format(sys.argv0[0]))
        return

    demiurge = MyDemiurge('Example-Demiurge')

    if 'summon' == sys.argv[1]:
        demiurge.summon()
    elif 'banish' == sys.argv[1]:
        demiurge.banish()
    elif 'reset' == sys.argv[1]:
        demiurge.reset()
    else:
        sys.exit(2)
    sys.exit(0)

if __name__ == '__main__':
    main()