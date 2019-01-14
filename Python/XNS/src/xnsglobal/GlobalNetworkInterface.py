'''

    Josh A. Bosley
    The GlobalNetwork class creates a beacon thread, and 
    acts as an intermediary between the user, and the beacon's
    routing ability. The beacon object, when bound to other beacons
    that are in a global, or private nest acts as a routing system
    between them. This enables the user to interact with other
    private networks.

    0, 1 = private nests
    a, b, c, d  = private nodes 
    g = global beacon

    a --- 0) --- g
                |
    b --- 1) ---/
    | \
    c  d
    Now, a can talk to b, x, or d by way of : g->b->

'''


from os import system

from xnsdisplay import cout, ctxt
from xnscommon import settings
from xnscomm import NetworkTransmitter

from .xSRH import SrhInterface
from .Beacon import Beacon

class GlobalNetwork:

    def __init__(self, beacon, local_nexus):
        self.beacon = beacon
        self.local_nexus = local_nexus
        self.global_nexus = []

        self.signal = True
        self.prompt = ctxt("cyan", settings["name"]) + "@" + ctxt("red", "global") + "[" + ctxt("yellow", beacon["nest_address"]) + "] > "

        self.args = None
        self.command_map = {
            "help": self.showHelp,
            "clear": self.clearScreen,
        }

        # An instance of the beacon for routing messages to/fro this network to other networks
        self.beacon_th = Beacon(self.beacon["address"])
        self.beacon_th.start()

        # Start eating human inpus
        self.main()

    ''' Main user inpue '''
    def main(self):
        while self.signal:
            x = input(self.prompt)
            self.args = x.split(" ")[1:]
            x = x.split(" ")[0]

            if x == "":
                continue
            if x == "q" or x == "quit":
                self.signal = False
            else:
                try:
                    self.command_map[x]()
                except KeyError:
                    cout("red", "> Unknown command. ")
                self.args = None

        # Trigger disconnect
        self.beacon_th.signal = False
            
    def clearScreen(self):
        system("clear")

    def showHelp(self):
        print('''
        
    help            - help message
    quit            - disconnect from global
    clear           - clear screen
        
        ''')