import time
import threading

from xnsdb import DataStore
from xnscommon import settings
from xnscommon import device_settings
from xnscommon import OmniClock
from xnscomm import NetworkTransmitter
from xnscomm import probeInfo
from datetime import datetime
from json import loads as jloads
from json import dumps as jd

'''

    Node nexus continuously monitors the state of the nodes
    It will attempt to keep-up connections, contacting known nodes
    frequently enough to keep them active. 

'''

class NodeNexus(threading.Thread):
    
    def __init__(self):
        threading.Thread.__init__(self)
        self.signal = True

        # To handle unique ids on requests
        self.id_counter = 0

        # Variable for access by caller to tell if updates have happened
        self.nexus_information = [False, []]

        # Local data store class
        self.db = DataStore()

        # Local transmitter
        self.transmitter = NetworkTransmitter(settings)
        
        # Local clock
        self.cronos = OmniClock()

        # Self info
        self.own_info = ('{"UID": "' + settings["UID"] + 
			'", "name": "' + settings["name"] + 
			'", "location": "' + settings["location"] + 
			'", "nest_address": "' + settings["nest_address"] + 
			'", "address": "' + settings["address"] + 
			'", "devices": ' + jd(device_settings) + '}')
        
        # Ensure the node knows itsself
        self.db.addNodeActivity(jloads(self.own_info))

        # Nodes on the same nest
        self.node_kin = []

        # Nodes that haven't responded for a while
        self.node_prune = []

    '''     The main thread handler                                  '''
    def run(self):
        while self.signal:
            # Reach out to any idle nodes > 15 minutes of inactivity
            self.ensureNodeActivity()

            # Update the the nexus info with known nodes
            self.updateNexusInformation()

            time.sleep(1)
            
    '''     Get the node information, and signal any change          '''
    def updateNexusInformation(self):
        temp = self.nexus_information[1].copy()
        self.nexus_information[1] = self.db.getNodeActivity()

        # Indicate if an update has happened to the list
        self.nexus_information[0] = (temp != self.nexus_information[1])

        # Indicate which nodes are on the same network as this one
        self.node_kin.clear()
        for n in self.nexus_information[1]:
            if n["nest_address"] == settings["nest_address"]:
                self.node_kin.append(n)

    '''     Reach out to nodes that haven't contacted us in a while   '''
    def ensureNodeActivity(self):

        # If we don't know of any nodes, just leave
        if len(self.nexus_information[1]) == 0:
            return

        # Go through each known node, and display their information
        for entry in self.nexus_information[1]:
            # Get the difference between now, and the last activity time
            #difference = datetime.now() - datetime.strptime(entry["activity"], "%Y-%m-%d %H:%M:%S.%f")
            #minutes = divmod(difference.days * 86400 + difference.seconds, 60)[0]
            minutes = self.cronos.minutesPassedFromString(entry["activity"], "%Y-%m-%d %H:%M:%S.%f")

            # If the node hasn't updated within 15 minutes, probe them
            if minutes > 15:
                try:
                    resp = self.transmitter.outgoingQuery(entry["address"], settings["port"], "Q$" + probeInfo(True))
                except:
                    resp = settings["error"]

                # If we could contact them, update with their information
                if resp != settings["error"] and resp != None:
                    self.db.addNodeActivity(jloads(resp))
                else:
                    self.db.removeNode(entry["uid"])
