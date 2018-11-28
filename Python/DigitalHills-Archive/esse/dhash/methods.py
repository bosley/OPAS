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

import hashlib
from random import randint
from settings import block_settings

'''
    Perform the hash for mining
'''
def hasher(nonce, *data):
    _hash = hashlib.sha256()
    _hash.update(str(nonce).encode('utf-8'))
    [_hash.update(x.encode('utf-8')) for x in list(data)]
    return _hash.hexdigest()

'''
    Block Miner
'''
def mine(data):
    from sys import stdout    
    print("")
    valid = False
    while not valid:
        nonce = randint(0, 2**256)
        _hash = hasher(nonce, data)

        stdout.write("\033[F")
        print("Hash result : ", _hash)

        if _hash.startswith(('0'*block_settings["_DIFFICULTY_"])):
            valid = True
    return(nonce, _hash) 
