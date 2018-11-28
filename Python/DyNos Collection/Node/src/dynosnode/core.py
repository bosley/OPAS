import os
import dill
import dynosnode
import gherkindb
import dynostcp
import signal
from time import sleep
from datetime import datetime
from uuid import getnode as getmac
import threading

''' Create a directory structure '''
def create_structure(directory):
    try:
        os.makedirs(directory)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

''' Ensure that directory strutures exist '''
if not os.path.exists(dynosnode.PROG_ROOT):
    create_structure(dynosnode.PROG_ROOT)

if not os.path.exists(dynosnode.DB_ROOT):
    create_structure(dynosnode.DB_ROOT)
    dynosnode.KEY_MANAGER.newKey("node-key", True)

if not os.path.exists(dynosnode.DB_SLOTS):
    create_structure(dynosnode.DB_SLOTS)

if not os.path.exists(dynosnode.DB_ROUTINES):
    create_structure(dynosnode.DB_ROUTINES)

''' Node datbase is cleared every run '''
db = gherkindb.load(dynosnode.DB_CONFIG, True)
db.deldb()
db.set("routines", [])
db.set("slots", [])
db.set("_routine_capsules_", [])
db.set("_slot_capsules_", [])
db.set("server-info", {})

''' Initial node setup '''
def get_node_setup():
    global db
    return {
        "info": db.get("info"),
        "slots": db.get("slots"),
        "routines": db.get("routines")
    }

''' Build a simple mac.name string '''
def get_dyno_id(func_name):
    if isinstance(func_name, str):
        rname = func_name
    elif callable(func_name):
        rname = func_name.__name__
    else:
        rname = str(func_name)
    return "{}.{}".format(str(getmac()), rname)

''' Register slot '''
def register_slot(func, arg_list, returns):
    if not callable(func):
        print("Function must be callable to become slot")
        exit(1)
    if not isinstance(arg_list, list):
        print("Slot registration error, arg_list must be of type 'list.'")
        exit(1)
    for x in arg_list:
        if not isinstance(x, tuple):
            print("Slot registration error, arguments must be of type 'tuple.'")
            print("Such-that the tuple contains (arg_name, var_type).")
            exit(1)
        if not isinstance(x[0], str):
            print("Argument name must be of type 'str.'")
            exit(1)
    if returns not in dynosnode.VALID_RETURNS:
        print("Type of 'returns' in slot is invalid. Allowed types are:")
        for x in dynosnode.VALID_RETURNS:
            print("\t", x)
        exit(1)

    # Should be valid, set in db
    global db
    slots = db.get("slots")
    slots.append({
        "name": func.__name__,
        "func": func,
        "arguments": arg_list,
        "returns": returns
    })
    db.set("slots", slots)
    return get_dyno_id(func)

''' Register Routine '''
def register_routine(func, returns):
    if not callable(func):
        print("Function must be callable to become a routine")
        exit(1)
    if returns not in dynosnode.VALID_RETURNS:
        print("Type of 'returns' in routine is invalid. Allowed types are:")
        for x in dynosnode.VALID_RETURNS:
            print("\t", x)
        exit(1)
    
    # Should be valid, set in db
    global db
    routines = db.get("routines")
    routines.append({
        "name": func.__name__,
        "func": func,
        "returns": returns
    })
    db.set("routines", routines)
    return get_dyno_id(func)

''' Register node definition '''
def register_definition(local_definition):
    if not isinstance(local_definition, dict):
        print("Definition must be of type 'dict'.")
        exit(1)

    # Ensure no missing definitions
    for definition in dynosnode.HEADERS_LOCAL_DEFINITION:
        if definition not in local_definition:
            print("Missing definition", definition, 
                "in node_definition dict. Required headers are: \n")
            for x in dynosnode.HEADERS_LOCAL_DEFINITION:
                print("\t", x)
            exit(1)
    
    # Ensure only allowed keys
    for key in list(local_definition.keys()):
        if key not in dynosnode.HEADERS_LOCAL_DEFINITION:
            print("Unknown key: ", key, "in node_definition. Only the following are allowed:")
            for x in dynosnode.HEADERS_LOCAL_DEFINITION:
                print("\t", x)
            exit(1)

    local_definition["mac"] = str(getmac())

    global db
    db.set("info", local_definition)

''' Verify a signature '''
def checkSignature(signature, data, publicKey):
    return dynosnode.KEY_MANAGER.verifySignature(
        publicKey,
        signature,
        data
    )

''' Sign data '''
def signData(data):
    return dynosnode.KEY_MANAGER.signFromKey(
        "node-key", 
        str(dill.dumps(data)))

