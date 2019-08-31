import threading
from sys import exit
from time import sleep
from datetime import datetime
from json import loads
from queue import Queue
from .data import DataStore
from .display import cout, ctxt
from .core import generate_address, ValidTCPRespnse
from .core import EnsureDT, EnsureST
from .core import GetRandomChoice, ClearScreen
from .core import create_message, disassemble_message
from .core import generate_keys, sign_data, verify_data
from .tcplib import query
from .debug import debug_TODO, debug_MESSAGE
from .debug import debug_VAROUT, debug_GOOD
from .debug import debug_BAD, debug_FATAL

'''
        TODO : 
                [TESTED BROADCAST, SEND (from other) SHOULD WORK THE SAME]
                - request_messages
                    > Server marks all pulled messages as read - Pehaps
                        add a 'pull_unread'

            neet to write 
                - update_available_nodes
                - update_available_pillars
'''



'''
    Network settings 
'''
# Network static data, changing can ensure
# incompatibility between seperate networks 
__NETWORK_DATA_PING__ = "*"

class XNMNode(threading.Thread):
    def __init__(self, id, create_node=False, port=None, ip_addr="127.0.0.1"):
        threading.Thread.__init__(self)

        # Access to databases
        self.data_access = DataStore()

        # If create node flag is raised, create and add to db
        if create_node:
            if port is None:
                cout("fail", "Port must be given for network")
                exit(1)
            port = str(port)
            try:
                temp_unused = int(port)
            except ValueError:
                cout("fail", "Port must be an integer!")
                exit(1)
            cout("yellow", "Generating address....")
            new_address = generate_address(id)
            if new_address[0]:
                new_address = new_address[1]
            else:
                cout("fail", "Error creating address: " + new_address[1])
                exit(1)
            self.data_access.add_node_info(
                "node", 
                new_address, 
                ip_addr, 
                EnsureST(datetime.now()), 
                EnsureST(datetime.now()),
                port
                )
            cout("yellow", "Generating RSA keys....")
            self.keys = generate_keys()
            self.data_access.add_keys_info("node", new_address, self.keys[0], self.keys[1])

            print(
                ctxt("lightgreen", "New node created! Address: "), 
                ctxt("cyan", new_address),
                "@",
                ctxt("yellow", port)
            )
            exit(0)
        self.id = id
        node_info = self.data_access.get_node("node", id)

        if len(node_info) > 1:
            cout("fail", "Error: Node's node table has duplicates..")
            exit(1)
        if len(node_info) == 0:
            cout("fail", "No node was found with the given address..")
            exit(1)

        key_info = self.data_access.get_keys("node", id)

        if len(key_info) > 1:
            cout("fail", "Error: Node has more than one key pair..")
            exit(1)
        if len(key_info) == 0:
            cout("fail", "Error: Node can't load key pair..")
            exit(1)

        # Node keys'''
        self.public_key = key_info[0]["public"]
        self.private_key = key_info[0]["private"]

        # Set the node settings
        self.ip = node_info[0]["ip"]
        self.last_pull = node_info[0]["last_pull"]
        self.last_connect = node_info[0]["last_connect"]

        # Behaviour 
        self.supressText = True
        self.signal = True
        self.daemon = True

        '''
            Interaction settings
        '''
        # Node serving as current point of contact
        self.current_provider = None
        self.known_pillars = []
        self.currently_available_pillars = []
        self.known_nodes = []

        # Messages we want to send to the network
        self.outgoing_messages = Queue()

        # Messages we've recieved from other nodes / etc
        self.received_messages = Queue()

    '''
        Main node loop
    '''
    def run(self):
        if not self.supressText:
            print("Node starting..")

        '''
            Load known pillars, get list of online
            Select random online pillar to join onto network, this becomes self.current_provider
                - If errors occur interacting with current_provider, retest avaiailable pillars, and get new provider
        '''

        self.sleep_on_provider_cycle = 1
        self.cycles_since_active_provider = 1

        while self.signal:
            
            # If we have a provider, ensure they are online
            if self.current_provider is not None:
                self.sleep_on_provider_cycle = 1
                result = self.perform_ping(self.current_provider["ip"], self.current_provider["port"])
                if not result:
                    self.current_provider = None
            
            # If we lost our provider, cycle known pillars and assign providers.
            # once we are attached to an active provider, we will continue updating.
            while self.current_provider is None and self.signal:
                
                # Every 10 failed attempts to contact a provider, add a second
                # onto how long we sleep so we don't over-flood the network
                # Once we exceed a 5-second wait per cycle, reset to 1
                self.cycles_since_active_provider += 1
                if 0 == self.cycles_since_active_provider % 10:
                    self.sleep_on_provider_cycle += 1
                    if self.sleep_on_provider_cycle > 5:
                        self.sleep_on_provider_cycle = 1
                        self.cycles_since_active_provider = 1

                # Check for online pillars, and try to assign a provider
                self.check_available_pillars()
                self.assign_provider()

                # Sleep for the determined amount of time
                sleep(self.sleep_on_provider_cycle)
            
            # We have a provider ? - Update the node!
            if self.current_provider is not None:
                self.update_available_pillars()

                # Did we lose the provider while updating? If not, update available nodes
                if self.current_provider is not None:
                    self.update_available_nodes()

                    # Did we lose the provider yet? No? Request messages
                    if self.current_provider is not None:
                        self.request_messages()

                        # If we still have the provider, make it provide a 
                        # receiving end for messages
                        if self.current_provider is not None:
                            self.push_messages()
                sleep(1)

    '''
        End the node, and bring down the thread
    '''
    def kill(self):
        if not self.supressText:
            print("\nPlease wait while node shuts down")
        # Write updates to db
        self.data_access.update_node_activity(
            "node",
            self.id,
            self.ip,
            self.last_pull,
            self.last_connect
            )
        # Kill and join
        self.signal = False
        self.join()

    '''
        Pull nodes and return (for interacting with another program)
    '''
    def external_pull_known_nodes(self):
        return self.data_access.get_nodes("node")

    '''
        Pull pillars and return (for interacting with another program)
    '''
    def external_pull_known_pillars(self):
        return self.data_access.get_pillars("node")

    '''
        Ensure the provider is who we think they are
    '''
    def validate_response(self, response, sender):
        
        key = self.data_access.get_keys("node", sender["id"])
        if len(key) != 1:
            return (False, "Couldn't load key from sender")

        return verify_data(key[0]["public"], response["hash"], response["signature"])


    '''
        Used to set-up the node to a network initially from 
        an external file - Someone using the library
    '''
    def force_register(self, pillar_id, pillar_ip, pillar_port):
        try:
            p = int(pillar_port)
        except ValueError:
            cout("fail", "Error: Port must be an integer!")
            return (False, "Not registered")
        
        # Add the pillar to the database
        self.data_access.add_pillar_info("node", pillar_ip, pillar_id, pillar_port, EnsureST(datetime.now()))

    '''
        Ping a pillar - Attempt 0,5 times
        True    - Success
        False   - Failure
    '''
    def perform_ping(self, address, port):
        port=int(port)
        for i in range(0, 5):
            result = query(address, port, __NETWORK_DATA_PING__, timeout=2)
            if ValidTCPRespnse(result):
                return True
        return False

    '''
        Assign new pillar contact - The 
        current interface into the network
    '''
    def assign_provider(self):
        if len(self.currently_available_pillars) == 0:
            return
        self.current_provider = GetRandomChoice(self.currently_available_pillars)
        self.cycles_since_active_provider = 1
        self.sleep_on_provider_cycle = 1

    '''
        Generic request
    '''
    def execute_request(self, request, receiver=None):
        if receiver is None:
            receiver = self.current_provider
        #print("REQ: ", receiver["ip"], "@", receiver["port"])
        port = int(receiver["port"])
        for i in range(0,5):
            result = query(receiver["ip"], port, request)
            #print(result)
            if ValidTCPRespnse(result):
                return(True, result)
        return(False, "Unable to send request")

    '''
        Ping for available pillars, and ensure registration
    '''
    def check_available_pillars(self):
        # Clear known pillars
        self.currently_available_pillars = []

        # Pull list of known pillars from db
        self.known_pillars = self.data_access.get_pillars("node")

        # Try pinging all pillars, ones that respond will have a registration
        # reqeust stent to them. If we can register, we add it to available
        for pillar in self.known_pillars:
            if self.perform_ping(pillar["ip"], pillar["port"]):

                # Ensure we are registered with the available pillar
                registration = create_message(self.private_key, "n.register", self.id, pillar["id"], self.ip + "<&>" + self.public_key)
                result_of_execute = self.execute_request(registration, receiver=pillar)

                if result_of_execute[0]:
                    result = disassemble_message(result_of_execute[1])
                    if result[0]:
                        if result[1]["data"] == "_MALFORMED_REGISTRATION_" or result[1]["data"] == "_MALFORMED_REGISTRATION_IP_ERROR_":
                            cout("fail", "Failure to comply with pillar's registration rules")
                        elif result[1]["data"] == "_REGISTERED_":
                                self.currently_available_pillars.append(pillar)
                        else:
                            # Ensure they gave us a key that can decode their signed hash
                            if verify_data(result[1]["data"], result[1]["hash"], result[1]["signature"]):
                                self.data_access.add_keys_info("node", result[1]["from"], result[1]["data"], "")
                                self.currently_available_pillars.append(pillar)
                            else:
                                cout("fail", "Pillar failed to give valid key")
                    else:    
                        cout("fail", "Unknown data response in regards to registration with " + pillar["id"])

    '''
        Request list of available nodes - update db
    '''
    def update_available_nodes(self):
        if self.current_provider is None:
            return
        request = create_message(self.private_key, "n.nodes", self.id, self.current_provider["id"], ".")
        nodes = self.execute_request(request)
        if not nodes[0]:
            cout("fail", nodes[1])
            self.current_provider = None
            return
        nodes = disassemble_message(nodes[1])
        if not nodes[0]:
            cout("fail", "[node->update_available_nodes] => invalid node data from pillar!")
            self.current_provider = None
            return

        try:
            nodes = loads(nodes[1]["data"])
        except:
            cout("fail", "Unable to loads node data from pillar")
            self.current_provider = None
            return

        # Get currently known nodes
        currently_known_nodes = self.data_access.get_nodes("node")

        # Check each incomming node against all known nodes. n2 :(
        for inode in nodes:
            
            incomming_node_exists_in_database = False
            for cnode in currently_known_nodes:
                if cnode["uid"] == inode["uid"]:
                    incomming_node_exists_in_database = True
                
            # If the incomming node doesn't exist in the database, add it
            if not incomming_node_exists_in_database:
                self.data_access.add_node_info("node",
                inode["uid"],
                inode["ip"],
                inode["last_pull"],
                inode["last_connect"],
                inode["port"]
                )
        # All incomming nodes checked, new should be added 
        return

    '''
        Request list of available pillars - update db
    '''
    def update_available_pillars(self):
        request = create_message(self.private_key, "n.pillars", self.id, self.current_provider["id"], ".")
        pillars = self.execute_request(request)
        if not pillars[0]:
            cout("fail", pillars[1])
            self.current_provider = None
            return
        pillars = disassemble_message(pillars[1])
        if not pillars[0]:
            cout("fail", "[node->update_available_pillars] => invalid node data from pillar!")
            self.current_provider = None
            return


