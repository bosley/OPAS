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

from json import dumps
from .interface import SrhInterface

# Custom handler must have a input of settings, and data
def customHandler(settings, data):
    print("Custom handler got : ", data, "\nUsing settings :\n")
    print(dumps(settings, indent=4))

    # Custom handler must always return a string! 
    return "__RESULT__"

# Perform verbose tests
def performTests():
        

        '''

                Example of the default behaviour 

        '''
        print("\n\tDefault handler: \n")

        si = SrhInterface()

        result = si.send("127.0.0.1", 4096, "<<TEST DATA>>")

        print(result)

        print("\n\tKilling.. this might take a moment.. ")
        si.kill()
        del si


        '''

                Example of the customized handler 

        '''

        print("\n\tCustomized handler : \n")

        # Default settings, with a custom handler
        si = SrhInterface(settings=None, redirect=customHandler)

        result = si.send("127.0.0.1", 4096, "<<TEST DATA>>")

        print(result)

        print("\n\tKilling.. this might take a moment.. ")
        si.kill



        '''

                Example of a fully customized SRH

        '''

        print("\n\tFully customized : \n")

        # Ensure that all keys and values are present
        # Handshake key can be any length, the encryption key must be len(32)
        customSettings = {
        "handshake_key": "mdyfgskCYpRLHwJPaSUPXFoAExsQo2Yojk4YhbzgeURHmOHerypQPZuiJLg7JHVFm2VswfB8etbLTlBs5Hh2DiVZFbInim9aB4Jx59rh1WB9tIGKAg7k1fI8UD0bb4J4",
        "encryption_key": "1e4ZgSLUcMOhNgWDMOGnRfln0JSlQM29",
        "network_transmission_timeout": 20,
        "verification_block": 1024,
        "accepted": "0x0000",
        "error": "0x0001",
        "address": "127.0.0.1",
        "port": 8888,
        }

        # Default settings, with a custom handler
        si = SrhInterface(settings=customSettings, redirect=customHandler)

        result = si.send("127.0.0.1", 8888, "<<TEST DATA>>")

        print(result)

        print("\n\tKilling.. this might take a moment.. ")
        si.kill

        '''

                Example of key generation

        '''

        print('''

        To make things easier, the interface class has a key generator. 
        The key it generates needs to be manually placed into the settings dictionary,
        and the server reset.

        ''')

        print("\t", si.generateNewKey())