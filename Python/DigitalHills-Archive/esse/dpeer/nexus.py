'''
    Copyright 2017 Digital Hills, LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
'''

import logging
import threading
from time import sleep
from queue import Queue
from json import loads, dumps
from datetime import datetime
from datetime import timedelta
from datetime import timezone
from random import choice, SystemRandom

from tcplib import query
from settings import targeting
from settings import node_settings
from display import cout, ctxt

# How much propagation history to take. Enforce int type
__NEXUS__PROP_HISTORY_LENGTH__ = 2000
__NEXUS__PROP_HISTORY_CUT__ = int(__NEXUS__PROP_HISTORY_LENGTH__ / 2)

# Network variables
__NEXUS__PING_TIMEOUT__ = 1
__NEXUS__PING_CAP__ = 10

# Time Variables
__NEXUS__BASE_EPOCH_SECONDS__ = 40

# Console out peers dropping offline
__NEXUS__SHOW_PEER_DROPS__ = False

class PeerNexus(threading.Thread):
    def __init__(self, memPool, chain):
        threading.Thread.__init__(self)
        self.signal = True

        # A logger
        self.logger = logging.getLogger(__name__)

        # Supress console messages from Nexus
        self.supressText = False

        '''
            Time Information
        '''
        # Initialization time 
        self.uptime = datetime.utcnow()
    
        # Network time - Exists as None until on a network
        self.utc_network = None

        # Block epochs for PoT
        self.next_epoch = None
        self.prev_epoch = None

        # Number of epochs we've been alive for
        self.epochs_passed = 0

        # Get seconds from a time difference
        #   use:  self.timeDeltaToSeconds(endTime - StartTime)
        self.timeDeltaToSeconds = lambda x: int(str(x).split(".")[0].split(":")[2])

        '''
            References to the DHNode Information 
            -memPool = Transaction pool (yet-to-be-mined)
            -chain   = The current block chain
        '''
        self.memPool = memPool
        self.chain = chain

        '''
            Network/ Peer Information
            -propagated = A list that ensures we don't send 
                previouslt sent requests
            -peers = List of active (online) peers
        '''
        # Things that we've already propageted
        self.propagated = []

        # All known peers - Load from whatever file
        self.peers = []

        # Know thyself - Ensure we list ourselves as a node. 
        # Ignored by the '_ALL_' propagation tag
        self.addNewPeer("127.0.0.1", node_settings["port"], self.next_epoch, self.uptime)

        # Eventual constant node
        self.addNewPeer("digitalhills.space", node_settings["port"], None, None)

        # Different outgoing types to talk with peers
        self.peerQueueEvent = {
            "sync_MemPool": "smp",
            "sync_NextBlock": "snb",
            "sync_KnownNodes": "skn",
            "sync_BlockChain": "sbc",
            "info_ReleasedBlock": "irb",
        
        # Programmed into Handler or in-use elsewhere
            "ping_peers": "pp",
            "data_propagate": "dp"
        }

        # Queue of all things that need to be pushed to all peers
        # - Updates to mempool, current block set aside (about to go out - to sync all blocks released)
        #   Messages will be sent every cycle of self.run
        self.pushToPeers = Queue()


        '''
            Before we start running, load a list of expected peers, and attempt contact
            The first ones we come in contact with will be our basis of synchronization
        '''
        # Load file
        # Attempt pings
        # If accept: Sync block chain - Sync epochs

        cout("cyan", "\n> Initializing Peer Nexus\n> Attempting to join known peers...")
        known_nodes = None
        try:
            known_nodes = open("dpeer/known_nodes.json", "rb").read().decode('utf-8')
        except IOError:
            cout("fail", "Unable to open known peers file - Assuming peer is alone")
        if known_nodes is not None:
            try:
                known_nodes = loads(known_nodes)
            except:
                cout("fail", "Error loading known_nodes.json p Assuming peer is alone")
                known_nodes = None

        # Add nodes from file
        for node in known_nodes:
            self.addNewPeer(node["address"], node["port"], node["epochTime"], node["personalTime"])

        # Attempt to sync with all nodes
        self.synchronizeEpoch()

    '''
        Push an event into the pushToPeers Queue
    '''
    def addItemToPeerQueue(self, _peerQueueEvent, _peer, data=None):
        self.pushToPeers.put((_peer, _peerQueueEvent, data))

    '''
        Add new peer
    '''
    def addNewPeer(self, address, port, epochTime, personalTime):
        
        # To ensure nobody attempts changing what we think about ourselves
        if address == "127.0.0.1":
            port = node_settings["port"]
            epochTime = str(self.next_epoch)
            personalTime = str(self.uptime)
            
        # Use address + str(port) to uniquely identify nodes behind same address
        cleanse = None
        for peer in self.peers:
            if (peer["address"] + str(peer["port"])) == address + str(port):
                cleanse = address + str(port)
        
        # If the item exists, remove it to append with new information!
        if cleanse is not None:
            self.peers = [peer for peer in self.peers if (peer["address"] + str(peer["port"])) != cleanse]
        
        # Append the peer as-if its a new peer
        self.peers.append({
            "address": str(address),
            "port": int(port),
            "lastConnect": str(datetime.now()),
            "failedPings": 0,
            "online": True,
            "epochTime": epochTime,
            "personalTime": personalTime
        })

    '''
        Perform all pushes to peers
            - Should only be called by this thread's run loop
    '''
    def __pushEventsToPeers(self):
        while not self.pushToPeers.empty():
            
            # Get an event from the queue, and break into its pieces
            peerEvent = self.pushToPeers.get()
            _peer = peerEvent[0]
            _event = peerEvent[1]
            _data = peerEvent[2]

            '''
                Build list of peers that will be receiving the information
            '''
            outgoing = []
            if _peer == "_ALL_":
                outgoing = [peer for peer in self.peers if peer["address"] != "127.0.0.1"]
            else:
                ''' Find the peer to ensure it exists - Here we accept sending to local address '''
                for _p in self.peers:
                    if _p["address"] == _peer:
                        outgoing.append(_p)
                if len(outgoing) == 0:
                    self.logger.info("<Nexus unable to locate peer [{}] in peer list>".format(_peer))
                    if not self.supressText:
                        cout("fail", ("<Nexus unable to locate peer [{}] in peer list>".format(_peer)))
                    return

            '''
                Push the data to the selected peers
                 - Handle all self.peerQueueEvent

            '''
            for _p in outgoing:
                '''
                    Ping Request
                '''
                if _event == self.peerQueueEvent["ping_peers"]:
                    query(_p["address"], _p["port"], "*", self.logger)




                # Need to add all self.peerQueueEvent items here



                '''
                    Data Propagation Request
                '''
                if _event == self.peerQueueEvent["data_propagate"]:
                    if _data == None:
                        self.logger.warning("Propagation of 'None' data triggered")
                    else:
                        self.logger.info("Propagating data")
                        if _p["address"] != "127.0.0.1":
                            query(_p["address"], _p["port"], _data, self.logger)
                            self.propagated = _data
                        
            # Indicate that the task from the queue is compelete
            self.pushToPeers.task_done()

    '''
        Trigger shutdown 
    '''
    def triggerShutdown(self):
        self.signal = False

        # We are at the end of the run, write out active nodes
        try:
            open("dpeer/known_nodes.json", "w+").write(dumps(self.peers, indent=4))
        except IOError:
            cout("fail", "Unable to save TX to file.")

    '''
        Request handler
            - Handles all incomming requests from other peers
            - Need to send back string-type even on failures
            - The events handled here are listed in :
                    Documentation -> RequestProtocol.md
    '''
    def handle(self, data):
        '''
            Ping received - Send back a pong
        '''
        if data == b'*':
            return "___PONG___"

        '''
            Split the data into chunks, each request should follow the
            request protocol chunking defined in 'RequestProtocol.md'
        '''
        original_data = data
        data = data.decode()
        data = data.split('\n')
        if len(data) < 3:
            return("_INVALID_DATA_CHUNK_")

        # Ensure that we don't repeat ourselves
        if original_data in self.propagated:
            self.logger.info("Handler got request to propagate previously propageted data")
            return("_PREVIOUSLY_PROPAGETED_")

        '''
                Information Pushed (IP) - Broadcast received
        '''
        if data[0] == "broadcast":
            '''     
                IP.0    
            '''
            if data[1] == "new_client":
                # Ensure there isn't anything silly with \n happening
                ensured = ''.join(data[2:])
                try:
                    ensured = loads(ensured)
                except:
                    return ("_INVALID_UNABLE_TO_LOADS_DATA_")
                
                try:
                    addr = ensured["address"]
                    prt = ensured["port"]
                except:
                    return("_INVALID_JSON_DATA_")

                # Attempt to ping the peer they asked us to add. If we can't reach them, we wont try
                res = None or query(addr, prt, "*", self.logger)
                if res is None or res == -1:
                    return ("_CANT_REACH_GIVEN_PEER_")

                try:
                    self.addNewPeer(ensured["address"], ensured["port"], ensured["epochTime"], ensured["personalTime"])
                except:
                    return("_INVALID_JSON_DATA_")

                # Propagate peer to network    
                self.addItemToPeerQueue(self.peerQueueEvent["data_propagate"], "_ALL_", original_data)
                return("_REGISTERED_")

            '''     
                IP.1    
            '''
            if data[1] == "new_transaction":
                # Add to mempool, and propagate
                _memaddresult = self.memPool.insert_transaction(''.join(data[2:]))
                if _memaddresult == "_NEW_TX_CREATED_":
                    self.addItemToPeerQueue(self.peerQueueEvent["data_propagate"], "_ALL_", original_data)
                return _memaddresult
            

        '''
                Synchronization Request (SR)
        '''
        if data[0] == "synchronize":
            print("\nA synchronization request was picked up!")

        '''
                Information Request (IR)
        '''
        if data[0] == "information":
            ''' IR.0 '''
            if data[1] == "timestamp":
                return str(datetime.now())
            ''' IR.1 '''
            if data[1] == "numBlocks":
                return str(self.chain.head)
            ''' IR.2 '''
            if data[1] == "numPeers":
                return str(len(self.peers))
            ''' IR.3 '''
            if data[1] == "numPool":
                temp = self.memPool.request_pool_size()
            ''' IR.4 '''
            if data[1] == "uptime":
                return str(self.uptime)
        
        cout("fail", "\tNEXUS->Handle->data:")
        cout("lightgreen", data)

        '''
            If we reach the bottom, that means there was an issue with the 
            data, return something to indicate the issue
        '''
        return "_NULL_"

    '''
        Sync the EPOCH time and network time with peers
    '''
    def synchronizeEpoch(self):
        cout("fail", "NEED TO SYNC WITH PEERS ON NEXUS LAUNCH - NYP")

        if not self.supressText and self.prev_epoch is None and self.next_epoch is None:
            cout("yellow", "...Attempting to sync from previously known nodes...")

        # Nodes to attempt conenct with
        attempt = []

        # Attempt ping 
        for peer in self.peers:
            res = None or query(peer["address"], peer["port"], "*", self.logger, timeout=2)
            if res is None or res == -1:
                attempt_sync = False
                if not self.supressText:
                    print(
                        ctxt("fail", "Failed to ping ["),
                        ctxt("yellow", (peer["address"] + "@" + str(peer["port"]))),
                        ctxt("fail", "] - Wont attempt to sync")
                    )
            else:
                attempt_sync = True

            if attempt_sync and peer["address"] != "127.0.0.1":
                if not self.supressText:
                    cout("yellow", "Adding external peer to attempt sync with")
                attempt.append[peer]

        if not self.supressText:
            cout("yellow", ("Ping-scan for sync complete, " + str(len(attempt)) + " nodes to attempt sync with"))

        # We are either starting alone, or we were cut off from the network
        if len(attempt) == 0:
            self.next_epoch = None
            return

        # Grab random peer that is available to sync with
        sysRand = SystemRandom()
        peer_to_sync = sysRand.choice(attempt)

        print("NOT YET DONE ------ ")
        print("Sync with: ", peer_to_sync)

    '''
        Epoch triggered, perform mining process
    '''
    def performCurrentEpoch(self):
        self.epochs_passed += 1
        return

    '''
        Main running loop
    '''
    def run(self):
        cout("lightgreen", "> Peer Nexus Online <")

        # If we dont sync on launch, this will be true
        if self.next_epoch is None and self.prev_epoch is None:
            print( 
                    ctxt("fail", "(No peers discovered)") +
                    ctxt("yellow", " . Assuming pillar status at [") + 
                    ctxt("cyan", str(datetime.now())) + 
                    ctxt("yellow", "]")
                )
            self.prev_epoch = datetime.utcnow()
            self.next_epoch = self.prev_epoch + timedelta(seconds=__NEXUS__BASE_EPOCH_SECONDS__)


        while self.signal:
    
            '''
                Ensure list of propagated data isn't getting out of hand
            '''
            if len(self.propagated) > __NEXUS__PROP_HISTORY_LENGTH__:
                self.propagated = self.propagated[__NEXUS__PROP_HISTORY_CUT__:]

            '''
                Ping all online peers to ensure connectivity
            '''
            for peer in self.peers:
                res = None or query(peer["address"], peer["port"], "*", self.logger, timeout=__NEXUS__PING_TIMEOUT__)
                if res is None or res == -1:
                    peer["failedPings"] += 1
                else:
                    peer["online"] = True

                if peer["failedPings"] > __NEXUS__PING_CAP__:
                    peer["online"] = False 

                    # Indicate their change in status
                    if not self.supressText and __NEXUS__SHOW_PEER_DROPS__:
                        print(
                            ctxt("fail", "Peer ["), ctxt("yellow", peer["address"]), 
                            ctxt("fail", "] has gone offline.")
                        )
                    self.logger.info(("Peer [" + peer["address"] + "] went offline"))
                
            # Only keep online peers
            self.peers = [peer for peer in self.peers if peer["online"]]
            
            '''
                Push events to all online peers
            '''
            if not self.pushToPeers.empty():
                self.__pushEventsToPeers()

            '''
                Check for EPOCH
            '''
            if self.next_epoch <= datetime.utcnow():
                
                # Perofrm the current epoch
                self.performCurrentEpoch()

                # Once we have mined the block, and ensured changes set, set next epoch
                self.prev_epoch = self.next_epoch
                self.next_epoch = datetime.utcnow()+ timedelta(seconds=__NEXUS__BASE_EPOCH_SECONDS__)

            # No, the CPU doesn't like grindcore
            sleep(0.1)
