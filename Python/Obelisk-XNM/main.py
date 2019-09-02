from xnmlib import XNMPillar
from xnmlib import XNMNode

'''
        ABOUT: 

        n - Run a node
            type 'help' for list of commands
            'madd' to manually add a pillar

        p - Run a pillar
        
        c - Create a node or a pillar
            Creates and adds local node or pillar into their
            respective databases, along-with their RSA keys

        i - Inject a pillar into the pillar database
            Because pillars aren't threaded and don't have 
            a human-computer interface, the injection mode
            must be set with relevant data while node is offline.
            RSA keys aren't required here, because the local pillar
            will register with the injected node once it can contact
'''

'''
    Because nodes are meant to be an instrument in an semi/automated system, 
    this node interaction is setup to demonstrate / test the operations
    that the node can perform

'''
def node():

    x = input("Which node ID to load? : ")

    n = XNMNode(x)
    n.supressText = False
    n.start()

    try:
        while True:
            x = input("node > ")

            if x == "help":
                print('''

                help            - Show help
                send            - Send a message
                broadcast       - Broadcast a message
                nodes           - Show list of available nodes
                pillars         - Show all pillars
                fupdate         - Force update of nodes and pillars
                madd            - Manually add a pillar

                check           - Check for messages
                ''')
            elif x == "send":
                a = input("receiver: ")
                b = input("data: ")
                n.send_message(a, b)
            elif x == "broadcast":
                
                '''
                    - messages sent to non-existent networks are dropped. 
                        no indication of event from outside perspective
                '''

                a = input("network: ")
                b = input("data: ")
                n.broadcast_message(a, b)
            elif x == "nodes":
                nods = n.external_pull_known_nodes()
                print("Node knows: ", len(nods), " nodes: ")
                for nod in nods:
                    print("\t", nod["uid"])
            elif x == "pillars":
                pills = n.external_pull_known_pillars()
                print("Node knows: ", len(pills), " pillars: ")
                for p in pills:
                    print("\t", p["id"])
            elif x == "madd":
                # Only needs to be done once
                pid = input("ID of pillar (Assumed correct - not checked) : ")
                pip = input("IP Address of pillar (Assumed correct - not checked) : ")
                pport = input("Port of pillar (Assumed correct - not checked) : ")
                n.force_register(pid, pip, pport)
            elif x == "check":
                print("MESSAGES: ", n.received_messages.qsize())
            elif x == "messages":
                while not n.received_messages.empty():
                    print(n.received_messages.get())
            elif x == "":
                pass
            else:
                print("Unknown command")
                
    except KeyboardInterrupt:
        n.kill()

'''
    Start a pillar - no HCI
'''
def pillar():
    x = input("Which pillar ID to load? : ")
    x=XNMPillar(x)

'''
    Launch pillar injection 
'''
def inject():
    id = input("ID: ")
    ip = input("IP: ")
    port = input("PORT: ")
    XNMPillar(".",remote_pillar=[id, ip, port])

'''
    Generate new nodes or pillars based on some input
'''
def create():
    x = input("node or pillar? [n, p] : ")
    if x != "n" and x != "p":
        print("n, or p only...")
        return

    if x == "n":
        x = input("Node name: ")
        y = input("Node port: ")
        n=XNMNode(x, create_node=True, port=y)

    if x == "p":
        x = input("Network: ")
        y = input("Pillar port: ")
        z = input("Pillar addr [dh,jb, *] : ")
        n=XNMPillar(x,create_mode=True,ip_addr=z, port=y)

'''
    Direct the user to thier interactions
'''
x = input("node, pillar, create_mode, inject_mode ? [n,p,c,i]:")

if x == "n":
    node()
elif x == "p":
    pillar()
elif x == "c":
    create()  
elif x == "i":
    inject()    
else:
    print("Invalid - use n,p,c,i")


