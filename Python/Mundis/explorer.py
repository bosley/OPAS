import getpass
import connector

from code import InteractiveConsole
from imp import new_module
 
class Console(InteractiveConsole):
    def __init__(self, names=None):
        names = names or {}
        names['console'] = self
        InteractiveConsole.__init__(self, names)
        self.superspace = new_module('superspace')
    def enter(self, source):
        try:
            source = self.preprocess(source)
            self.runcode(source)
        except SyntaxError:
            print("Python Syntax Error")
    @staticmethod
    def preprocess(source):
        return source
 
mundisspace = new_module('mundisspace')

try:
    server = input("Server: ")
    port = input("Port: ")
    db = input("Database: ")
    stoken = input("Server Auth Token: ")
    uname = input("Username: ")
    passw = getpass.getpass("Password: ")
    con = connector.munconnect(
        server, port, stoken, uname, passw, db
    )
    enable_logging = input("Enable logging? [Y/n]")
    if enable_logging.lower() == "y":
        con.log_response = True
    else:
        con.log_response = False
    con.connect()
    mundisspace.con = con
    mundisspace.help = '''
----------------------[ Mundis Explorer ]----------------------

Use 'help' to bring up this message, to view syntax use 'syntax'

Each line entered will be stored into a buffer until
'commit' is detected. Once 'commit' is entered, all commands
in buffer will be executed sequentially. 

Note:
    con is the only variable given to the user, and setting 
    variables is not enabled. Stick to Mundis syntax ONLY

Warning: 
    All commands entered are live - changes will be reflected on
    the database 

Use ^C to exit

---------------------------------------------------------------
'''

    mundisspace.syntax = '''
Mundis queries are all dictionaries. 
Each query type takes different keys to evaluate what you're 
trying to do. The following are methods of 'con.'

    get     
        "type"      - Get all keys mapping to specific data type
        "key"       - Get a specific key's value
        <empty>     - Get all keys in the database

        Examples:
            con.get({"key": "db_item"})
            con.get({"type": int})
            con.get({})

    getli
        - Get an index from a list-type
            "key", "index"
    
        Examples:
            con.getli({"key": "my_list", "index": 6})
            
    getdv
        - Get a value from a dict-type
            "key", "value"

    set
        - Set "value" the key "key" 
            "key", "value"

    setli
        - Set an index of a list-type
            "key", "value", "index"

        - Same as setli, but will accept overflow values 'append' and 'expand'
            append will add the data if the end of the list if index is out of bounds
            expand will expand the list to length required for index to be within bounds
                expanded indices will be filled with type 'None' 

            "key", "value", "index", "overflow"

    setdv
        - Set a value within dict-type entry. "value" must be of type 'tuple' where
            the 0th index is the name of the key within the dictionary, and the 1st 
            index is the value within the dictionary 

            "key", "value"

    For example uses, please check the Mundis documentation

''' 
    console = Console({'mundisspace': mundisspace})

    print(mundisspace.help)

    buffer = []
    while True:
        uin = input(">>> ")
        if len(uin) == 0:
            buffer.append("\n")
        elif uin == "commit":
            for line in buffer:
                console.enter("print(mundisspace."+line+")")
            buffer[:] = []
            print()
        else:
            buffer.append(uin)
except KeyboardInterrupt:
    exit(0)