''' Send data off to the server, and get a response '''
def send_to_server(address, port, data):
    try:
        data = dill.dumps(data)
    except Exception as e:
        return (str(e.__doc__), -9)
    try:
        response = dynostcp.query(
            address, port, data
            )
    except Exception as e:
        return (str(e.__doc__), -10)
    try:
        response = dill.loads(response)
    except Exception as e:
        return (str(e.__doc__), -11)
    return response

''' Register the node with a hub '''
def ensure_registration():

    nodeInfo = get_node_setup()
    if nodeInfo["info"] is None:
        print("No 'info' present in local DB.")
        exit(1)

    requestDict = {
        "caller": nodeInfo["info"]["mac"],
        "slots": dill.dumps(nodeInfo["slots"]),
        "routines": dill.dumps(nodeInfo["routines"]),
        "description": nodeInfo["info"]["description"],
        "location": nodeInfo["info"]["location"]
    }

    registration_packet = {
        "to": "DynosHub",
        "from": nodeInfo["info"]["mac"],
        "publicKey": dynosnode.KEY_MANAGER.getPublicKey("node-key"),
        "signature": signData(requestDict),
        "route": "registerNode",
        "data": requestDict
    }

    result = send_to_server(
        nodeInfo["info"]["address"], 
        dynosnode.NET_NODE_PORT,
        registration_packet)

    # Throw out any errors that came back
    if isinstance(result, tuple):
        print("Error registering node:\n", result[0])
        exit(1)

    elif isinstance(result, dict):
        if result["errors"]:
            print(result["data"])
            exit(1)
        
        # Ensure the dict is valid
        for x in ["signature", "data", "publicKey"]:
            if x not in result.keys():
                print("Invalid keys returned from server.")
                exit(1)

        # Verify signature and save public key of server
        if not checkSignature(
            result["signature"], 
            str(dill.dumps(result["data"])), result["publicKey"]):
            print("Unable to verify response from server.")
            exit(1)
        
        # Save the key to verify comm later
        db.dadd("server-info", ("publicKey", result["publicKey"]))

''' Send off a call '''
def send_call(caller, receiver, route, argument_dict):
    info = get_node_setup()
    packet = {
        "to": receiver,
        "from": caller,
        "publicKey": dynosnode.KEY_MANAGER.getPublicKey("node-key"),
        "signature": signData(argument_dict),
        "route": route,
        "data": dill.dumps(argument_dict)
        }
    result = send_to_server(
        info["info"]["address"], 
        dynosnode.NET_NODE_PORT,
        packet)
    if isinstance(result, tuple):
        return False
    if isinstance(result, dict):
        if result["errors"]:
            return False
    return True

'''
    Calls and responses to and from slots and routines
'''

''' Call a slot '''
def call_slot(caller, receiver, argument_dict):
    return send_call("{}->{}".format(caller, receiver), 
        receiver, "callSlot", argument_dict)

''' Call a routine '''
def call_routine(caller, receiver):
    return send_call("{}->{}".format(caller, receiver),
         receiver, "callRoutine", {})

''' Retrieve a response from a call '''
def get_response(caller, receiver, defer=True):
    def call_for_resp(ninfo, request_packet, skey):
        resp = send_to_server(
            ninfo["info"]["address"], 
            dynosnode.NET_NODE_PORT,
            request_packet)
        if isinstance(resp, tuple):
            return False, None
        if isinstance(resp, dict):
            if resp["errors"]:
                return False, None
            if not checkSignature(
                resp["signature"], 
                str(dill.dumps(resp["data"])), skey):
                return False, None
            return True, resp["data"]
        return False, None
    info = get_node_setup()
    response_id = "{}->{}".format(caller, receiver)
    packet = {
        "to": "DyNOS.server",
        "from": response_id,
        "publicKey": dynosnode.KEY_MANAGER.getPublicKey("node-key"),
        "signature": signData({}),
        "route": "getResponse",
        "data": dill.dumps({})
    }

    ''' Get response from server, if not deferred then hang  '''
    resp = False
    ret = {"response": "No response from server", "error": True}
    global db
    skey = db.dget("server-info", "publicKey")
    if not defer:
        while not resp:
            resp, ret = call_for_resp(info, packet, skey)
            sleep(0.1)
        return ret["response"]
    else:
        resp, ret = call_for_resp(info, packet, skey)
        return ret["response"]

