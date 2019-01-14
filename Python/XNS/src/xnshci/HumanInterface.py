'''

    Josh A Bosley
    This is the human computer interface, or aka the cli.
    This gives full access to the person at the keyboard for
    handling all aspects of a running node. 

    Note: In this mode, a file is not loaded into the core
          unless specifically done so.

'''

from sys import stdin 
from time import sleep
from select import select
from datetime import datetime
from json import loads as jloads
from json import dumps as jdumps
from os import name as oname, system as osys

from xnsdb import DataStore
from xnscomm import startScan
from xnscomm import manScan
from xnsdisplay import cout, ctxt
from xnscommon import settings
from xnscommon import device_settings
from xnscommon import OmniClock
from xnscomm import NetworkTransmitter
from xnscomm import probeInfo
from xnsdisplay import header_main, header_help, header_info
from xnscore import CoreInteraction
from xnsglobal import GlobalNetwork
from xnsglobal import Beacon

class HumanInterface:
    def __init__(self, prompt, nexus, core):
        self.prompt = prompt + "> "
        self.running = True

        # Local transmitter
        self.transmitter = NetworkTransmitter(settings)

        # Local data store
        self.ds = DataStore()

        # Local clock manager
        self.cronos = OmniClock()
        self.cronos.newCounter("_HCI_INIT_")

        # Assign the nexus
        self.nexus_th = nexus

        # Assign the core
        self.core_th = core

        # For interaction with core
        self.core_interact = CoreInteraction(self.core_th)

        if oname == 'nt':
            self.winOs = True
        else:
            self.winOs = False

        # Command map of all available commands
        self.commands = {
            "help": self.help,
            "ls": self.showNodeList,
            "stat": self.showStatus,
            "reach": self.reach,
            "scan": self.initiateScan,
            "clear": self.clearScreen,
            "asd": self.showActiveDisplayStats,
            "devs": self.showAttatchedDevices,
            "core": self.interactWithCore,
            "load": self.loadFileToCore,
            "man": self.manualAdd,
            "rat": self.remoteCommand,
            "global": self.goGlobal
        }

        # The help text that corresponds to each command
        self.commandHelp = {
            "help": "Display a list of commands.",
            "ls": "Display all nodes.",
            "stat": "Display status info.",
            "asd": "Active stat display.",
            "reach": "Attempt to reach a node.",
            "scan": "Scan network for nodes.",
            "clear": "Clear the screen.",
            "devs": "Show attatched devices.",
            "devs.<node>": "Show an external node's devices.",
            "core": "Begin an interaction with the core. <NYD>",
            "load": "Shows the load-file prompt.",
            "man": "Manually add a node by address.",
            "rat": "Remote administration tool. <NYD>",
            "global": "Attatch this node to the global network.<NYD>"
        }

    '''     Take all human input, and begin routing it to the various control methods      '''
    def begin(self):
        header_main("lightgreen")
        while(self.running):
            try:
                self.routeCommand(input(self.prompt))
            except KeyboardInterrupt:
                self.running = False

    '''     Clear the screen despite whatever OS we're on                                  '''
    def clearScreen(self, args):
        if self.winOs:
            osys("cls")
        else:
            osys("clear")
        return

    '''     This is the area the user will go to start throwing commands at the core       '''
    def interactWithCore(self, args):
        self.clearScreen(True)
        self.core_interact.cliBegin()

    '''     Evaluate the command, and route it to the corresponding function               '''
    def routeCommand(self, cmd):
        args = []
        if "." in cmd:
            [args.append(a.replace(" ", "")) for a in cmd.split(".")]
            cmd = args[0]
            del args[0]
        try:
            self.commands[cmd](args)
        except KeyError:
            pass

    '''     Display all commands, and their help text in a somewhat pretty fashion        '''
    def help(self, args):
        header_help("cyan")

        # Spacing stuff
        lk = len(max(self.commandHelp.keys(), key=len))
        lv = len(max(self.commandHelp.values(), key=len))
        width = lk + lv + 2

        # Show an alphabetical list of commands
        for x in sorted(self.commandHelp.items()):
            cout("yellow", x[0], "")
            dots = "." * (width - (len(x[0]) + len(x[1])))
            cout("cyan", dots, "")
            cout("yellow", x[1])
        print("\n\n")

    '''     Show a list of all the nodes that we know about                              '''
    def showNodeList(self, args):
        cout("cyan", "\nKnown Nodes:\n")
        if len(self.nexus_th.nexus_information[1]) == 0:
            cout("yellow", "\tNo known nodes. Try running a network scan.\n")
            return

        # Column width list
        c = [0,0,0,0]
        # Columns
        uid_col = []
        name_col = []
        address_col = []
        location_col = []
        nest_col = []
        # Time logs
        times = []

        # Go through list to buld columns, and get max widths
        for x in self.nexus_th.nexus_information[1]:
            if len(x["uid"]) > c[0]:
                c[0] = len(x["uid"])
            if len(x["name"]) > c[1]:
                c[1] = len(x["name"])
            if len(x["location"]) > c[2]:
                c[2] = len(x["location"])
            if len(x["nest_address"]) > c[3]:
                c[3] = len(x["nest_address"])

            uid_col.append(x["uid"])
            name_col.append(x["name"])
            address_col.append(x["address"])
            location_col.append(x["location"])
            nest_col.append(x["nest_address"])
            times.append(x["activity"])

        # Massage, and display
        for x in range(0,len(uid_col)):
            # Create the spacing
            w = [
                " " * (c[0] - len(uid_col[x]) +2),
                " " * (c[1] - len(name_col[x])+2),
                " " * (c[2] - len(location_col[x])+2),
                " " * (c[3] - len(nest_col[x])+2)
            ]
            # Color code
            minutes = self.cronos.minutesPassedFromString(times[x], "%Y-%m-%d %H:%M:%S.%f")

            if minutes <= 5:
                dc = "lightgreen"
            elif minutes <= 10:
                dc = "yellow"
            else:
                dc ="fail"

            # Print each row
            cout("underlined", uid_col[x])
            cout(dc, 
            (name_col[x] + w[1] + address_col[x] + w[2] + location_col[x] + w[3] + "[" + nest_col[x] + "]"),
             "")

            if nest_col[x] == settings["nest_address"]:
                cout("blue", "[+]\n")
            else:
                cout("red", "[x]\n")
        print("\n")

    '''     Show the status of the current node, and known nodes                         '''
    def showStatus(self, args):
        # Show the status infromation of the node
        cout("lightgreen", settings["UID"], " ")
        print(":", end=" ")
        cout("cyan", settings["name"], " ")
        print(":", end=" ")
        cout("yellow", settings["location"], "\n\n")
        cout("yellow", "Number of known nodes : ", " ")
        cout("lightgreen", str(len(self.nexus_th.nexus_information[1])), "\n\n")
        cout("blue", "Public address:", "")
        cout("yellow", settings["nest_address"], "\n\n")


    '''     Bring up the active status display                                           '''
    def showActiveDisplayStats(self, args):
        tock = True
        self.sass = True
        self.cronos.newCounter("_HCI_ADS_")
        last = self.nexus_th.nexus_information[1].copy()
        while(self.sass):
            
            # Check if there was an update in the nexus
            if last != self.nexus_th.nexus_information[1]:
                last = self.nexus_th.nexus_information[1].copy()
                tock = True

            # Check if its been 5 minutes since last screen update
            if tock == False:
                if self.cronos.minutesPassed("_HCI_ADS_") >= 1:
                    self.cronos.resetClock("_HCI_ADS_")
                    tock = True
                
            # See we should update the screen
            if tock == True:
                lastdt = datetime.now()
                self.clearScreen([])
                tock = False
                print("\n")
                self.showStatus([])
                self.showNodeList([])
                cout("red", "\n\n\t[Press return to close]\n")

            # If the user presses enter, exit the screen
            if stdin in select([stdin], [], [], 0)[0]:
                self.sass = False
                self.cronos.removeClock("_HCI_ADS_")
                return

            # So we don't eat the cpu
            sleep(1)
        
    '''     Attempt to reach out to a known node                                         '''
    def reach(self, args):
        if len(args) != 1:
            cout("fail", "Usage : reach . <node id | node name | node location>")
            return

        cout("red", "\nReach assumes that you are searching by a unique value.")
        cout("red", "If there are multiple nodes with the same search param,")
        cout("red", "the first one found will be attempted.\n")

        list_to_reach = []
        # Find out who they are trying to reach
        for entry in self.nexus_th.nexus_information[1]:
            for x in args:
                if x == entry["location"] or x == entry["name"] or x == entry["uid"]:
                    list_to_reach.append((entry["address"], x))

        # Attempt to reach each node
        for ip in list_to_reach:
            try:
                resp = self.transmitter.outgoingQuery(ip[0], settings["port"], "Q$" + probeInfo(True))
            except:
                resp = settings["error"]
                cout("fail", " > Unable to reach", " ")
                cout("cyan", ip[1], "")
                print("@", end="")
                cout("yellow", ip[0])

            if resp != settings["error"]:
                try:
                    self.ds.addNodeActivity(jloads(resp))
                    cout("lightgreen", "> Reached, and logged :", " ")
                    cout("cyan", ip[1], "")
                    print("@", end="")
                    cout("yellow", ip[0])
                except:
                    self.ds.addLogEntry(["HUMAN_INTERFACE", "Unable to addNodeActivity in reach"])
                    cout("fail", "> Unable to make sense of response from :", " ")
                    cout("cyan", ip[1], "")
                    print("@", end="")
                    cout("yellow", ip[0])
        print("\n")

    '''     Initiate a network scan looking for new, and updating current nodes          '''
    def initiateScan(self, args):
        
        if self.winOs:
            cout("red", "<Scan not yet supported on windows>")
            return

        if len(args) == 0:
            startScan()
        else:
            cout("fail", "Arguments not yet implemented for scanning.")
        
        # Update interaction's information
        self.core_interact.known_nodes = self.ds.getNodeActivity()
        return

    '''     Show all of the devices attatched to this node, or a node passed in          '''
    def showAttatchedDevices(self, args):
        
        # If no args, show devices attached to this node
        if len(args) == 0:
            if len(device_settings) == 0:
                a = '''

    There are no devices attatched to this node. 
    Please see ./docs/ConfiguringXNS.

                '''
                cout("fail", a)
                return
            
            for device in device_settings:
                cout("cyan", "\n" + device["id"])
                cout("yellow", "\tSlots:", "\t   ")
                cout("lightgreen", str(len(device["slots"])))
                cout("yellow", "\tRoutines:", " ")
                cout("lightgreen", str(len(device["routines"])))
                print("")
        else:
            # If an arg came in, find it in the nexus and do the same as above
            for x in self.nexus_th.nexus_information[1]:
                if x["name"] == args[0] or x["uid"] == args[0] or x["location"] == args[0]:
                    
                    # For some reason empty length is 2
                    if len(x["devices"]) == 2 or len(x["devices"]) == 0:
                        a = '''

    There are no devices attatched {a} 
    Please see ./docs/ConfiguringXNS.txt

                '''.format(a=args[0])
                        cout("fail", a)
                        return

                    # Make the string jloads-able and load it to show information
                    cout("cyan", ("\n" + args[0] + " devices: "))
                    xdevs = jloads(x["devices"].replace("'", "\""))
                    for device in xdevs:
                        cout("cyan", "\n" + device["id"])
                        cout("yellow", "\tSlots:", "\t   ")
                        cout("lightgreen", str(len(device["slots"])))
                        cout("yellow", "\tRoutines:", " ")
                        cout("lightgreen", str(len(device["routines"])))
                        print("")
                    return

        print("")
        return

    '''     Send a file to be rotated by the core       '''
    def loadFileToCore(self, args):
        
        # Clear the screen, and inform the user of what will happen
        self.clearScreen(args)
        cout("cyan", "File loader")
        cout("yellow", "All input files will be loaded into the core.\n Once loaded, rotation will be triggered.")
        cout("fail", "Warning: Entering the core manually post file-load\n will clear the core for cli interaction.\n")

        # Get all of the files from the user
        files = []
        cout("yellow", "Please enter all files to load, seperated by commas.\n\n")
        x = input(ctxt("red", " load > "))

        # Ensure they can be seperated by ',' then send all files to the core
        x += ","

        files = x.split(",")[:-1]

        if len(files) >= 1:
                self.core_interact.loadFromFiles(files)
        cout("cyan", "... All files loaded into the core ...")

    '''     Manually add a node by address              '''
    def manualAdd(self, args):
        self.clearScreen(args)

        if len(args) == 0:
            cout("fail", "Fail.\n Usage : man ", "")
            cout("yellow", "<address>")
            return
            
        ip = '.'.join(args)
        if ip == '.':
            cout("fail", "Fail.\n Usage : man ", "")
            cout("yellow", "<address>")
            return

        cout("cyan", "Attempting to manually add node : ", "")
        cout("yellow", ip)
        manScan(ip)

    def remoteCommand(self, args):
        
        # Get node name, and be sure its the correct node
        confirmed = False
        self.clearScreen(args)
        while confirmed == False:
            x = input("Receiving node (name | ls) : ")

            if x == "ls":
                self.clearScreen(args)
                self.showNodeList(args)
                continue

            y = input("Is [" + x + "] correct? (y/n/ls) : ")
            if y == "y":
                confirmed = True
            elif y == "ls":
                self.clearScreen(args)
                self.showNodeList(args)
            else:
                self.clearScreen(args)
                print("Use 'ls' to show nodes.")

        # Get the command        
        self.clearScreen(args)
        cout("red", "RAT.", "")
        cout("cyan", x)
        print("")

        scmd = None
        confirmed = False
        while False == confirmed:
            scmd = input(ctxt("red", "Command to execute : "))
            res = input(ctxt("lightgreen", "Please confirm (y/n) : "))
            if res != "y":
                self.clearScreen(args)
                continue
            else:
                confirmed = True

        cout("red", ".... Requesting command execution ....")    
        
        found = False
        request = "@$" + scmd
        for entry in self.nexus_th.nexus_information[1]:
            if x == entry["location"] or x == entry["name"] or x == entry["uid"]:
                found = True
                resp = None
                try:
                    resp = self.transmitter.outgoingQuery(entry["address"], settings["port"], request)
                except:
                    print("Unable to submit command to node [" + x + "]")
                
                if resp is None:
                    continue

                cout("lightgreen", "\n\n[RESPONSE]\n")
                print(resp)

        if False == found:
            print("No node represented by [", x , "]")
            return


    def goGlobal(self, args):
        
        # Check to see if this is to beacome a true beacon
        if len(args) > 0:
            if args[0] == "relay":
                self.__becomeGlobalRelay()
                return
        
        # If not a true beacon, connect to a beacon
        self.clearScreen(args)

        cout("red", "\nGlobal Network Interface...\n\n")

        beacons = []
        for entry in self.nexus_th.nexus_information[1]:
            if entry["nest_address"] != settings["nest_address"]:
                beacons.append(entry)
            elif entry["name"] == "__FORCED_BEACON__":
                
                cout("lightgreen", "\n\tAdding a forced beacon [Human Interface]\n")
                beacons.append(entry)

        index = 0
        if len(beacons) != 0:
            cout("lightgreen", "Beacons available: ")
            for x in beacons:
                cout("yellow", str(index), "  ")
                cout("cyan",  x["uid"] + "\t" + x["address"] + "\t" + x["name"] )
                index +=1
            index -=1
        else:
            cout("underlined", "A beacon must be known prior to joining (see command : man).\n\n")
            return

        print("\n\n")

        confirm = False
        while False == confirm:
            beacon = input("Enter beacon number: ")
            try:
                beacon = int(beacon)
            except ValueError:
                cout("red", "NaN")
                continue

            if beacon > index or beacon < index:
                cout("red", "Beacon index out of range...")
                continue
            confirm = True

        # Change to global network mode
        GlobalNetwork(beacons[beacon], self.nexus_th)


    def __becomeGlobalRelay(self):

        print("NYD - Become a global beacon whose sole purpose is to serve.")
        
        self.clearScreen([])
        cout("cyan", (settings["UID"] + ":" + settings["nest_address"]) )
        cout("yellow", "...Creating beacon...")
        b = Beacon(settings["address"])
        b.start()
        cout("lightgreen", "Beacon launch success...")
        cout("yellow", "Beginning active status. Press return to shutdown...\n\n")

        sab = True
        tock = True
        self.cronos.newCounter("_BEACON_ADS_")
        while sab:
            
            if tock:
                tock = False
                print("UPDATE THE SCREEN WITH THE INFO... NYD ~Line 550 HCI ")
                
            if self.cronos.minutesPassed("_BEACON_ADS_") >= 1:
                self.clearScreen([])
                tock = True

            # If the user presses enter, exit the screen
            if stdin in select([stdin], [], [], 0)[0]:
                sab = False
                self.cronos.removeClock("_BEACON_ADS_")
                b.signal = False
                return
        return