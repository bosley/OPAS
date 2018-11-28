'''
    Copyright 2017 Digital Hills, LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
'''

from dhash import hasher
from datetime import datetime
from settings import block_settings
from json import dumps

typeTrans = {
    "ONE2ONE": 0,
    "ONE2MANY": 1
}

class transaction:
    def __init__(self, _txType, _txData, _key, _sig, _timestamp=None, _txHash=None):
        
        '''
             tx_data = {
                "from": _from,
                "to": _to,
                "amount": _amount,
            }
        '''
        self.txType = _txType
        self.txData = _txData
        if _timestamp is None:
            self.timestamp = str(datetime.now())
        else:
            self.timestamp = str(_timestamp)
        if _txHash is None:
            self.txId = hasher(block_settings["_TX_SEED_"], _txData, self.timestamp)
        else:
            self.txId = _txHash
        self.pkey = _key
        self.sig = _sig

    def __str__(self):
        return '''

    TX ID:\t {}\n
    TX TYPE:\t {}\n
    TX KEY:\t {}\n
    TX SIG:\t {}\n
    TX DATA:\t {}\n

    '''.format(self.txId, self.txType, self.pkey, self.sig, self.txData )

    def json(self):
        return dumps({
            "id": self.txId,
            "type": self.txType,
            "key": self.pkey,
            "sig": self.sig,
            "time": self.timestamp,
            "data": self.txData
        })