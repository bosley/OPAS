import threading
from time import sleep
from xnsdisplay import cout, ctxt
from xnscommon import settings
from xnscomm import NetworkTransmitter
from xnsdb import DataStore

from .xSRH import SrhInterface

class state:
    BINDING = 0

'''
    Relays are expected to have significant up-time
    Ephems have potentially volatile behaviour
'''
class classification:
    RELAY = 0
    EPHEMERAL = 1

'''
    The threaded Beacon class uses xSRH on port 8600 to serve
    on the beacon network. The keys for accessing the network are currently 
    harcoded, and should be moved to a setting, along with the port 8600.
    Making that dynamic may enable the use of multiple beacon servers
    to increase transmission rate. (Get network addressing down first :P )
'''
class Beacon(threading.Thread):

    def __init__(self, first_bind):
        threading.Thread.__init__(self)
        self.signal = True

        self.__setupGni()

        '''
                     HAVE TO BIND TO THE BEACON THE USER SPECIFIED, OR SELF
                     IF RELAY, TO BEGIN COMMUNICATION OF REGISTERING ALL KNOWN 
                     GLOBAL NODES. 

                        I.E 
                        A IS_UNATTATCHED
                        B IS_ATTATCHED C, D, E
                        A ATTATCHES_TO B BY WAY OF USER SPECIFICATION
                        A AUTO_ATTATHES_TO C, D, E

        '''
        print("Starting beacon need to bind to : ", first_bind, ". <NYD>")

        self.current_state = state.BINDING

        if first_bind == settings["address"] or first_bind == settings["nest_address"]:
            print("RELAY beacon setup : Load previously known routes from db and attempt to reconnect")
        else:
            print("EPHEMERAL beacon setup : Load nest info and BIND to the address [first_bind]")

        ''' 
            Other beacons we are attatched to 
            This will hold the address of that beacon, and all the private nodes
            attatched to it, but not to this beacon. This is to enable routing of
            requests from one private network, to another

            (address, attatched_nests = (nest_address, [nested nodes,...,...]))
        '''
        self.attatched_beacons = []


        '''
            Routing requests can come from another beacon that we're attatched to


            (from, to_nest, to_node_in_nest, data)
        '''
        self.routing_requests = []


    def __setupGni(self):

        self.gs_settings = {
            "handshake_key": "9aB4Jx59rh1WB9tIGKAg7k1fI8UD0bb4J4",
            "encryption_key": "1e4ZgSLUcMOhNgWDMOGnRfln0JSlQM29",
            "network_transmission_timeout": 20,
            "verification_block": 1024,
            "accepted": "0x0000",
            "error": "0x0001",
            "address": "127.0.0.1",
            "port": 8600,
        }

        # Interface 
        self.gni = SrhInterface(settings=self.gs_settings, redirect=self.__globalInterfaceHandler)

    ''' Thead main body '''
    def run(self):
        
        # Serve as a beacon until the signal is cut
        while self.signal:
            
            # Run a command here to reload the attatched_beacons from datastore

            sleep(0.5)

        
        # Disconnect from the network
        cout("yellow", "\n\n...Disconnecting from global network...")
        self.gni.kill()
        print("")
        cout("lightgreen", "Disconnected\n\n")

    def logMessage(self, message):
        ds = DataStore()
        print(message)
       # ds.addLogEntry("Beacon", message)

    ''' Handles all incomming TCP connections '''
    def __globalInterfaceHandler(self, settings, data):

        print("Beacon got data: ", data)

        router = {
            0: self.__bindingRequest,
        }

        # Split the data, by the standard delimter
        data = data.split("$")

        # Make sure something silly didn't come through
        if len(data) == 0:
            return settings["error"]

        # Get the routing id
        try:
            rid = int(data[0])
        except ValueError:
            if str(data[0]) == "__HALT__":
                return settings["accepted"]
            else:
                self.logMessage("Unable to translate id in __globalInterfaceHandler")
                return settings["error"]

        # Get the routing function
        try:
            route = route[rid]
        except KeyError:
            self.logMessage("Key error when routing connection in __globalInterfaceHandler")
            return settings["error"]

        # Remove the route id from the data
        data = data[1:]

        # Route the data
        route(settings, data)

        '''
                Detect, and handle binding request from node. 
                    This will involve the remote node sending their nest hash, and nest id
                    if the is is new (not in storage), store it and tie it to their hash.
                    If the id exists, check the hash to determine if changes were made. 
                    If changes in the hash are present, update the nest info.

        '''

        return settings["error"]

    def __bindingRequest(self, settings, data):

        '''

                [beacon_from_address, nest_hash, nest_directory]

        '''


        print("Got a binding request !")
        print(" The data that came with is is : ", data)

        #( Perhaps just store the relative address )
        # If the beacon_from_address exists, in the db, update with info
        # If it doesn't exist, insert it.

        # Send the beacon back the network directory this beacon has compiled (Knowledge of all others)

        # Once saved, all set. The 'RUN' execution method will update the known node lists 

        return settings["error"]