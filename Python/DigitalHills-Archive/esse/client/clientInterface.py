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

'''
    This file is meant to give access to commonly used functions
    in things like address / tx generators 
'''

import hashlib
import logging
from random import randint
from ecdsa import SigningKey, VerifyingKey, BadSignatureError
from display import cout
from settings import block_settings
from settings import node_settings
from tcplib import query
from chain import transaction, typeTrans
from json import dumps

__PUSH_ATTEMPTS__ = 20

logger = logging.getLogger(__name__)

'''
    Hashes used in address creation
'''
def hashMe(text, which):
    if which == 256:
        h = hashlib.sha256()
    elif which == 512:
        h = hashlib.sha512()
    elif which == 160:
        h = hashlib.new('ripemd160')
    else:
        cout("fail", "Internal hashMe error")
        return "_ERROR_"
    h.update(text)
    return h.hexdigest()

'''
    Verify a signature
'''
def verify(publicKey, sig, tx):
    vk  = VerifyingKey.from_pem(publicKey)
    try:
        vk.verify(sig, tx)
        return True
    except BadSignatureError:
        return False
    

'''
    Generate an address
'''
def generate_address():
    privateKey = SigningKey.generate()
    publicKey = privateKey.get_verifying_key()
    round_one = (hashMe(publicKey.to_string(), 256))
    round_two = (hashMe(round_one.encode('utf-8'), 160))
    network = node_settings["network"] + "."
    address = network + round_two
    checksum =  hashMe(
        hashMe(
            address.encode('utf-8'), 512).encode('utf-8')
            , 256)
    address = address + "." + checksum[-8:]
    return (publicKey.to_pem(), privateKey.to_pem(), address)

'''
    Generate a transaction
'''
def generate_transaction(_from, _to, _amount, _pubkey, _privkey):
    if not isinstance(_from, str):
        return (False, "_ERROR_[FROM]_MUST_BE_STR_TYPE_")
    if not isinstance(_to, list):
        return (False, "_ERROR_[TO]_MUST_BE_LIST_TYPE_")
    else:
        for x in _to:
            if not isinstance(x, str):
                return(False, "_ERROR_[TO.MEMBER]_MUST_BE_STR_TYPE_")
    if not isinstance(_amount, float):
        return(False, "_ERROR_[AMOUNT]_MUST_BE_FLOAT_")
    if not isinstance(_pubkey, str):
        return (False, "_ERROR_[PUBKEY]_MUST_BE_STR_TYPE_")
        
    try:
        signingkey = SigningKey.from_pem(_privkey)
    except:
        return (False, "_ERROR_LOADING_SIGNING_KEY_FROM_PEM_")

    tx_data = {
        "from": _from,
        "to": _to,
        "amount": _amount,
    }

    tx_data = dumps(tx_data).encode('utf-8')
    signature = signingkey.sign(tx_data)
    if not verify(_pubkey, signature, tx_data):
        return (False, "_ERROR_VERIFYING_SIGNATURE_")

    tx_type = typeTrans["ONE2ONE"]
    if len(_to) > 1:
        tx_type = typeTrans["ONE2MANY"]
        
    FINAL_TX = transaction(tx_type, str(tx_data), _pubkey, str(signature))

    return FINAL_TX

'''

    Send TX to network

'''
def send_transaction(tx, node="127.0.0.1"):
    '''
        Create IP.1 compliant data and broadcast
    '''
    out = "broadcast\nnew_transaction\n" + tx + "\n"
    qres = query(node, node_settings["port"], out, logger, 25)

    if qres == -3:
        retry = __PUSH_ATTEMPTS__
        while True:
            qres = query(node, node_settings["port"], out, logger, 25)
            if qres != -3:
                break
            retry -= 1
            if retry == 0:
                qres = "Unable to verify send after " + str(__PUSH_ATTEMPTS__) + " attempts"
                break

    logger.info((str(qres) + out))
    if qres == b"_NEW_TX_CREATED_":
        return (True, "Sent, Created")
    elif qres == b"_TRANSACTION_ALREADY_EXISTS_":
        return (True, "Sent, Exists")
    else:
        return (False, ("TX failt to be added to pool => " + str(qres)))

