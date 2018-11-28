
import sys
import dill
import signal
import threading
import os, errno
import gherkindb
from time import sleep
from hashlib import sha512
from uuid import getnode as getmac
from datetime import datetime

import dynoshub
import dynoskeys
from dynostcp import dynostcp

'''
    Global volatile information
'''
# List of call dicts {caller, arguments} mapped to an owner
active_calls = gherkindb.load("active-calls", False)
# List of response dicts {} mapped to owner
active_responses = gherkindb.load("active-responses", False)
# Locks for slots and routines to ensure correctness of data
node_locks = gherkindb.load("node-locks", False)

'''
    Helper functions
'''

''' Create a directory structure '''
def create_structure(directory):
    try:
        os.makedirs(directory)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

''' Ensure that directory strutures exist '''
def ensure_directory_structure():
    if not os.path.exists(dynoshub.PROG_ROOT):
        create_structure(dynoshub.PROG_ROOT)

    if not os.path.exists(dynoshub.DB_ROOT):
        create_structure(dynoshub.DB_ROOT)

''' Timestamp '''
def timestamp():
    return datetime.now().strftime("[%Y-%m-%d %H:%M:%S]")

''' Configure first-time setup '''
def configure_setup():
    db = gherkindb.load(dynoshub.DB_CONFIG, True)
    if db.get("server-init") is not None:
        return 

    # Create a key-pair for the server
    dynoshub.KEY_MANAGER.newKey("server-key", True)

    # Setup server definition
    db.set("server-mac", str(getmac()).replace(" ", ""))

    # Indicate that the server is setup
    db.set("server-init", timestamp())

''' sha512 a python object '''
def hashObject(obj):
    return sha512(
        dill.dumps(obj)
    ).hexdigest()

''' Verify a signature '''
def checkSignature(signature, data, publicKey):
    return dynoshub.KEY_MANAGER.verifySignature(
        publicKey,
        signature,
        data
    )

''' Sign data '''
def signData(data):
    return dynoshub.KEY_MANAGER.signFromKey(
        "server-key", 
        str(dill.dumps(data)))

''' Setup locks, calls, and responses'''
def setup_global_dict(owner):
    global node_locks
    global active_calls
    global active_responses
    node_locks.set(owner, threading.Lock())
    active_calls.lcreate(owner)
    active_responses.lcreate(owner)

''' Receive a node lock '''
def get_node_lock(node):
    global node_locks
    node_lock = node_locks.get(node)
    if node_lock is None:
        setup_global_dict(node)
        node_lock = node_locks.get(node)
    return node_lock
    
'''
    TCP Handling methods
'''
def registerNode(request):
    db = gherkindb.load(dynoshub.DB_DEVICE, True)
    existing_entry = db.get(request["data"]["caller"])
    return_message = "Registration successful"

    entry = request["data"]
    entry["publicKey"] = request["publicKey"]
    entry["slots"] = dill.loads(entry["slots"])
    entry["routines"] = dill.loads(entry["routines"])

    # See if the node exists, and ensure that it is from a valid sender
    if existing_entry is not None:

        # Ensure that if a request might be an update, that the keys didn't change
        if existing_entry["publicKey"] != request["publicKey"]:
            return "Existing entry with different key. Cannot update.", True

        # Check to see if an update needs to take place
        if hashObject(existing_entry) == hashObject(entry):
            return "Node registered, and no update is required", False

        return_message = "Update successful"

    # Create entry, and return success
    db.set(request["data"]["caller"], entry)
    return return_message, False

''' Insert a request to a slot '''
def callSlot(request):
    #print("callSlot from:", request["from"], "Calling slot:", request["to"])

    global active_calls
    with get_node_lock(request["to"]):
        active_calls.ladd(request["to"],{
            "caller": request["from"],
            "arguments": request["data"]
        })
    return "Inserted", False

''' Insert a request to a routine '''
def callRoutine(request):
    global active_calls
    with get_node_lock(request["to"]):
        active_calls.ladd(request["to"],{
            "caller": request["from"],
            "arguments": {}
        })
    return "Inserted", False

''' Grap a response '''
def getResponse(request):
    global active_responses
    node_lock = get_node_lock(request["from"])
    # Check length of responses to see if any exist
    if active_responses.llen(request["from"]) == 0:
        return { "responder": None, "response": None }, False
    # If some exist, we need to pop-off an item
    with node_lock:
        response_dict = active_responses.lpop(request["from"], 0)
    return response_dict, False

''' Put a response '''
def putResponse(request):
    global active_responses
    with get_node_lock(request["to"]):
        active_responses.ladd(request["to"],{
            "responder": request["from"],
            "response": request["data"]
        })
    return "Response inserted", False

