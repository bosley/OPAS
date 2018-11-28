

from xnmlib import XNMNode


def show_help():
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



def node_interface(lnode):

    n = XNMNode(lnode)
    n.supressText = False
    n.start()

    prompt = lnode[:10] + " $ "

    try:
        while True:

            x = input(prompt)

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
                messages        - Dump all messages 

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
            elif x == "fupdate":
                n.update_available_pillars()
                n.update_available_nodes()
                print("Updated pillars and nodes")
            elif x == "":
                pass
            else:
                print("Unknown command")

    except KeyboardInterrupt:
        n.kill()

