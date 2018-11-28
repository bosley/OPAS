# DH Key Manager

A simple python3-based keymanager. The goal of this project was to create an easy means
of generating and storing RSA keys for use in other projects. 

The usage is pretty straight-forward, and dhkeyman can be found on the python package manager
under ***dhkeyman***

    pip3 install dhkeyman

## Example:

```python
from dhkeyman import DHKeyman, DHKeymanException

key_manager = DHKeyman()

try: 
    key_manager.newKey("Local", True)
except DHKeymanException as err:
    print(err)

my_public_key = key_manager.getPublicKey("Local")

message = "Some Data I Need Signed"

sig = key_manager.signFromKey("Local", message)

verify = key_manager.verifySignature(
    my_public_key,
    sig,
    message
)

if verify:
    print("VERIFIED")
else:
    print("NOT VERIFIED")

```
