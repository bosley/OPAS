# xSRH

### Crossed Send, Receive, Handle
This little project was made as I started to do network programming
with the Raspberry Pi. I wanted a decently secure way to send data
across the network. xSRH is a module that handles the encryption,
and decryption of messages when sending and receiving.

###### Sender
The sender functionality takes in an address, a port, and a message
after the object is created and handles the handshake / transfer of 
infromation to another device runnign xSRH using the same settings 
dictionary. 

###### Receiver
The receiver uses the sender class to encrypt/decrypt information
and then calls a user-defined handler function once the data is 
decrypted. The user-defined handler function can do whatever with 
the data, and must send back a string so the other device's sender
method can obtain a reply. If no string is sent back, it is recommended
that 'settings["accepted"]' is sent back as 'None' is not allowed and
an error will be thrown in the en/decryption process. 

###### Example Usage
```python

# The interface
from xSRH import SrhInterface

# A handler function
def customHandler(settings, data):
	print("I got some data! : ", data)
	return settings["accepted"]

# Default settings used
si = SrhInterface(settings=None, redirect=customHandler)
result = si.send("127.0.0.1",4096, "Some data")
print(result)
```

_Result:_
> I got some data! : Some data
> 0x0000

###### More examples:

For more examples on how to use xSRH check out 'example.py' and
'xSRH/srhtester.py'








