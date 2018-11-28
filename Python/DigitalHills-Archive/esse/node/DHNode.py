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

from chain import blockchain
from chain import MemPool
from display import ctxt, cout
from dpeer import PeerNexus
from tcplib import tcpServerInstance
from tcplib import query
from settings import node_settings
from client import EsseClient

from os import system, name
from sys import stdin
from select import select
from time import sleep
import logging
from datetime import datetime

from client import EsseClient

logging.basicConfig(format='%(asctime)s [DHNode]: %(message)s <%(levelname)s>')

class DHNode:
    def __init__(self, _mode="node"):
        self.operating = True
        self.chain = blockchain()

        # A logger
        self.logger = logging.getLogger(__name__)

        # Area for pending transactions 
        self.memPool = MemPool()

        # Start the threads up
        self.startThreads()

        # Start in specified mode
        if _mode == "node":
            self.interface()
        elif _mode == "cli":
            self._th_peer_nexus.supressText = True
            EsseClient(self._th_peer_nexus)
            self.shutdown()
        elif _mode == "gui":
            print('''
            
            [ START TKINTER WINDOW HERE ]

            ''')

    def startThreads(self):
        # Start the peer nexus, give it a reference to the pool and chain
        self._th_peer_nexus = PeerNexus(self.memPool, self.chain)
        self._th_peer_nexus.daemon = True
        self._th_peer_nexus.start()

        # Start TCP server
        self._th_tcp_server = tcpServerInstance(
            node_settings["local"], node_settings["port"], self._th_peer_nexus
            )
        self._th_tcp_server.signal = True
        self._th_tcp_server.daemon = True
        self._th_tcp_server.start()

    '''
        Command line interface to the node
        -----------------------------------------
    '''
    def interface(self):
        
        # Setup screen clearing command
        self.cls = "clear"
        if name == 'nt':
            self.cls = "cls"

        # Main thread's running loop
        while self.operating:
            try:
                command = input(ctxt("cyan", "esse-node> "))

                '''
                        Basic commands
                '''
                if command  == "exit" or command == "quit" or command == "q":
                    self.shutdown()
                if command == "clear":
                    system(self.cls)

                if command == "help" or command == "h":
                    print('''
    Help Menu:
    ---------------------------------------             
    test net        ->  Test network ops
    test bc         ->  Test blockchain operations
    display screen  ->  Show an active display of node information
    exit            ->  Shutdown the node
    --------------------------------------- 

                    ''')

                '''
                        Command Routing
                '''
                cmd = command.split(" ")

                if cmd[0] == "test":
                    self.__execute_test(cmd)

                if cmd[0] == "display":
                    self.__execute_display(cmd)

                '''
                        - TESTING COMMANDS
                            > Temporary commands meant to be removed before release
                '''
                if command == "TEST_ADD_TO_QUEUE_ONE":
                    self._th_peer_nexus.addItemToPeerQueue(self._th_peer_nexus.peerQueueEvent["sync_NextBlock"], "127.0.0.1")
                if command == "TEST_ADD_TO_QUEUE_ALL":
                    self._th_peer_nexus.addItemToPeerQueue(self._th_peer_nexus.peerQueueEvent["sync_KnownNodes"], "_ALL_")
                '''
                        - END TESTING COMMANDS
                '''

            except KeyboardInterrupt:
                self.shutdown()

    '''
        Execution of commands
        -----------------------------------------
    '''
    def __execute_test(self, data):
        if len(data) != 2:
            print("Nothing given to test....")
            return

        '''
            Test network functionality
        '''
        if data[1] == "net":
            from json import dumps
            cout("cyan", "\t[Broadcast]")
            cout("yellow", "\n\nSending general broadcast to self=>")

            ownSelf = dumps({
                "address": "127.0.0.1",
                "port": 9000,
                "lastConnect": str(datetime.utcnow()),
                "failedPings": 0,
                "online": True,
                "epochTime": None,
                "personalTime": None
            })
            res = query("127.0.0.1", node_settings["port"], "broadcast\nnew_client\n" + ownSelf + "\n", self.logger)
            cout("lightgreen", res)


            cout("cyan", "\n\t[Synchronize]")
            cout("yellow", "\nSending general synchronize to self=>")
            query("127.0.0.1", node_settings["port"], "synchronize\nblockchain\n{\"head\": \"<current head>\", \"height\":0}\n", self.logger)

            cout("cyan", "\n\t[Information]")
            cout("yellow", "\nSending general information request to self [IR.4] =>")
            res = query("127.0.0.1", node_settings["port"], "information\nuptime\n", self.logger)
            print(ctxt("yellow", "\nReceived [",), ctxt("blue", str(res)), ctxt("yellow", "] from self.") )

        if data[1] == "bc":
            from tests import fulltest
            fulltest()

    def __execute_display(self, data):
        if len(data) != 2:
            print("No request for display")
            return

        #system(self.cls)
        if data[1] == "peernum":
            print("There are [", ctxt("yellow", str(len(self._th_peer_nexus.peers))), "] peer(s) online.")

        if data[1] == "screen":
            self._th_peer_nexus.supressText = True
            i = 0
            # A really bad way to find seconds passed :-)
            td = lambda x: str(x).split(".")[0].split(":")[2]
            while True:
                system(self.cls)
                cout("fail", "[Press 'RETURN' to exit]\n")
                cout("lightgreen", "Uptime :\t\t\t", "")
                cout("yellow", str((datetime.utcnow() - self._th_peer_nexus.uptime)))
                cout("lightgreen", "Connected peers:\t\t", "")
                cout("yellow", str(len(self._th_peer_nexus.peers)-1))
                cout("lightgreen", "Blockchain height:\t\t", "")
                cout("yellow", self.chain.head)
                cout("lightgreen", "Items in pool:\t\t\t", "")
                cout("yellow", self.memPool.request_pool_size()[1])
                cout("lightgreen", "Epochs passed:\t\t\t", "")
                cout("yellow", str(self._th_peer_nexus.epochs_passed))
                cout("lightgreen", "Last Epoch:\t\t\t", "")
                cout("yellow", str(self._th_peer_nexus.prev_epoch))
                try:
                    tdEpoch = td(self._th_peer_nexus.next_epoch - datetime.utcnow())
                except:
                    tdEpock = "NaN"
                cout("lightgreen", "Next Epoch:\t\t\t", "")
                cout("yellow", tdEpoch)
                sleep(1)
                if stdin in select([stdin], [], [], 0)[0]:
                    x = input()
                    break
                i += 1
            self._th_peer_nexus.supressText = False
            
    '''
        Shutdown the node
        -----------------------------------------
    '''
    def shutdown(self):
        cout("cyan", "\n...Shutting down, this might take a moment...")

        # Shutdown TCP server
        cout("yellow", "\nShutting down TCP server...", "")
        self._th_tcp_server.signal = False
        query(node_settings["local"], node_settings["port"], "*", self.logger)
        self._th_tcp_server.join()
        cout("lightgreen", "OK")

        # Shutdown Nexus
        cout("yellow", "\nShutting down Peer Nexus...", "")
        self._th_peer_nexus.triggerShutdown()
        self._th_peer_nexus.join()
        cout("lightgreen", "OK")

        # Signal that we are no-longer operating
        self.operating = False
