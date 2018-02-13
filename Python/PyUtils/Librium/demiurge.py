#! /usr/bin/env python3

"""
The MIT License (MIT)

Copyright (c) 2015 Josh Bosley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

__author__ = 'Josh Bosley'

import os
import sys
import signal
import atexit

class Demiurge:
    """
                Python 3 Demiurge [Daemon]

    Responsible for the fashioning and maintenance of the physical universe
    [https://en.wikipedia.org/wiki/Demiurge]
    """

    def __init__(self, demiurge_name):
        self.demiurge_name = str(demiurge_name).replace(' ', '_').replace('/','_')
        self.pidfile = ('/tmp/' + demiurge_name + '.pid')
        self.stdout = ('/var/log/' + demiurge_name + '.log')
        self.stderr = ('/var/log/' + demiurge_name + '.log')
        self.stdin = '/dev/null'

    def demiurgeify(self):
        """
                Demiurge-ify class. UNIX double fork
        """

        # First fork
        try:
            if os.fork() > 0:
                raise SystemExit(0)
        except OSError as e:
            raise RuntimeError('Fork 1 Failed')

        os.chdir('/')
        os.umask(0)
        os.setsid()

        # Second Fork
        try:
            if os.fork() > 0:
                raise SystemExit(0)
        except OSError as e:
            raise RuntimeError('Fork 2 Failed')

        # Flush I/O
        sys.stdout.flush()
        sys.stderr.flush()

        with open(self.stdin, 'rb') as f:
            os.dup2(f.fileno(), sys.stdin.fileno())

        with open(self.stdout, 'ab') as f:
            os.dup2(f.fileno(), sys.stdout.fileno())

        with open(self.stderr, 'ab') as f:
            os.dup2(f.fileno(), sys.stderr.fileno())

        # Write PID
        with open(self.pidfile, 'w') as f:
            f.write(str(os.getpid()))

        # Specify at exit procedure
        atexit.register(self.clean)

        # Terminating signal handler
        def sigterm_handler(signo, frame):
            raise SystemExit(1)

        signal.signal(signal.SIGTERM, sigterm_handler)

    def clean(self):
        print('Bannished. [{}] scrubbed. '.format(self.pidfile))
        try:
            os.remove(self.pidfile)
        except Exception:
            print('Unable to remove [{}]'.format(self.pidfile))

    def summon(self):
        """
                Start the Demiurge
        """

        if os.path.exists(self.pidfile):
            raise RuntimeError('Your Demiurge is Alive.')

        self.demiurgeify()
        self.run()

    def banish(self):
        """
                Return the Demiurge to the pits of Nil
        """

        if os.path.exists(self.pidfile):
            with open(self.pidfile, 'r') as f:
                try:
                    pid = int(f.read().strip())
                except ValueError:
                    pid = None

                if not pid:
                    print('Could not terminate {} : The PID could not be read.'.format(self.demiurge_name))
                    raise SystemExit(1)
                else:
                    os.kill(pid, signal.SIGTERM)
        else:
            print('Your demiurge has not been summoned.', file=sys.stderr)
            raise SystemExit(1)

    def reset(self):
        """
                Send the Demiurge to the depths of obilivion, and summon its light once again
        """
        self.banish()
        self.clean()
        self.summon()

    def run(self):
        """
                Method to be overridden when making a subclass.
            It will be called after Demiurgifying an ephimeral process
        """
