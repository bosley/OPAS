from os import name as osname
from os import path as ospath
import dynoskeys 

PATHING = "/"

'''
    File structure information
'''
if osname == "posix":
    PROG_ROOT = ospath.expanduser('~') + "/.dynos_node/"
elif osname == "nt":
    PATHING = "\\"
    PROG_ROOT = ospath.expanduser('~') + "\\dynos_node\\"
else:
    PROG_ROOT = "/tmp/dynos_node/"

DB_ROOT = PROG_ROOT + "data_store" + PATHING
DB_SLOTS = DB_ROOT + "slots" + PATHING
DB_ROUTINES = DB_ROOT + "routines" + PATHING
DB_CONFIG = DB_ROOT + "config.db"
DB_LOG = DB_ROOT + "log.db"
DB_KEYS = DB_ROOT + "dynoskeys.db"

'''
    Network information
'''
NET_NODE_PORT = 9000

'''
    Returns types
'''
VALID_RETURNS = [list, str, dict, int, float, bytes, bool]

'''
    Definition headers
'''
HEADERS_LOCAL_DEFINITION = ["address", "port", "description", "location"]

'''
    Global key manager
'''
KEY_MANAGER = dynoskeys.Dynoskeys(key_db=DB_KEYS)
