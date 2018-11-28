
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
