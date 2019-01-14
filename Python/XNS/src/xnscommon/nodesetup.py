from sys import exit
from json import load as jl
from json import dumps
import uuid
import netifaces as ni
import requests

'''
        Handles walking the user through configuring the node settings

        To add : 

                Full reconfigure (All settings new)

                Dump keys (Show all keys)

'''
class NodeSetup:

    def __init__(self, config):
        self.config = config

    # Write the dic out to file
    def writeJsonFile(self):
        with open("xnscommon/node_settings.json", 'w+') as o:
            print("Writing . . . ")
            o.write(dumps(self.config, indent=4))
            print("Complete . . . ")

    def sourceDevice(self):
        idev = input("Enter network device name : ")
        try:
            ip = ni.ifaddresses(idev)[ni.AF_INET][0]['addr']
        except:
            print("Error: Invalid device")
            return
        print("Node ip : ", ip)
        name = input("Enter a name for the node : ")
        location = input("Enter the node's location : ")
        uid = str(uuid.uuid1().hex)
        self.config["UID"] = uid
        self.config["location"] = location
        self.config["adapter"] = idev
        self.config["address"] = ip
        self.config["name"] = name
        self.config["pending"] = "NYD"

        r = requests.get(r'http://jsonip.com')
        self.config["nest_address"] = r.json()['ip']
        self.writeJsonFile()