''' Check for calls to a slot or routine '''
def checkCalls(request):
    #print("checkCalls from:", request["from"])

    global active_calls
    node_lock = get_node_lock(request["from"])
        
    # Check length of calls to see if any exist
    if active_calls.llen(request["from"]) == 0:
        return { "caller": None, "arguments": None }, False
    # If some exist, we need to pop-off an item
    with node_lock:
        request_dict = active_calls.lpop(request["from"], 0)
    return request_dict, False

''' Network request handler '''
class NetRouter:
    def __init__(self):

        # Ensure that the server is configured before launching
        configure_setup()
        
        # Generate a 'from' label that all packets will use, 
        # along-with a generic signature tof the label (insecure)
        db = gherkindb.load(dynoshub.DB_CONFIG, False)
        
        # Standard 'from' label
        self.from_label = "{}.server".format(db.get("server-mac"))

        # All requests will containe a 'route' key to indicate where the data
        # needs to be routed to
        self.requestRouter = {
            "registerNode": registerNode,
            "callSlot": callSlot,
            "callRoutine": callRoutine,
            "getResponse": getResponse,
            "putResponse": putResponse,
            "checkCalls": checkCalls
        }

        # Errors that might be generated by the class
        self.errors = {
            "INVALID_REQUEST": "The request contained an invalid key.",
            "BAD_ACTOR": "Can not verify signature."
        }

    # Log something!
    def _log(self, message):
        print("Network Router: ", message)

    # Generate a network packet to send back to a caller
    def networkPacket(self, data, errors):
        return dill.dumps({
            "from": self.from_label,
            "publicKey": dynoshub.KEY_MANAGER.getPublicKey("server-key"),
            "signature": signData(data),
            "data": data,
            "errors": errors
        })

    # Take-in and validate a request, then send back the reply from a method
    def handle(self, data):
        data = dill.loads(data)
        # Some data may be packaged further
        if isinstance(data["data"], bytes):
            data["data"] = dill.loads(data["data"])
        # Ensure required keys exist
        if sorted(list(data.keys())) != sorted(dynoshub.KEYS_NETWORK_PACKET):
            return self.networkPacket(self.errors["INVALID_REQUEST"], True)
        # Check the data signature
        if not checkSignature(
            data["signature"], 
            str(dill.dumps(data["data"])), 
            data["publicKey"]):
            return self.networkPacket(self.errors["BAD_ACTOR"], True)
        # Get the response, and any errors 
        response, errors = self.requestRouter[data["route"]](data)
        return self.networkPacket(response, errors=errors)

'''

     Dynos Core controller 

'''
class DynosCore(dynoshub.Demiurge):
    def __init__(self):
        dynoshub.Demiurge.__init__(self, "DynosHub")
        ensure_directory_structure()

        # Not required since daemon implemented
        self.alive = True
        signal.signal(signal.SIGTERM, self.program_kill)
        signal.signal(signal.SIGINT, self.program_kill)

    def _log(self, message):
        print("DynosCore.log: ", message)

    # Not required since daemon implemented
    def program_kill(self, signum, frame):
        self.alive = False
        self._log("Program being killed. Signum: {}".format(signum))

    def run(self):
        # Network server
        self._nst = dynostcp("127.0.0.1", dynoshub.NET_NODE_PORT, RouterClass=NetRouter)
        self._nst.start()

        self._log("Starting program loop")
        while self.alive:
            sleep(0.5)

        # Not required since daemon implemented, but kept in-case
        # ran as non-daemon
        self._log("Attempting to shutdown threads")
        self.alive = False
        self._nst.kill()

''' Display server status '''
def dynos_status():
    print("\n\nDyNOS Hub Information\n\n")
    print("Server mac:\t", getmac())
    print("Node port:\t", dynoshub.NET_NODE_PORT)
    status = "OFFLINE"
    if os.path.isfile("/tmp/DynosHub.pid"):
        status = "ONLINE"
    print("Status:\t\t", status)
    print("\n\n")

''' Exported launcher of the core class '''
def launch_dynos():
    if len(sys.argv) != 2:
        print('Usage: {} [start|stop|info]'.format(sys.argv[0]))
        return

    demiurge = DynosCore()

    try:
        if 'start' == sys.argv[1]:
            print("Starting DyNOS Hub")
            demiurge.summon()
        elif 'stop' == sys.argv[1]:
            demiurge.banish()
            print("DyNOS Hub has been terminated")
        elif 'info':
            dynos_status()
        else:
            sys.exit(2)
    except RuntimeError as e:
        print("Command RuntimeError: ", e)
    sys.exit(0)