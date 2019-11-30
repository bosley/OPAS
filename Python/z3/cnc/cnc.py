
# Requires 
#   netifaces
#   pycrypto
#   edcsa

import json
import os.path
from os import remove
import argparse
from hashlib import sha512
from getpass import getpass
from sys import exit
from time import sleep
from random import randint
from xnmlib import XNMNode

parser = argparse.ArgumentParser()
parser.add_argument('--version', action='version', version='%(prog)s 0.1')
parser.add_argument('--init', action="store_true", dest='do_init', default=False,
                    help='Initialize CNC')      
parser.add_argument('--clean', action="store_true", dest='clean_db', default=False,
                    help='Clean the databases and configs')    
results = parser.parse_args()

'''
    Clean DB
'''
if results.clean_db:
    try:
        remove("xnmlib/data/pillar_data.db")
    except:
        print("Error removing Pillar DB - Perhaps it didn't exist")

    try:
        remove("xnmlib/data/node_data.db")
    except:
        print("Error removing node DB - Perhaps it didn't exist")
        
    try:
        remove("local_pillar.config")
    except:
        print("Error removing 'local_pillar.config' - Perhaps it didn't exist")
        
    try:
        remove("local_node.config")
    except:
        print("Error removing 'local_node.config' - Perhaps it didn't exist")
        
    try:
        remove("config.cnc")
    except:
        print("Error removing 'local_node.config' - Perhaps it didn't exist")
    exit(0)
        
'''
        Initialize the CNC
'''
if results.do_init:
    print("\n\nDetecting potential ips..")
    from netifaces import interfaces, ifaddresses, AF_INET
    j = 0
    ifa = {}
    for ifaceName in interfaces():
        addresses = [i['addr'] for i in ifaddresses(ifaceName).setdefault(AF_INET, [{'addr':'L'}] )]
        if addresses[0] != "L":
            ifa[str(j)] = addresses[0]
            j += 1
    print("\n\nSelect an IP to bind CNC")
    print("To manually supply IP, use 'm'")
    for k, v in ifa.items():
        print(k, "\t", v)
    try:
        ip = None
        while ip is None:
            inp = input(">")
            if inp == "m":
                ip = input("Input IP: ")
            else:
                try:
                    ip = ifa[inp]
                except KeyError:
                    print("That wasn't an option.. exiting")
                    exit(1)
    except KeyboardInterrupt:
        print("...Exiting")
    print("\nUsing IP: ", ip)
    label = input("Label for CNC: ")
    port = input("Port for CNC node: ")
    print("Network password for CNC: ")
    pwd = getpass().encode('utf-8')
    pwd = sha512(pwd).hexdigest()
    print("Re-Enter: ")
    pwd1 = getpass().encode('utf-8')
    pwd1 = sha512(pwd1).hexdigest()
    if pwd != pwd1:
        print("FOOL! Passwords don't match ... Exiting")
        exit(1)
    print("Writing settings...")
    if os.path.isfile("config.cnc"):
        remove("config.cnc")
    sett = {}
    sett["ip"] = ip
    sett["port"] = port
    sett["label"] = label
    sett["password"] = pwd1
    with open("config.cnc", 'w') as o:
        json.dump(sett, o)
    print("Complete.")
    print("Setting up node..")
    XNMNode(label, create_node=True, port=port)
    exit(0)

''' 
    Ensure that CNC is setup 
'''
if not os.path.isfile("config.cnc") or not os.path.isfile("local_node.config"):
    print("CNC not yet setup. Run --init")
    exit(0)
    

'''
    Open and setup the XNM Node
'''
with open("local_node.config", "r") as r:
    lnode = r.readline()

local_node = XNMNode(lnode)
local_node.supressText = False
local_node.start()

message_box = []

total_links = 0

'''
    Load config
'''
config = None
with open("config.cnc", 'r') as o:
    config = json.load(o)

''' 
    Display information things
'''
def dsp_info():
    nods = local_node.external_pull_known_nodes()
    pills = local_node.external_pull_known_pillars()
    print(config["label"], "@", config["ip"])
    print(len(pills), " pillars known")
    print(len(nods), " nodes known")

def dsp_nodes():
    nods = local_node.external_pull_known_nodes()
    print("Node knows: ", len(nods), " nodes: ")
    for nod in nods:
        print("\t", nod["uid"])

def dsp_pillars():
    pills = local_node.external_pull_known_pillars()
    print("Node knows: ", len(pills), " pillars: ")
    for p in pills:
        print("\t", p["id"])

def dsp_messages():
    for m in message_box:
        print(m)
    
