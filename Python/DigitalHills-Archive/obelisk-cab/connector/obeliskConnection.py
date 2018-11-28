from .xnmlib import XNMNode, DataStore
from .channel import channel

class _obelisk_connection:
    def __init__(self, connection_info):
        self.channel = channel()
        self.connection_info = connection_info

    def setup(self):
        # Check for node in connection info [0], if it doesnt exist, generate it.
        if self.connection_info[2] is None:
            return False    
        try:
            parts = self.connection_info[0].split("@")
            self.remote_address = parts[0]
            self.remote_ip = parts[1]
        except:
            print("Incorrect obelisk receiver! Required : 'id-<hash>@ip'")
            return False
        self.port = self.connection_info[1]
        self.node_network = self.connection_info[2]
        self.node_id = None
        self.add_remote = True
        # if node exists, pull it. If not, load it and register it
        self.data_access = DataStore()
        nodes = self.data_access.get_nodes("node")
        for x in nodes:
            if x["uid"].split("-")[0] == self.node_network:
                print("Load an existing node")
                self.node_id = x["uid"]
                self.add_remote = False
                break

        if self.node_id is None:
            print("create  a new node")
            self.node_id = XNMNode(self.node_network, True, self.connection_info[1]).naddress

        # load a node with the channel received queue as the message storage queue
        print("load node")
        self.node = XNMNode(self.node_id, qoverride=self.channel.received)

        if self.add_remote:
            print("adding remote")
            self.node.force_register(self.remote_address, self.remote_ip, self.port)

        self.node.supressText = False
        self.node.start()
        return True

    '''
        Triggered by connector to send data
    '''
    def output(self, data):
        self.node.send_message(data[0], data[1])
