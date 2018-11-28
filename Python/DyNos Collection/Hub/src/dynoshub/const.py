import dynoskeys
from os import name as osname
from os import path as ospath

PATHING = "/"

'''
    File structure information
'''
if osname == "posix":
    PROG_ROOT = ospath.expanduser('~') + "/.dynos_core/"
elif osname == "nt":
    PATHING = "\\"
    PROG_ROOT = ospath.expanduser('~') + "\\dynos_core\\"
else:
    PROG_ROOT = "/tmp/dynos_core/"

DB_ROOT = PROG_ROOT + "data_store" + PATHING
DB_CONFIG = DB_ROOT + "config.db"
DB_DEVICE = DB_ROOT + "devices.db"
DB_LOGGING = DB_ROOT + "logging.db"
DB_KEYS = DB_ROOT + "dynoskeys.db"

'''
    Network information
'''
NET_NODE_PORT = 9000
KEYS_NETWORK_PACKET = ["to", "from", "publicKey", "signature", "route", "data"]

'''
    Registration keys required
'''
KEYS_REGISTRATION = ["caller", "publicKey", "slots", "routines"]

'''
    Global key manager
'''
KEY_MANAGER = dynoskeys.Dynoskeys(key_db=DB_KEYS)