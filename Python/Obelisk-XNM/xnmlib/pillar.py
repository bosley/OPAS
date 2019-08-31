from sys import exit
from datetime import datetime
from json import dumps, loads
from time import sleep
from .display import cout, ctxt
from .data import DataStore
from .core import generate_address
from .core import EnsureDT, EnsureST, ValidTCPRespnse
from .core import GetRandomChoice, ClearScreen
from .core import create_message, disassemble_message
from .core import generate_keys, sign_data, verify_data
from .tcplib import query, tcpServerInstance
from .debug import debug_TODO, debug_MESSAGE
from .debug import debug_VAROUT, debug_GOOD
from .debug import debug_BAD, debug_FATAL

'''
    Debug flags
'''
from .debug import __DISPLAY_TODO_OUTPUT__
from .debug import __DISPLAY_MERGE_OUTPUT__
from .debug import __DISPLAY_SCANNING_OUTPUT__
from .debug import __DISPLAY_SYNC_PILLAR_OUTPUT__
from .debug import __DISPLAY_SYNC_NODE_OUTPUT__

'''
    Network settings 
'''
# Network static data, changing can ensure
# incompatibility between seperate networks 
__NETWORK_DATA_PING__ = "*"
__NETWORK_DATA_NOT_READY__ = "__PILLAR_NOT_READY__"

'''
    Global, static, settings
'''
__MESSAGE_LIFE__ = 600
__NODE_INACTIVITY_CAP__ = 8600
__PILLAR_INACTIVITY_CAP__ = 2000
__MAX_PING_ATTEMPT__ = 5
__SCREEN_REFRESH__ = 2
__SCREEN_DISPAY__ = True
__SCREEN_MESSAGE_COUNT__ = 10
__SCREEN_MESSAGE_LEN_LIM__ = 45
__MESSAGE_PRUNE_SEC__ = 600
__NODE_PRUNE_SEC__ = 1800
__PILLAR_PRUNE_SEC__ = 600

'''
    System Messages
'''
class XNMPillarSystemMessage:
    def __init__(self, mode, message, time=10):
        color = None
        if "info" == mode: self.info = [None, time, ctxt("yellow", message)]
        elif "flag" == mode: self.info = [None, time, ctxt("blue", message)]
        elif "warning" == mode: self.info = [None, time, ctxt("red", message)]
        elif "critical" == mode: self.info = [None, time, ctxt("red", message)]
        elif "fail" == mode: self.info = [None, time, ctxt("fail", message)]
        elif "success" == mode: self.info = [None, time, ctxt("lightgreen", message)]
        else: self.info = [None, time, message]
            
    def __str__(self):
        if len(self.info[2]) > __SCREEN_MESSAGE_LEN_LIM__:
            return self.info[2][0:(__SCREEN_MESSAGE_LEN_LIM__+5)] + "..."
        else:
            return self.info[2]