####################################
######
     #   print("Got the following pillar data from the provider: \n", pillars[1]["data"])
######
####################################

        try:
            pillars = loads(pillars[1]["data"])
        except:
            cout("fail", "Unable to loads pillars data from pillar")
            self.current_provider = None
            return

        # Get currently known pillars
        currently_known_pillars = self.data_access.get_pillars("node")

        # Check each incomming node against all known nodes. n2 :(
        for ipillar in pillars:
            
            incomming_pillar_exists_in_database = False
            for cpillar in currently_known_pillars:
                if cpillar["id"] == ipillar["id"]:
                    incomming_pillar_exists_in_database = True
                
            # If the incomming node doesn't exist in the database, add it
            if not incomming_pillar_exists_in_database:
                self.data_access.add_pillar_info("node",
                ipillar["ip"],
                ipillar["id"],
                ipillar["port"],
                ipillar["last_online"]
                )
        # All incomming nodes checked, new should be added 

        return

    '''
        Request list of new messages
    '''
    def request_messages(self):
        if self.current_provider is None:
            return
        request = create_message(self.private_key, "n.check", self.id, self.current_provider["id"], ".")
        messages = self.execute_request(request)

        if not messages[0]:
            cout("fail", messages[1])
            self.current_provider = None
            return

        #debug_TODO("request_messages", "Add the new messages to current node messages! ")

        messages = disassemble_message(messages[1])

        # Invalid message data 
        if not messages[0]:
            cout("fail", "[node] => invalid message data from pillar!")
            self.current_provider = None
            return

       # print("Messages : ", messages)
        
        # No messages, no problem
        if messages[1]["data"] == "NO_MESSAGES":
            return
        else:
            # Data will be a stringed list of dictionaries
            try:
                current_messages = loads(messages[1]["data"])
            except:
                cout("fail", "Unable to loads messages from pillar")
                self.current_provider = None
                return

            # Add each dict into the queue
            for cmsg in current_messages:
                self.received_messages.put(cmsg)

    '''
        Send a message to a node on the network
            - Take in - assemble message and add to outgoing queue
    '''
    def send_message(self, receiver, data, previous_hash=None):
        message = create_message(self.private_key, "n.submit", self.id, receiver, data, previous_hash=previous_hash)
        self.outgoing_messages.put(message)

    '''
        Send a message to a network of nodes
            - Take in - assemble message and add to outgoing queue
    '''
    def broadcast_message(self, receivers, data, previous_hash=None):
        message = create_message(self.private_key, "n.broadcast", self.id, receivers, data, previous_hash=previous_hash)
        self.outgoing_messages.put(message)

    '''
        Send all queued messages
    '''
    def push_messages(self):
        
        # Ensure again that we have a provider
        if self.current_provider is not None:
            
            try_flag = True
            failed_attempts = 0
            original_size = self.outgoing_messages.qsize()
            
            # If we do, and we have messages, send them out
            while not self.outgoing_messages.empty() and try_flag:
                
                success = False
                message = self.outgoing_messages.get()


##################################
                debug_MESSAGE("push_messages", "sending messages...")
                debug_MESSAGE("push_messages", "Queue Size: " + str(original_size))
                
                print("TRYING TO SEND: ", message)
##################################

                for i in range(0, 5):
                    
                    result = query(self.current_provider["ip"], int(self.current_provider["port"]), message)
                    success = ValidTCPRespnse(result)

                    if success:
                        self.outgoing_messages.task_done()
                        break

                if success == False:
                    self.outgoing_messages.task_done()
                    self.outgoing_messages.put(message)
                    failed_attempts += 1
                
                # Ensure that if pillar goes down while sending messages, we only fail
                # so-many times before stopping, allowing us to cycle providers
                if failed_attempts > (original_size + (original_size/2) ):
                    try_flag = False