'''
    Capsules
'''
class FunctionCapsule(threading.Thread):
    def __init__(self, ctype, server_pkey, func, verbose=False):
        threading.Thread.__init__(self)

        self.func = func
        self.ctype = ctype
        self.verbose = verbose
        self.info = get_node_setup()
        self.stop = threading.Event()
        self.server_public = server_pkey
        self.id = "{}.{}".format(self.info["info"]["mac"], func.__name__)
        
        dbname = None
        if ctype == "slot":
            dbname = dynosnode.DB_SLOTS + func.__name__ + ".db"
            self.main_func = self.slot_main

        if ctype == "routine":
            dbname = dynosnode.DB_ROUTINES + func.__name__ + ".db"
            self.main_func = self.routine_main
            self.last_data_from_routine = None

        self.logdb = gherkindb.load(dbname, True)

    def timestamp(self):
        return datetime.now().strftime("[%Y-%m-%d %H:%M:%S]")

    def log(self, message):
        self.logdb.set(self.timestamp(), message)
        if self.verbose:
            print(self.timestamp(), message)

    def run(self):
        self.log("Starting {} {}".format(self.ctype, self.func.__name__))
        while not self.stop.set():
            self.main_func()
            sleep(0.5)

    def check_for_call(self):
        packet = {
            "to": "DyNOS.server",
            "from": self.id,
            "publicKey": dynosnode.KEY_MANAGER.getPublicKey("node-key"),
            "signature": signData({}),
            "route": "checkCalls",
            "data": {}
        }
        result = send_to_server(
            self.info["info"]["address"], 
            dynosnode.NET_NODE_PORT,
            packet)

        if isinstance(result, tuple):
            self.log("{} : Error checking for call : {}".format(
                self.id, result[0]
            ))
            return False, None
        if isinstance(result, dict):
            if result["errors"]:
                self.log("{} : Error checking for call : {}".format(
                self.id, result["data"]
                ))
                return False, None
        
        if self.server_public != result["publicKey"]:
            self.log("{} : Received key different from server's known key.".format(self.id))
            return False, None

        if not checkSignature(
            result["signature"], 
            str(dill.dumps(result["data"])), self.server_public):
            self.log("{} : Could not ".format(self.id))
            return False, None
        
        if result["data"]["caller"] is None:
            return False, None

        return True, result["data"]

    '''
        Create a response packet and send to the server
    '''
    def respond_to_call(self, caller, response):
        packet = {
            "to": caller,
            "from": self.id,
            "publicKey": dynosnode.KEY_MANAGER.getPublicKey("node-key"),
            "signature": signData(response),
            "route": "putResponse",
            "data": dill.dumps(response)
        }

        # Errors may be present in return, but the slot or routine
        # encapsulated doesn't really care as-to the response of 
        # sending - not yet
        send_to_server(
            self.info["info"]["address"], 
            dynosnode.NET_NODE_PORT,
            packet)
        
    '''
        Check for a request, and if a call exists 
        then execute the slot with the arguments
    '''
    def slot_main(self):
        active_request, request = self.check_for_call()
        if not active_request:
            return
        try:
            slot_return = self.func(request["arguments"])
        except Exception as e:
            self.log("{} :\n\tError executing slot with :\n\t{}".format(
                self.id, request["arguments"]
                ))
            slot_return = str(e.__doc__)
        self.respond_to_call(request["caller"], slot_return)

    '''
        Run the routine, and if a call for it's data
        exists then send the data off as a response
    '''
    def routine_main(self):
        try:
            self.last_data_from_routine = self.func()
        except Exception as e:
            self.log("{} : Error executing routine".format(self.id))
            self.last_data_from_routine = str(e.__doc__)
        active_request, request = self.check_for_call()
        if active_request:
            self.respond_to_call(request["caller"], self.last_data_from_routine)

'''
    Dynos Node Controller
'''
class NodeController:
    def __init__(self, verbose=False):
        self.alive = True
        self.verbose = verbose
        signal.signal(signal.SIGTERM, self.program_kill)
        signal.signal(signal.SIGINT, self.program_kill)
        self.program_loop()

    def _log(self, message):
        if self.verbose:
            print("DynosNodeController.log: ", message)

    def program_kill(self, signum, frame):
        self.alive = False
        self._log("Program being killed. Signum: {}".format(signum))

    def program_loop(self):

        global db
        self.capsules = []
        routine_list = db.get("routines")
        slot_list = db.get("slots")
        server_key = db.dget("server-info", "publicKey")

        for routine in routine_list:
            self.capsules.append(
                FunctionCapsule(
                    "routine", server_key, routine["func"], verbose=self.verbose
                    ))

        for slot in slot_list:
            self.capsules.append(
                FunctionCapsule(
                    "slot", server_key, slot["func"], verbose=self.verbose
                    ))

        for x in self.capsules:
            x.daemon = True
            x.start()

        while self.alive:
            sleep(0.5)

        self._log("Killing capsule threads.")
        for x in self.capsules:
            x.stop.set()


