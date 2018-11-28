'''
    MIT License

    Copyright (c) 2018 DigitalHills - Josh Bosley

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

from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography import exceptions
import gherkindb

'''
    Key manager exceptions
'''
class DHKeymanException(Exception):
    def __init__(self, exception_tuple):
        Exception.__init__(self, "Error: {0} | {1}".format(
            exception_tuple[0], exception_tuple[1]
            ))
        self.exception_tuple = exception_tuple

'''
    Key manager
'''
class DHKeyman:
    def __init__(self, key_db="./keyman.db"):
        self.keydb = gherkindb.load(key_db, True)
        self.error_dict = {
            "KEY_EXISTS": (100, "KEY EXISTS: Overwrite not enabled"),
            "NO_KEY_FROM_LABEL": (101, "NO KEY FOR LABEL: Given label has no key")
        }

    '''
        Internal use
    '''
    def _loadKey(self, pem_key):
        return serialization.load_pem_private_key(
            pem_key,
            password=None,
            backend=default_backend()
        )

    '''
        Generate a new key and store it in the db.
        Won't overwrite the key in the db if one exists 
        unless overwrite is enabled
    '''
    def newKey(self, label, overwrite=False):
        private_key = rsa.generate_private_key(
            backend=default_backend(), 
            public_exponent=65537, 
            key_size=2048)
        private_pem = private_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.TraditionalOpenSSL,
            encryption_algorithm=serialization.NoEncryption())
        if self.keydb.get(label) is not None and not overwrite:
            raise DHKeymanException(self.error_dict["KEY_EXISTS"])
        else:
            self.keydb.set(label, private_pem)

    '''
        Get a public key from a private key in the key database
    '''
    def getPublicKey(self, label):
        key_from_label = self.keydb.get(label)
        if key_from_label is None:
            raise DHKeymanException(self.error_dict["NO_KEY_FROM_LABEL"])
        public_key = self._loadKey(key_from_label).public_key().public_bytes(
            serialization.Encoding.OpenSSH,
            serialization.PublicFormat.OpenSSH)
        return public_key.decode('utf-8')

    '''
        Sign data with a private key in the key db
    '''
    def signFromKey(self, label, data):
        key_from_label = self.keydb.get(label)
        if key_from_label is None:
            raise DHKeymanException(self.error_dict["NO_KEY_FROM_LABEL"])
        if not isinstance(data, bytes):
            data = str.encode(data)
        private_key = self._loadKey(key_from_label)
        return private_key.sign(
            data,
            padding.PSS(
            mgf=padding.MGF1(hashes.SHA256()),
            salt_length=padding.PSS.MAX_LENGTH
        ), hashes.SHA256())

    '''
        Verify a signature with an RSA public key
    '''
    def verifySignature(self, public_key, signature, data):
        if not isinstance(public_key, bytes):
            public_key = str.encode(public_key)
        if not isinstance(data, bytes):
            data = str.encode(data)

        public_key = serialization.load_ssh_public_key(
            public_key, default_backend()
            )
        try:
            public_key.verify(
                signature,
                data,
                padding.PSS(
                    mgf=padding.MGF1(hashes.SHA256()),
                    salt_length=padding.PSS.MAX_LENGTH
                ), hashes.SHA256())
        except exceptions.InvalidSignature:
            return False
        return True