#!/usr/bin/python3

from time import sleep
from sys import argv
from sys import exit
import urllib.request as ur

# Globally used static information
from xnscore import Core
from xnscommon import settings
from xnsdisplay import cout, ctxt
from xnscomm import serverMonitor, NetworkTransmitter
from xnsdevices import DeviceManager
from xnshci import HumanInterface
from xnsdevices import NodeNexus

class MainEngine:

    def __init__(self):


        self.running = True

        '''
            Network Transmitter has the functionality
            of en/decrypting messages, and sending messages to
            other nodes. The method "outgoingQuery" handles
            encryption of data, and sending the data to another 
            node (It handles the handshake process)
        '''
        self.transmitter = NetworkTransmitter(settings)

        # Handle running modes

        # Direct human interface (cli)
        if len(argv) == 1:
            self.initiateThreads()
            self.interface_human()
            self.perform_shutdown()

        # Silent - (No output, no control at all. Just serve info)
        elif len(argv) == 2:
            # Silent!
            if argv[1] == "silent" or argv[1] == "s":
                self.initiateThreads()
                self.interface_none()
                self.perform_shutdown()

        #
        #   NOT YET COMPLETED
        #
        #
        # Run - Load a file into the core and begin rotating it
        elif len(argv) == 4:
            
            if argv[2] == "run":
    
                # Ensure that the core handles its own rotation
                self.core_th.handle_rotation = True

                print("This is where we need to load the file into the core")
                exit(1)

        else:
            a = '''


        Options for XNS entry are :

            ./entry  ........................... Starts in cli mode

            ./entry [silent | s] ............... Start in silent, passive mode

            ./entry run <file> ................. Load a file into core, and process


            '''
            print(a)
            exit(1)

    def initiateThreads(self):
        
        '''
            The core, rather than being the center of all operations
            is the center of all user-defined operations. The core's
            purpose is to interperet user scripts that handle the 
            interplay between nodes on the cluster. 
        '''
        cout("yellow", "Starting core...", "\t\t ->")
        self.core_th = Core()
        self.core_th.signal = True
        self.core_th.daemon = True
        self.core_th.start()
        cout("lightgreen", "OK")
        
        '''
            Device manager cycles device routines and stores
            returned signal information, it also handles
            all requests from remote nodes in querying slots, ticketing,
            signal info, and responding to updates
        '''
        cout("yellow", "Starting device manager...", "\t ->")
        self.device_man_th = DeviceManager(self.core_th)
        self.device_man_th.signal = True
        self.device_man_th.daemon = True
        self.device_man_th.start()
        cout("lightgreen", "OK")

        '''
            The Nexus is the watchdog that continually monitors 
            the state between this node, and the others within the 
            cluster to ensure activity between nodes is present,
            and that their information is up-to-date in local records
        '''
        cout("yellow", "Starting node nexus...", "\t ->")
        self.nexus_th = NodeNexus()
        self.nexus_th.daemon = True
        self.nexus_th.start()
        cout("lightgreen", "OK")

        '''
            Listens in on XNS's active port defined in 
            xnscommon/node_settings.json. TCP Server handles
            decryption and handshakes of all requests between this
            node and the cluster. All requests, once validated
            are routed by xncscomm/xnsrequesthandler and processed
            by xnsdevices/DeviceManager
        '''
        cout("yellow", "Starting TCP server...", "\t ->")
        self.tcp_serv_th = serverMonitor(self.device_man_th)
        self.tcp_serv_th.signal = True
        self.tcp_serv_th.daemon = True
        self.tcp_serv_th.start()
        cout("lightgreen", "OK")
        
    # Interfacing with a human
    def interface_human(self):
        self.interface_type = "human"
        self.prompt = ctxt("cyan", settings["name"]) + "@" + ctxt("yellow", settings["address"])
        self.hci = HumanInterface(self.prompt, self.nexus_th, self.core_th)
        self.hci.begin()
        # End Of Session

    # Interfacing with nothing. Just running
    def interface_none(self):
        self.interface_type = "silent"
        while(self.running):
            try:
                sleep(60)
                pass
            except KeyboardInterrupt:
                self.running = False
        # End Of Session

    # Stop everything, and shutdown
    def perform_shutdown(self):
        
        # Stop the server thread
        cout("yellow", "\n ...Killing TCP server...", "\t  ->")
        self.tcp_serv_th.signal = False
        self.transmitter.outgoingQuery(settings["address"], settings["port"], "__HALT__")
        self.tcp_serv_th.join()
        cout("lightgreen", " OK")

        # Stop the nexus thread
        cout("yellow", "...Killing node nexus... ", "\t  ->")
        self.nexus_th.signal = False
        self.nexus_th.join()
        cout("lightgreen", " OK")

        # Stop the device monitor thread
        cout("yellow", "...Killing device manager...", "\t  ->")
        self.device_man_th.signal = False
        self.device_man_th.join()
        cout("lightgreen", " OK")

        # Stop the core
        cout("yellow", "...Killing core... ", "\t\t  ->")
        self.core_th.signal = False
        self.core_th.join()
        cout("lightgreen", " OK")

# Start the engine
MainEngine()
