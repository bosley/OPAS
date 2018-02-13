# Documentation

#### deltamem (∆mem)
> A network attatched volitile memory unit used for offloading data.
> In some cases, a device may be generating more data than it can store
> locally on disk, or in memory. While these cases may be unlikely, the ∆mem
> package is a great way to ensure that the data generated can be stored and
> easily accessed. 

***Synopsis***

There are two parts to the ∆mem module, (Network Attatched Memory, or NAM).
1. RPC Server (netmem)
   The remote procedure call server uses XMLRPC and a dictionary to hold  
   all data that is requested to be stored. To prevent random garbage being  
   added, an authorization token is required to successfully store informaton.  
   For each device that calls the server, a new dictionary is created to allocated  
   storage of data that is marked by the caller with a label.  
   The server uses a 'localhost' by default, but can be set to any bindable address.  
   For cases that where limiting the amount of storage may be required, a max_storage  
   variable may be set.  

2. RPC Client (memchan)
   Interfacing with the server using memchan is quite simple. All that is required  
   is an authorization token matching the server, and a device identifier, along with  
   the server connection information (port, and ip if 'localhost' is not the server binding).

***Example***

RPC Server
```python
'''
    Netmem is a threaded class.
    If the only purpose of the program is to serve, 
    *.daemon can be omitted, otherwise the server will
    run until either *.signal is set to False, or the 
    main thread is exited
'''
from musys import netmem

# Using default 'localhost' binding
n = netmem("auth_token", 9001)
n.daemon = True
n.start()

# Using specified binding
n0 = netmem("auth_token", 9001, host="joshbosley.com")
n0.daemon = True
n0.start()

# Using a data cap
n1 = netmem("auth_token", 9001, host="joshbosley.com", max_storage=4096)
n1.daemon = True
n1.start()
```

RPC Client
```python
from musys import memchan

chan = memchan("auth_token", 9001, "Example-Device-ID")
# chan = memchan("auth_token", 9001, "Example-Device-ID", host="joshbosley.com")

my_data = "A wonderful string"

# Store a string 
chan.put("wonderful_string-label", my_data)

# Retrieve the string
fetch_result = chan.fetch("wonderful_string-label")

if fetch_result is None:
    print("Unable to fetch the string")
else:
    print(fetch_result)

# Since memchan serializes all data before sending to the server,
# even a function, or class can be stored remotely

def my_func():
    print("My awesome function!")

fetch_result = chan.fetch("my_func", my_func)

try:
    fetch_result()
except:
    print("Failure fetching function")
    raise Exception("Data not found, or integrity lost")

# Clears memory belonging to this instance
chan.memory_clear()
```


#### memsess (τmem)
> A session managment utility that uses ∆mem for temporary storage of data.
> Sessions can be timed (seconds) or set to stay open until manually closed.
> Once a session is closed, the data is scrubbed from the RPC Server

***Synopsis***

The τmem's memsess acts mostly as a pass-through for ∆mem's memchan functionality.
The only difference is the ability to set a timeout period, and a reset
to the timeout period to keep a session alive.

***Examples***

```python
from musys import memsess

# Create a session that will timeout after 10 seconds, even if activity occurs
my_session = memsess("auth_token", 9001, "Example-Device-ID", 10)
my_session.beginSession()
my_session.put("label", "TESTDATA")
ret = my_session.fetch("label")
print(ret)

# Create a session that will timeout after 10 seconds of no activity
my_session = memsess("auth_token", 9001, "Example-Device-ID", 10, reset_timeout=True)
my_session.beginSession()
my_session.put("label", "TESTDATA")
ret = my_session.fetch("label")
print(ret)

# Create a session that will timeout after 10 seconds of no activity, and manually
# The following code will demonstrate how to manually close the sessions, and then
# start the session back up (timeout resets when closed and re-opened)
new_session = memsess("auth_token", 9001, "Example-Device-ID", 10, reset_timeout=True)
new_session.beginSession()
new_session.put("label", "TESTDATA")
ret = new_session.fetch("label")
print(ret)

# Close the session
new_session.closeSession()

print("This should give None - Session not active")
ret = new_session.fetch("label")
print(ret)

# Restart the session
new_session.beginSession()

print("This should give key not found. - No previous session info should exist")
ret = new_session.fetch("label")
print(ret)

# Add some data for new session
new_session.put("label1", "TESTDATA1")
ret = new_session.fetch("label1")
print(ret)

# Close the session again
new_session.closeSession()
```