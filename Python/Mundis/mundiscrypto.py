
import base64
import random
import string
from Crypto.Cipher import AES

class MundisCryptographerException(Exception):
    def __init__(self, dErrArguments):
        Exception.__init__(self, "Error: {0} | {1}".format(
            dErrArguments[0], dErrArguments[1]
            ))
        self.dErrArguments = dErrArguments

def GenerateToken(token_len):
    if token_len != 16 and token_len != 24 and token_len != 32:
        raise MundisCryptographerException((100, "Token length must be 16, 24,or 32"))
    else:
        return ''.join(
            random.SystemRandom().choice(
                string.ascii_uppercase + string.digits
                ) for _ in range(token_len)
            )

class MundisCrypto:
    def __init__(self, token):
        self.token = token
        self.byte_len = 16
        self.token_len = 32
        
    def _pad(self, data, b):
        if b < len(data):
            b += b
            return self._pad(data, b)
        else:
            return data.rjust(b)

    def encode(self, data):
        data = self._pad(data, self.byte_len)
        cipher = AES.new(self.token, AES.MODE_ECB)
        return base64.b64encode(cipher.encrypt(data))

    def decode(self, data):
        data = self._pad(data, self.byte_len)
        cipher = AES.new(self.token,AES.MODE_ECB)
        data = cipher.decrypt(base64.b64decode(data))
        return data.decode("utf-8").lstrip()