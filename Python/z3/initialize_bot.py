LABEL = "TEST_BOT"
PORT = 9001
INITIAL_PILLAR = "192.168.1.5"
from xnmlib import XNMNode
XNMNode(LABEL, create_node=True, port=PORT)