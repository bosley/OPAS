
from os import name, system
from time import sleep
from hashlib import sha256
from random import randint, uniform, choice
from datetime import datetime
from Crypto.PublicKey import RSA 
from Crypto.Signature import PKCS1_v1_5 
from Crypto.Hash import SHA256 
from base64 import b64decode, b64encode

'''
    String and datetime conversion functions
'''
# Ensure datetime or string format of data
EnsureDT = lambda x: x if isinstance(x, datetime) else datetime.strptime(x, '%Y-%m-%d %H:%M:%S')
EnsureST = lambda x: x if isinstance(x, str) else '{0:%Y-%m-%d %H:%M:%S}'.format(x)

'''
    TCP Responses
'''
# Ensure valid response, but dont extrapolate any meaning
ValidTCPRespnse = lambda x: True if (x != -1 and x != -3 and x is not None) else False

'''
    Selection functions
'''
# Pass in list, get random element back
GetRandomChoice = lambda x: choice(x)

'''
    Screen functions
'''

def ClearScreen():
    if name == 'nt':
        system("cls")
    else:
        system("clear")

'''
    Network static settings
'''
message_delim  = "\r\t\n\r\n"

'''
    Generate a unique address
'''
def generate_address(network):
    h = sha256()
    h.update(network.encode('utf-8'))
    for i in range(1,257):
        h.update(str(randint(0, 2**i)).encode('utf-8'))
        sleep( uniform(0.001, 0.009) )
    addr = h.hexdigest().encode('utf-8')
    # XOR result's half's, return result
    a1, a2 = addr[:len(addr)//2], addr[len(addr)//2:]
    addr = hex(int(a1,16) ^ int(a2,16))
    return(True, network + "-" + addr)

'''
    Assemble a message
'''
def create_message(private_key, route, nfrom, nto, message, previous_hash=None):
    if previous_hash is None:
        previous_hash = "nil"
    h = sha256()
    h.update(message.encode('utf-8'))
    h.update(previous_hash.encode('utf-8'))
    cHash = h.hexdigest()
    signature = str(sign_data(private_key, cHash))
    data = (
                str(route) + message_delim + 
                str(nfrom) + message_delim + 
                str(nto) + message_delim + 
                cHash + message_delim + 
                previous_hash + message_delim + 
                message + message_delim +
                signature
            )
    return data

'''
    Disassemble Message
'''
def disassemble_message(message):
    if not isinstance(message, str):
        try:
            message = message.decode()
        except:
            message = str(message)
    message = message.split(message_delim)
    if len(message) != 7:
        return(False, "Invalid Data")
    mData = {
        "route": message[0],
        "from": message[1],
        "to": message[2],
        "hash": message[3],
        "prev_hash": message[4],
        "data": message[5],
        "signature": message[6]
    }
    return (True, mData)

'''
    Verify a signature
'''
def verify_data(public_key, data, signature):
    signature = b64decode(signature)
    rsakey = RSA.importKey(public_key) 
    digest = SHA256.new() 
    digest.update(data.encode('utf-8'))
    verifier = PKCS1_v1_5.new(rsakey)
    return(verifier.verify(digest, signature))

'''
    Sign some data
'''
def sign_data(private_key, data):
    key = private_key
    rsakey = RSA.importKey(key) 
    signer = PKCS1_v1_5.new(rsakey) 
    digest = SHA256.new() 
    digest.update(data.encode('utf-8'))
    sign = b64encode(signer.sign(digest)).decode()
    return sign

'''
    Generate RSA key pairs
'''
def generate_keys():
    RSAkey = RSA.generate(1024)
    private = RSA.generate(1024)
    public  = private.publickey()
    return(public.exportKey().decode(), private.exportKey().decode())