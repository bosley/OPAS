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

import sqlite3
from json import loads as jloads
from json import dumps as jdumps
from datetime import datetime

class DataStore:

    def __init__(self):
        self.xnsdb = './xnsglobal/xSRH/xSRH.db'
        self.ensure_db()
        return

    def get_db(self, store):
        try:
            db = sqlite3.connect(store, timeout=15)
        except:
            print("Unable to open the data store : ", store)
            exit(1)
        return db

    def ensure_db(self):
        db = self.get_db(self.xnsdb)
        try:
            db.execute('''
                CREATE table logs (
                    date TEXT NOT NULL,
                    module TEXT NOT NULL,
                    message TEXT NOT NULL
                )
            ''')
            print("> Log table created")
        except:
            pass

        db.close()
        return

    def addLogEntry(self, log):
        
        if len(log) != 2:
            print("Error: Logging message. Invalid amount of information (module, message)")
            return False

        db = self.get_db(self.xnsdb)

        # Just to be sure strings
        log = [str(x) for x in log]

        stmt = '''INSERT INTO logs (date, module, message) VALUES ("{a}","{b}","{c}");'''.format(a=str(datetime.now()), b=log[0], c=log[1])
        db.execute(stmt)
        db.commit()
        db.close()
        return True