'''
    Pillar class
'''
class XNMPillar:

    def __init__(self, network, create_mode=False, ip_addr=None, port=None, remote_pillar=None):

        # Database accessor
        self.data_access = DataStore()

        if create_mode and remote_pillar is not None:
            cout("Remote pillar can not be defined in create mode!")
            exit(1)

        '''
            If this is defined, we are injecting a remote pillar
        '''
        if remote_pillar is not None:
            if not isinstance(remote_pillar, list):
                cout("fail", "remote_pillar needs to be a list [id, ip_address, port]")
                exit(1)
            else:
                if len(remote_pillar) != 3:
                    cout("fail", "remote_pillar needs to be a list [id, ip_address, port]")
                    exit(1)
            try:
                _temp = int(remote_pillar[2])
            except ValueError:
                cout("fail", "remote_pillar port must be integer")

            self.data_access.add_pillar_info("pillar", remote_pillar[1], remote_pillar[0], str(remote_pillar[2]), EnsureST(datetime.now()))
            exit(0)

        '''
            Setup a new pillar
        '''
        if create_mode:
            if ip_addr is None or ip_addr == "127.0.0.1" or ip_addr == "localhost":
                cout("fail", "An address that is non-local MUST be given to create pillar")
                exit(1)
            if port is None:
                cout("fail", "Port must be given for network")
                exit(1)
            port = str(port)
            try:
                temp_unused = int(port)
            except ValueError:
                cout("fail", "Port must be an integer!")
                exit(1)
            cout("yellow", "Generating address...")
            new_address = generate_address(network)
            if new_address[0]:
                new_address = new_address[1]
            else:
                cout("fail", "Error creating address: " + new_address[1])
                exit(1)
            self.port = port
            self.ip = ip_addr
            self.address = new_address
            self.data_access.add_pillar_info(
                "pillar", self.ip, self.address, self.port, EnsureST(datetime.now())
                )
            cout("yellow", "Generating RSA keys...")
            self.keys = generate_keys()
            self.data_access.add_keys_info("pillar", self.address, self.keys[0], self.keys[1])
            print(
                ctxt("lightgreen", "Pillar created : "),
                ctxt("cyan", self.address),
                "@",
                ctxt("yellow", self.port)
            )
            exit(0)
        
        # load pillar info
        pillar_info = self.data_access.get_pillar("pillar", network)
        if len(pillar_info) == 0:
            cout("fail", "Unable to find pillar in database")
            exit(1)

        self.ip = pillar_info[0]["ip"]
        self.id = pillar_info[0]["id"]
        self.port = int(pillar_info[0]["port"])
        self.last_online = EnsureST(datetime.now())

        # load RSA keys
        key_info = self.data_access.get_keys("pillar", self.id)
        if len(key_info) > 1:
            cout("fail", "Error: Pillar has more than one key pair..")
            exit(1)
        if len(key_info) == 0:
            cout("fail", "Error: Pillar can't load key pair..")
            exit(1)
        self.public_key = key_info[0]["public"]
        self.private_key = key_info[0]["private"]

        # Settings 
        self.allow_more_than_ping = False
        self.last_prune = datetime.now()
        self.initTime = datetime.now()
        self.newest_node_additions = self.initTime
        self.pillars_currently_online = []
        self.system_messages = []

        # For message flow control
        # Make sure we don't send read messages multiple times to pillars
        self.message_filter = set()

        # Ensure we don't send read messages multiple times to nodes
        self.node_message_filter = set()

        # Ensure we don't sync keys during overlaps
        self.node_pub_key_filter = set()

        # Accepted request routes
        self.accepted_requests_pillar = ["register", "messages", "nodes", "pillars", "read"]
        self.accepted_requests_node = ["register", "submit", "broadcast", "check", "nodes", "pillars"]

        # TCP bridge
        self.__th__tcp = tcpServerInstance(self.ip, self.port, RouterClass=self)
        self.__th__tcp.daemon = True
        self.__th__tcp.signal = True
        self.__th__tcp.start()

        # Last DB prune
        self.last_prune = datetime.now()

        # Run the pillar
        self.run()
    
    '''
        Main loop of the pillar construct
    '''
    def run(self):
        ClearScreen()
        self.running = True
        print(ctxt("cyan", self.ip) + ":" + ctxt("yellow", str(self.port)))
        clstimer = datetime.now()
        
        try:
            while self.running:
                
                # Display information to the screen
                if __SCREEN_DISPAY__:
                    if (datetime.now() - clstimer).total_seconds() > __SCREEN_REFRESH__:
                        clstimer = datetime.now()
                        ClearScreen()
                        print(ctxt("yellow", "Pillar ID: "), ctxt("cyan", self.id))
                        cout("yellow", "There are " + str(len(self.pillars_currently_online)) + " other pillars online, and ")
                        cout("yellow", "There are " + str(len(self.known_nodes)) + " nodes known to this pillar.\n\n")
                        cout("underlined", "\t------[System Messages]------\t")
                        print("\n")

                        _m = []
                        for message in self.system_messages:
                            if message.info[0] is not None:
                                if (datetime.now() - message.info[0]).total_seconds() > message.info[1]:
                                    _m.append(message)
                        for m in _m:
                            self.system_messages.remove(m)

                        if len(self.system_messages) == 0:
                            cout("yellow", "\tNo system messages")
                        msgcount = 0
                        for message in self.system_messages:
                            if message.info[0] is None:
                                message.info[0] = datetime.now()
                            if (datetime.now() - message.info[0]).total_seconds() < message.info[1] and msgcount < __SCREEN_MESSAGE_COUNT__:
                                print("\t", message)
                            msgcount += 1
                        if msgcount >= __SCREEN_MESSAGE_COUNT__:
                            cout("yellow", "\t...some messages omitted...")
                        cout("fail", "\n\nUse CTRL+C to close pillar")

                # Known pillars
                self.known_pillars = self.data_access.get_pillars("pillar")
                self.known_nodes = self.data_access.get_nodes("pillar")

                # Check for online pillars 
                self.check_for_pillar_activity()

                # use sync lock to ensure TCP handler doesn't accept sync requests 
                # until we complete our sync
                self.sync_lock = True

                # Mark ourselves as being online
                self.data_access.update_pillar_connect("pillar",self.id, EnsureST(datetime.now()))

                # Try syncing with any online pillars
                if len(self.pillars_currently_online) > 0:
                    self.select_and_sync_pillar()

                # See if we should prune the db
                self.check_for_prune()

                # Our sync should be completed, allow remotes to sync
                self.sync_lock = False

                sleep(0.1)

        except KeyboardInterrupt:
            self.running = False
            
        cout("yellow", "\nClosing down pillar.. please wait..")
        # Program terminated, write out the pillar status
        self.data_access.update_pillar_activity("pillar", self.ip, self.id, EnsureST(datetime.now()))

    '''
        Generic Execution
    '''
    def execute_request(self, request, receiver):
        port = int(receiver["port"])
        for i in range(0,5):
            result = query(receiver["ip"], port, request)
            if ValidTCPRespnse(result):
                return(True, result)
        return(False, "Unable to send request")

    '''
        Clean up the databases
    '''
    def check_for_prune(self):
        read_messages = self.data_access.get_messages_all_read("pillar")
        unread_messages = self.data_access.get_messages_all_unread("pillar")

        marked_for_deletion = []
        for x in unread_messages:
            if (datetime.now() - EnsureDT(x["creation"])).total_seconds() > __MESSAGE_PRUNE_SEC__:
                marked_for_deletion.append(x["hash"])
        for x in read_messages:
            if (datetime.now() - EnsureDT(x["creation"])).total_seconds() > __MESSAGE_PRUNE_SEC__:
                marked_for_deletion.append(x["hash"])
        for m in marked_for_deletion:
            self.data_access.delete_message_by_hash("pillar", m)

        marked_for_deletion.clear()
        nodes = self.data_access.get_nodes("pillar")
        for n in nodes:
            if (datetime.now() - EnsureDT(n["last_connect"])).total_seconds() > __NODE_PRUNE_SEC__:
                marked_for_deletion.append(n["uid"])
        for m in marked_for_deletion:
            self.data_access.delete_node_by_uid("pillar", m)
            self.data_access.delete_key_by_id("pillar", m)
            
        marked_for_deletion.clear()
        peers = self.data_access.get_pillars("pillar")
        for p in peers:
            if (datetime.now() - EnsureDT(p["last_online"])).total_seconds() > __PILLAR_PRUNE_SEC__:
                marked_for_deletion.append(p["id"])
        for m in marked_for_deletion:
            self.data_access.delete_pillar_by_id("pillar", m)
            self.data_access.delete_key_by_id("pillar", m)
                

    '''
        Scan known nodes to see who is online
    '''
    def check_for_pillar_activity(self):

        self.pillars_currently_online.clear()
        for pillar in self.known_pillars:

            ip = last_online = pillar["ip"]
            if ip != self.ip:

                result = None
                success = False
                for i in range(0, __MAX_PING_ATTEMPT__):
                    
                    # Send the ping query
                    result = query(pillar["ip"], int(pillar["port"]), __NETWORK_DATA_PING__, timeout=2)
                
                    if ValidTCPRespnse(result):
                        
                        '''
                                Pillar Registration

                                Ensure that we are registred with the pillar that responded to
                                our ping request
                        '''
                        registration = create_message(self.private_key, "p.register", self.id, pillar["id"], self.ip + "<&>" + self.public_key)
                        result_of_execute = self.execute_request(registration, pillar)

                        if result_of_execute[0]:
                            result = disassemble_message(result_of_execute[1])
                            if result[0]:
                                if result[1]["data"] == "_MALFORMED_REGISTRATION_" or result[1]["data"] == "_MALFORMED_REGISTRATION_IP_ERROR_":
                                    cout("fail", "Failure to comply with pillar's registration rules")
                                elif result[1]["data"] == "_REGISTERED_":
                                        self.pillars_currently_online.append(pillar)
                                        self.data_access.update_pillar_connect("pillar", pillar["id"], EnsureST(datetime.now()))
                                        break
                                else:
                                    # Ensure they gave us a key that can decode their signed hash
                                    if verify_data(result[1]["data"], result[1]["hash"], result[1]["signature"]):
                                        self.data_access.add_keys_info("pillar", result[1]["from"], result[1]["data"], "")
                                        self.pillars_currently_online.append(pillar)
                                        self.data_access.update_pillar_connect("pillar", pillar["id"], EnsureST(datetime.now()))
                                        break
                                    else:
                                        cout("fail", "Pillar failed to give valid key")
                            else:    
                                cout("fail", "Unknown data response in regards to registration with " + pillar["id"])
                        break
        
        # Signal single status
        if len(self.pillars_currently_online) == 0:
            self.allow_more_than_ping = True
            return
        return

    '''
        Chose, and sync with an online pillar

        This is where the outgoing requests from pillars originate, and end up
        at other pillars. 

                1) Sync Messages        - Grab all unread messages from remote pillar
                                            once we get the messages we will ensure that we don't have
                                            them already by checking them against their hash
                2) Sync Pillars         - Grab all known pillars from remote pillar
                                            once we get the list, check to ensure that we don't already have them
                3) Sync Nodes           - Grab all known nodes from the remote pillar
                                            once we get the list, check to ensure that we don't already have them


    '''
    def select_and_sync_pillar(self):
        if len(self.pillars_currently_online) == 0:
            print("No pillars currently online to sync with.")
            return
        
        display_method_activity = __DISPLAY_SYNC_PILLAR_OUTPUT__

        # Find an online pillar
        pillar_to_sync_with = GetRandomChoice(self.pillars_currently_online)

        '''
            Sync Messages from another pillar
        '''
        messages = create_message(self.private_key, "p.messages", self.id, pillar_to_sync_with["id"], "...")
        result_of_execute = self.execute_request(messages, pillar_to_sync_with)
        if not result_of_execute[0]:
            self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to execute message sync"))
        else:
            result = disassemble_message(result_of_execute[1])
            if not result[0]:
                self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to disasm message sync result"))
            else:
                if result[1]["data"] == "_SYNC_LOCK_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote is in sync_lock"))
                elif result[1]["data"] == "_LOCAL_ERROR_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote exp local error"))
                elif result[1]["data"] == "NO_MESSAGES":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "No messages to sync"))
                else:
                    messages_list = loads(result[1]["data"])
                    local_unread = self.data_access.get_messages_all_unread("pillar")
                    new_messages = 0

                    for message in messages_list:
                        
                        # If we didn't encounter this message before, ensure we don't have a saved copy
                        if message["hash"] not in self.message_filter:
                            found = False
                            for local in local_unread:
                                if message["hash"] == local["hash"]:
                                    found = True
                                    break
                            if not found:
                                self.data_access.add_message_info("pillar",
                                message["fro"],
                                message["gto"],
                                message["creation"],
                                message["previous_hash"],
                                message["hash"],
                                message["status"],
                                message["message"]
                                )
                                self.message_filter.add(message["hash"])
                                new_messages += 1

                    self.system_messages.append(XNMPillarSystemMessage("flag", "[" + str(new_messages) + "] new messages" ))

        '''
            Sync Pillars from another pillar
        '''
        inpillars = create_message(self.private_key, "p.pillars", self.id, pillar_to_sync_with["id"], "...")
        result_of_execute = self.execute_request(inpillars, pillar_to_sync_with)
        if not result_of_execute[0]:
            self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to execute pillar sync"))
        else:
            result = disassemble_message(result_of_execute[1])
            if not result[0]:
                self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to disasm pillar sync result"))
            else:
                if result[1]["data"] == "_SYNC_LOCK_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote is in sync_lock"))
                elif result[1]["data"] == "_LOCAL_ERROR_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote exp local error"))
                elif result[1]["data"] == "NO_PILLARS":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "No pillars to sync"))
                else:
                    pillar_list = loads(result[1]["data"])
                    local_pillars = self.data_access.get_pillars("pillar")
                    new_pills = 0
                    for pill in pillar_list:
                        found = False
                        for local in local_pillars:
                            if pill["id"] == local["id"]:
                                found = True
                                break
                        if not found:
                            self.data_access.add_pillar_info("pillar", 
                            pill["ip"], 
                            pill["id"], 
                            pill["port"],
                            pill["last_online"]
                            )
                            new_pills += 1
                    self.system_messages.append(XNMPillarSystemMessage("flag", "[" + str(new_pills) + "] new pillars" ))

        '''
            Sync Nodes from another pillar
        '''
        innodes = create_message(self.private_key, "p.nodes", self.id, pillar_to_sync_with["id"], "...")
        result_of_execute = self.execute_request(innodes, pillar_to_sync_with)
        if not result_of_execute[0]:
            self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to execute node sync"))
        else:
            result = disassemble_message(result_of_execute[1])
            if not result[0]:
                self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to disasm node sync result"))
            else:
                if result[1]["data"] == "_SYNC_LOCK_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote is in sync_lock"))
                elif result[1]["data"] == "_LOCAL_ERROR_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote exp local error"))
                elif result[1]["data"] == "NO_NODES":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "No nodes to sync"))
                else:
                    node_list = loads(result[1]["data"])
                    local_nodes = self.data_access.get_nodes("pillar")
                    new_nodes = 0
                    for nod in node_list:
                        found = False
                        for local in local_nodes:
                            if nod["uid"] == local["uid"]:
                                found = True
                                break
                        if not found:
                            self.data_access.add_node_info("pillar",
                            nod["uid"],
                            nod["ip"],
                            nod["last_pull"],
                            nod["last_connect"],
                            nod["port"]
                            )
                            new_nodes += 1
                        
                            
                            
                    self.system_messages.append(XNMPillarSystemMessage("flag", "[" + str(new_nodes) + "] new nodes" ))
                
        '''
            Sync Read Messages from another pillar
        '''
        messages = create_message(self.private_key, "p.read", self.id, pillar_to_sync_with["id"], "...")
        result_of_execute = self.execute_request(messages, pillar_to_sync_with)
        if not result_of_execute[0]:
            self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to execute message sync"))
        else:
            result = disassemble_message(result_of_execute[1])
            if not result[0]:
                self.system_messages.append(XNMPillarSystemMessage("critical", "Failed to disasm message sync result"))
            else:
                if result[1]["data"] == "_SYNC_LOCK_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote is in sync_lock"))
                elif result[1]["data"] == "_LOCAL_ERROR_":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "Remote exp local error"))
                elif result[1]["data"] == "NO_MESSAGES":
                    self.system_messages.append(XNMPillarSystemMessage("flag", "No messages to sync"))
                else:
                    messages_list = loads(result[1]["data"])
                    local_unread = self.data_access.get_messages_all_unread("pillar")
                    read_messages = 0
                    for message in messages_list:
                        for local in local_unread:
                            if message["hash"] == local["hash"]:
                                self.data_access.set_message_read("pillar", message["hash"])
                                if message["hash"] in self.node_message_filter:
                                    self.node_message_filter.remove(message["hash"])
                                if message["hash"] in self.message_filter:
                                    self.message_filter.remove(message["hash"])
                                read_messages += 1
                                break
                    self.system_messages.append(XNMPillarSystemMessage("flag", "[" + str(read_messages) + "] read messages" ))

        '''
            Once we are done with sync cycle (at least one), ensure we can do more than ping
            - This is to ensure we are updated before broadcasting to the network
        '''
        # Once we merge, we can start taking requests for the network
        self.allow_more_than_ping = True
    
    '''
        Handle TCP requests to the pillar

                The TCP handler function is a product of the threaded tcpserver thread instance
                All TCP data brought-in from this library is sent here for a response.

    '''
    def handle(self, data):
        
        data = data.decode()
        if data == __NETWORK_DATA_PING__:
            return __NETWORK_DATA_PING__
        if not self.allow_more_than_ping:
            return __NETWORK_DATA_NOT_READY__

        '''
            Decode and dissassemble
                - Ensure the message is in a format that we understand
        '''
        data = disassemble_message(data)

        '''
            If it comes in a form we don't understand, reject it
        '''
        if data[0] is False:
            return "_REJECTED_"

        if "." not in data[1]["route"]:
            return "_REJECTED_MALFORMED_ROUTE_"

        direction = data[1]["route"].split(".")[0]
        request = data[1]["route"].split(".")[1]

        # Route pillar requests
        if direction == "p":
            
            if request not in self.accepted_requests_pillar:
                return "_REJECTED_UNKNOWN_PILLAR_REQUEST_"

            return self.pillar_requests(request, data[1])

        # Route direction requests
        if direction == "n":
            if request not in self.accepted_requests_node:
                return "_REJECTED_UNKNOWN_NODE_REQUEST_"
            return self.node_requests(request, data[1])

        return "_REJECTED_UNKNOWN_ROUTE_"

    '''
        Verify a signature
            - Ensure that we are talking to someone who gave 
                us their public key
    '''
    def authenticate_remote(self, id, data, signature):
        keys = self.data_access.get_keys("pillar", id)
        if len(keys) > 1:
            return (False, "Multiple keys for id exist - can't ensure validity")
        if len(keys) == 0:
            return (False, "No keys for given id")
        verified = verify_data(keys["public"], data, signature)
        if verified:
            return(True, "Signature accepted")
        else:
            return(False, "Signature invalid")

    '''
        Handle all incomming pillar requests

                        -------------------------------------------------------------------------
                        p.register      -  Request to register with pillar network
                                [VERIFY PILLAR]
                                  [SYNC LOCK]
                        p.messages      -  Requests current messages, the ["data"]
                                           of request should contain remote pillar's messages
                        p.nodes         -  Request all available nodes from DB, the ["data"]
                                           of request should contain remote pillar's nodes
                        p.pillars       -  Request of all available pillars from DB the ["data"]
                                           of request should contain remote pillar's pillars
                        -------------------------------------------------------------------------

    '''
    def pillar_requests(self, request, data):
        
        '''
            HANDLE A REQUEST FROM AN EXTERNAL PILLAR TO REGISTER WITH LOCAL PILLAR

            p.register
        '''
        if request == "register":
            
            #print(ctxt("cyan", "Pillar Registration"), ":", ctxt("yellow", data["from"]))

            self.system_messages.append(XNMPillarSystemMessage("info", "Route: p.register"))
            
            requesting_pillar = self.data_access.get_pillar("pillar", data["from"])
            if len(requesting_pillar) > 0:
                return create_message(self.private_key, "register", self.id, data["from"], "_REGISTERED_")
            
            try:
                remote_ip = data["data"].split("<&>")[0]
                remote_pub = data["data"].split("<&>")[1]
            except:
                return create_message(self.private_key, "register", self.id, data["from"], "_MALFORMED_REGISTRATION_IP_ERROR_")
        
            self.data_access.add_pillar_info("pillar", remote_ip, data["from"], str(self.port), EnsureST(datetime.now()))
            self.data_access.add_keys_info('pillar', data["from"], remote_pub, "NA")

            # The pillar has registered, send back the pillar's public key, and indicate its presence
            return create_message(self.private_key, "register", self.id, data["from"], self.public_key)

        '''
            Verify the pillar key against known keys in the system
                - Walls off all requests from unknown pillars.
        '''
        key = self.data_access.get_keys("pillar", data["from"])
        if len(key) != 1:
            cout("fail", "Can't retrieve keys for pillar.")
            return create_message(self.private_key, "register", self.id, data["from"], "Need to Register")
        if not verify_data(key[0]["public"], data["hash"], data["signature"]):
            return create_message(self.private_key, "register", self.id, data["from"], "VERIFICATION_FAILURE")

        '''
            Ensure we are not in sync_lock
        '''
        if self.sync_lock:
            return create_message(self.private_key, "register", self.id, data["from"], "_SYNC_LOCK_")

        '''
            HANDLE A REQUEST FROM AN EXTERNAL PILLAR TO SYNC MESSAGES WITH LOCAL PILLAR
            
            p.messages
        '''
        if request == "messages":
            messages = self.data_access.get_messages_all_unread("pillar")
            if len(messages) == 0:
                return create_message(self.private_key, "messages", self.id, data["from"], "NO_MESSAGES")
            try:
                messages = dumps(messages)
            except:
                return create_message(self.private_key, "messages", self.id, data["from"], "_LOCAL_ERROR_")
            return create_message(self.private_key, "messages", self.id, data["from"], messages)
            
        '''
            HANDLE A REQUEST FROM AN EXTERNAL PILLAR TO SYNC MESSAGES WITH LOCAL PILLAR
            
            p.read
        '''
        if request == "read":
            messages = self.data_access.get_messages_all_read("pillar")
            if len(messages) == 0:
                return create_message(self.private_key, "read", self.id, data["from"], "NO_MESSAGES")
            try:
                messages = dumps(messages)
            except:
                return create_message(self.private_key, "messages", self.id, data["from"], "_LOCAL_ERROR_")
            return create_message(self.private_key, "messages", self.id, data["from"], messages)

        '''
            HANDLE A REQUEST FROM AN EXTERNAL PILLAR TO SYNC NODES WITH LOCAL PILLAR
            
            p.nodes
        '''
        if request == "nodes":
            nodes = self.data_access.get_nodes("pillar")
            if len(nodes) == 0:
                return create_message(self.private_key, "nodes", self.id, data["from"], "NO_NODES")
            try:
                nodes = dumps(nodes)
            except:
                return create_message(self.private_key, "nodes", self.id, data["from"], "_LOCAL_ERROR_")
            return create_message(self.private_key, "nodes", self.id, data["from"], nodes)

        '''
            HANDLE A REQUEST FROM AN EXTERNAL PILLAR TO SYNC PILLARS WITH LOCAL PILLAR
            
            p.pillars
        '''
        if request == "pillars":
            pillars = self.data_access.get_pillars("pillar")
            if len(pillars) == 0:
                return create_message(self.private_key, "pillars", self.id, data["from"], "NO_PILLARS")
            try:
                pillars = dumps(pillars)
            except:
                return create_message(self.private_key, "pillars", self.id, data["from"], "_LOCAL_ERROR_")
            return create_message(self.private_key, "pillars", self.id, data["from"], pillars)

        '''
            END OF PILLAR REQUESTS
        '''
        # End of Pillar requests. If this is reached, they sent an unknown request
        return "_REQUEST_ERROR_"


    '''
        Handle all node requests

                                    [ALL COMPLETED, LISTED IN THIS ORDER]
                        ---------------------------------------------------------------
                        n.register      -  Register the node into the DB
                                [VERIFY NODE]
                        n.submit        -  Add a message for a node in the DB
                        n.broadcast     -  Broadcast to multiple nodes
                        n.check         -  Request a pull of messages
                        n.nodes         -  Request a list of available nodes from DB
                        n.pillars       -  Request a list of available pillars from DB
                        ---------------------------------------------------------------

            Only allow register requests to go by without verifying nodes against
            their signatures. Once a node request is verified, other actions 
            become available
    '''
    def node_requests(self, request, data):
        
        '''
            Requests to register
        '''
        if request == "register":
            requesting_node = self.data_access.get_node("pillar", data["from"])
            requesting_node_keys = self.data_access.get_keys("pillar",data["from"])
            if len(requesting_node) > 0 and len(requesting_node_keys) > 0:
                return create_message(self.private_key, "register", self.id, data["from"], "_REGISTERED_")
            
            try:
                remote_ip = data["data"].split("<&>")[0]
                remote_pub = data["data"].split("<&>")[1]
            except:
                return create_message(self.private_key, "register", self.id, data["from"], "_MALFORMED_REGISTRATION_IP_ERROR_")
        
            
            self.data_access.add_node_info("pillar", data["from"], remote_ip, EnsureST(datetime.now()), EnsureST(datetime.now()), str(self.port))
            self.data_access.add_keys_info('pillar', data["from"], remote_pub, "NA")

            # The node has registered, send back the pillar's public key
            return create_message(self.private_key, "register", self.id, data["from"], self.public_key)

        '''
            Verify the nodes key against known keys in the system
                - Walls off all requests from unknown nodes.
        '''
        key = self.data_access.get_keys("pillar", data["from"])
        if len(key) == 0:
            cout("fail", "Can't retrieve keys for node.")
            return create_message(self.private_key, "register", self.id, data["from"], "_PERFORM_REGISTRATION_")
        if not verify_data(key[0]["public"], data["hash"], data["signature"]):
            return create_message(self.private_key, "register", self.id, data["from"], "VERIFICATION_FAILURE")

        # Update online status
        self.data_access.update_node_online("pillar", data["from"], EnsureST(datetime.now()))

        '''
            Requests to submit message
        '''
        if request == "submit":
            try:
                self.data_access.add_message_info("pillar", 
                    data["from"], 
                    data["to"], 
                    EnsureST(datetime.now()), 
                    data["prev_hash"], 
                    data['hash'], 
                    "unread", 
                    data["data"]
                )
            except:
                cout("fail", "[SUBMIT] Failed to insert a message")
                return create_message(self.private_key, "submit", self.id, data["from"], "_ERROR_SUBMITTING_")

            return create_message(self.private_key, "submit", self.id, data["from"], "_SUBMITTED_")

        '''
            Requests to broadcast
        '''
        if request == "broadcast":
    
            # Get all nodes, find ones in the network that the broadcast is directed to
            nodes = self.data_access.get_nodes("pillar")
            if len(nodes) == 0:
                cout("fail", "No nodes returned, should be at-least one..")
                return create_message(self.private_key, "submit", self.id, data["from"], "_NO_NODES_KNOWN_")
            
            added = 0
            for node in nodes:
                try:
                    network = node["uid"].split("-")[0]
                except:
                    network = node["uid"]
                
                # If the the network of the current node matches that of the request
                if network == data["to"]: 
                    try:
                        self.data_access.add_message_info("pillar", 
                            data["from"], 
                            node["uid"], 
                            EnsureST(datetime.now()), 
                            data["prev_hash"], 
                            data['hash'], 
                            "unread", 
                            data["data"]
                        )
                        added += 1
                    except:
                        cout("fail", "[SUBMIT] Failed to insert a message")
                        return create_message(self.private_key, "submit", self.id, data["from"], "_ERROR_SUBMITTING_")

            # Tell the node how many nodes got the message
            msg = "__SUBMITTED_TO_" + str(added) + "_NODES_"
            return create_message(self.private_key, "submit", self.id, data["from"], msg)
            
        '''
            Checking for messages
        '''
        if request == "check": 
            msgs = self.data_access.get_messages_by_status("pillar", data["from"], "unread")
            if len(msgs) > 0:
                _ms = []
                for m in msgs:
                    if m["hash"] not in self.node_message_filter:
                        _ms.append(m)
                        self.node_message_filter.add(m["hash"])
                try:
                    msgs = dumps(_ms)
                except:
                    cout("fail", "Error dumping " + data["from"] + " messages")
                    return create_message(self.private_key, "pulled_messages", self.id, data["from"], "ERROR_DUMPING_MESSAGES")
                return create_message(self.private_key, "pulled_messages", self.id, data["from"], msgs)
            else:
                return create_message(self.private_key, "pulled_messages", self.id, data["from"], "NO_MESSAGES")

        '''
            Request for list of nodes
        '''
        if request == "nodes":
            nodes = self.data_access.get_nodes("pillar")
            try:
                nodes = dumps(nodes)
            except:
                cout("fail", "Error dumping nodes for " + data["from"])
                return create_message(self.private_key, "pulled_nodes", self.id, data["from"], "ERROR_DUMPING_NODES")
            return create_message(self.private_key, "pulled_nodes", self.id, data["from"], nodes)

        '''
            Request for list of pillars
        '''
        if request == "pillars":
            pillars = self.data_access.get_pillars("pillar")
            try:
                pillars = dumps(pillars)
            except:
                cout("fail", "Error dumping pillars for " + data["from"])
                return create_message(self.private_key, "pulled_pillars", self.id, data["from"], "ERROR_DUMPING_PILLARS")
            return create_message(self.private_key, "pulled_pillars", self.id, data["from"], pillars)

        '''
            END OF NODE REQUESTS
        '''
        # End of Node requests. If this is reached, they sent an unknown request
        return "_REQUEST_ERROR_"

