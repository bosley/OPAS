from xnmlib import XNMPillar
from xnmlib import XNMNode
from node_interface import node_interface

import argparse
from os import remove
from sys import exit

parser = argparse.ArgumentParser()
parser.add_argument('--setup', dest='setup_type',
                    help='Setup simnet node or pillar. [n|p]')
parser.add_argument('--run', dest='do_run',
                    help='Run a simnet node or pillar. [n|p]')   
parser.add_argument('--clean', action="store_true", dest='clean_db', default=False,
                    help='Clean the databases')    
parser.add_argument('--join', action="store_true", dest='join_pillar', default=False,
                    help='Join local pillar to remote network')   
parser.add_argument('--version', action='version', version='%(prog)s 0.1')
results = parser.parse_args()

'''
    Inject pillar information into the pillar db
'''
def inject(id, i, p):
    print("...injecting information into DB...")
    XNMPillar(".",remote_pillar=[id, i, p])

def man_join():
    frem = input("Remote pillar ID: ")
    fip = input("Remote pillar ip: ")
    fport = input("Remote pillar port: ")
    inject(frem, fip, fport)

'''
        Setup a new node in the DB
'''
def setup_node():
    name = input("Name of the node: ")
    port = input("Node port: ")
    print("...Creating node...")
    XNMNode(name, create_node=True, port=port)

'''
        Setup a new pillar in the DB
'''
def setup_pillar():
    ip = input("Pillar IP: ")
    port = input("Pillar PORT: ")
    net = input("Pillar network name: ")
    print("Pillar will stand alone until joined with remote pillar ID using '--join'")
    n=XNMPillar(net,create_mode=True,ip_addr=ip, port=port)

'''
    Clean settings and databases
'''
def clean_db():
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
        
'''
    Start a simple node instance
'''
def run_node():
    local_node = None

    try:
        with open("local_node.config", "r") as r:
            local_node = r.readline()
    except:
        print("Unable to find node config. Is the node --setup yet?")
        exit(1)

    if local_node is None:
        print("Problem wil local node config. Try --clean, and redo --setup")
        exit(1)

    if len(local_node) == 0:
        print("Problem wil local node config. Try --clean, and redo --setup")
        exit(1)
        
    print("Starting node: ", local_node)
    node_interface(local_node)

'''
    Run a pillar
'''
def run_pillar():
    local_pillar = None

    try:
        with open("local_pillar.config", "r") as r:
            local_pillar = r.readline()
    except:
        print("Unable to find pillar config. Is the pillar --setup yet?")
        exit(1)

    if local_pillar is None:
        print("Problem wil local pillar config. Try --clean, and redo --setup")
        exit(1)

    if len(local_pillar) == 0:
        print("Problem wil local pillar config. Try --clean, and redo --setup")
        exit(1)
        
    print("Starting pillar: ", local_pillar)
    XNMPillar(local_pillar)

if __name__ == '__main__':

    if results.setup_type is not None:
        if results.setup_type.lower() == "n":
            setup_node()
        elif results.setup_type.lower() == "p":
            setup_pillar()
        else:
            print("Error: Setup must be of node [n] or pillar [p] type!")
            exit(1)
        exit(0)

    if results.clean_db:
        clean_db()
        
    if results.do_run is not None:
        if results.do_run.lower() == "n":
            run_node()
        elif results.do_run.lower() == "p":
            run_pillar()
        else:
            print("Error: Run must be of node [n] or pillar [p] type!")
            exit(1)
        exit(0)

    if results.join_pillar:
        man_join()