'''
The MIT License (MIT)

Copyright (c) 2017 Josh Bosley

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
'''

from .send import send 
from .receive import serverThread

class SrhInterface:

    def __init__(self, settings=None, redirect=None):

        if redirect is None:
            self.redirect = self.defaultRedirect
        else:
            self.redirect = redirect

        if settings is None:
            self.settings = {
                "handshake_key": "grEkIoXgxaaIEJeCTnwieVO4zy97kgleF27ZWro0VJk8h8yweDeLKwQnf928IVlh4SWxBTX7hgQvgQHfIQgLnccCStydBpLHI80rOEbUVJGxKi9V5eG8fJSXmETB2IBJ",
                "encryption_key": "fJvR0EjHKT5n9KO5311H506iSacTZEXe",
                "network_transmission_timeout": 20,
                "verification_block": 1024,
                "accepted": "0x0000",
                "error": "0x0001",
                "address": "127.0.0.1",
                "port": 4096,
            }
        else:
            self.settings = settings

        print("SRH > Starting TCP server on port[", str(self.settings["port"]), "]")
        self.server_thread = serverThread(self.settings, self.redirect)
        self.server_thread.signal = True
        self.server_thread.daemon = True
        self.server_thread.start()

        self.sender = send(self.settings)
        return

    def send(self, address, port, message):
        return self.sender.out(address, port, message)

    def defaultRedirect(self, dummy, data):
        return ('''>>> Redirect is not yet setup.
>>> Include the method to handle incomming TCP connections by 
>>> passing it into SRH Interface.
>>> Data received: {0}
        '''.format(data))

    def generateNewKey(self):
        from random import SystemRandom, randint
        from string import ascii_letters, digits
        return ''.join(SystemRandom().choice(ascii_letters + digits) for _ in range(32))
        
    def kill(self):
        self.server_thread.signal = False
        self.send(self.settings["address"], self.settings["port"], "__HALT__")
        self.server_thread.join()
        
