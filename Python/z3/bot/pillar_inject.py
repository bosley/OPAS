
PILLAR_ID = "server-000-0x7d260da4ef663ca65d8e232b3576caa8"
PILLAR_IP = "104.236.208.93"
PILLAR_PORT = 9001

from xnmlib import XNMNode

with open("local_node.config", "r") as r:
    lnode = r.readline()

n = XNMNode(lnode)
n.supressText = False
n.start()
n.force_register(PILLAR_ID, PILLAR_IP, PILLAR_PORT)
n.kill()