'''
    Refresh things
'''
def ref_peers():
    local_node.update_available_pillars()
    local_node.update_available_nodes()
    print("Nodes and pillars refreshed")

def ref_messages():
    message_box.clear()
    while not local_node.received_messages.empty():
        message_box.append(local_node.received_messages.get())

    print("There are: ", len(message_box), " messages.")


'''
    Connect to a thing
'''
def pillar_connect():
    try:
        pid = input("ID of pillar (Assumed correct - not checked) : ")
        pip = input("IP Address of pillar (Assumed correct - not checked) : ")
        pport = input("Port of pillar (Assumed correct - not checked) : ")
        local_node.force_register(pid, pip, pport)
    except KeyboardInterrupt:
        print("")
        return

'''
    Send stuff to things
'''
def broadcast_data():
    a = input("network: ")
    b = input("data: ")
    local_node.broadcast_message(a, b)

def send_data():
    a = input("receiver: ")
    b = input("data: ")
    local_node.send_message(a, b)

'''
    Have a whole conversation with a node
'''
def link_node():
    nods = local_node.external_pull_known_nodes()
    print("Node knows: ", len(nods), " nodes: ")
    ncon = {}
    j = 0
    for nod in nods:
        ncon[str(j)] = nod["uid"]
        j += 1

    if j == 0:
        print("There are no nodes currently known.")
        return

    selection = None
    print("\nSelect from available nodes: \n")
    for k,v in ncon.items():
        print(k, "\t", v)
    
    selection = input("\n\nnode # > ")
    try:
        selection = ncon[selection]
    except KeyError:
        print("Invalid selection")
        return

    print("Linking to node: ", selection)

    remote_node = None
    for nod in nods:
        if selection == nod["uid"]:
            remote_node = nod

    if remote_node is None:
        print("Something went wrong selecting the node...")
        return

    print("Starting dialog")
    global total_links
    try:
        local_node.send_message(remote_node["uid"], "INIT_LINK_" + str(total_links), previous_hash=sha512(
            str(randint(0,2**256)).encode('utf-8')
            ).hexdigest())

        total_links += 1

        print("Attempting to contact '",remote_node["uid"],"'")
        print("If link hash isn't unique for the pillar, the message will be filtered out.\n (low prob)\n\n")
        print("-"*10)
        msgs = []
        while True:
            waiting = True
            msgs.clear()
            while waiting:
                sleep(1)
                if not local_node.received_messages.empty():
                    msgs.append(local_node.received_messages.get())
                    waiting = False

            for m in msgs:
                if m["fro"] == remote_node["uid"]:
                    print("response> ", m["message"])
            
            reply = input("message> ")
            local_node.send_message(remote_node["uid"], reply, previous_hash=m["hash"])

    except KeyboardInterrupt:

        print("- Killing Link - ")
        local_node.send_message(remote_node["uid"], "KILL ", previous_hash=sha512(
        str(randint(0,2**256)).encode('utf-8')
        ).hexdigest())
        try:
            print("Waiting for kill confirmation")
            waiting = True
            msgs.clear()
            while waiting:
                sleep(1)
                if not local_node.received_messages.empty():
                    msgs.append(local_node.received_messages.get())
                    waiting = False
        except KeyboardInterrupt:
            print("Force killing connection.\n")
            return
        return

commands = {
    "info": dsp_info,
    "nodes": dsp_nodes,
    "pillars": dsp_pillars,
    "refresh": ref_peers,
    "pconnect": pillar_connect,
    "broadcast": broadcast_data,
    "refmsg": ref_messages,
    "dspmsg": dsp_messages,
    "send": send_data,
    "link": link_node
}

'''
    Command and control method
'''
def cnc():
    try:
        while True:
            uin = input((config["label"] + "@" + config["ip"] + "> "))
            func = None

            if len(uin) > 0:
                try:
                    func = commands[uin]
                except KeyError:
                    if uin == "help":
                        print('''

            info  ........ show cnc info
            nodes ........ show known nodes
            pillars ...... show known pillars
            refresh ...... update nodes and pillars
            pconnect ..... manually connect to a pillar
            dspmsg ....... display all messages
            refmsg ....... refresh all messages received
            -------------------------------------------

            broadcast .... Send a message to all of a group
            send ......... direct send to specific node - resp in msgs
            link ......... have a full conversation with a node
                        
                        ''')
                    else:
                        print("Unknown command -> Try using 'help'")

                if func is not None:
                    func()
    except KeyboardInterrupt:
        print("\nExiting")
        exit(0)
cnc()