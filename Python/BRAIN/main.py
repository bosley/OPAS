
from sys import exit
from random import randint
from hashlib import sha256
from datetime import datetime
from json import load, dump, dumps

# Custom screen coloring
from display import cout, ctxt

# System interaction 
from sysfunc import *

# Global scoped lambdas and variables
from core import *

from comms import Comms
from senses import Senses
from network import Network
from journal import Journal

ModuleRouter = {
    "jrnl" : Journal()
    }

DaemonRouter = {
    "senses": Senses(),
    "comms": Comms(),
    "network": Network()
    }

class HumanComputerInterface:

    def __init__(self):
        self.settings_file_loc =  RootPath + "local_device_data.json"
        self.data = None
        try:
            with open(self.settings_file_loc) as r:
                self.data = load(r)
        except IOError:
            user = droid.dialogGetInput('Username', 'Create a new username:').result
            device = droid.dialogGetInput('Device Name', 'Enter the device name: ').result
            password, seed = self.generate_password()
            if user is None or device is None or password is None:
                alert_dialog_n_response("Error", "Bad information input! Exiting...")
                exit(1)

            self.data = {
                "user": str(user), 
                "device": str(device),
                "seed": str(seed),
                "pass_hash": str(password),
                "genesis": EnsureST(datetime.now())
                }
            try:
                with open(self.settings_file_loc, 'w+') as w:
                    dump(self.data, w)
            except IOError:
                alert_dialog_n_response("Error","Unable to write settings file...")

        self.ensure_user()
        notify("BRAIN", "Local login - " + self.data["user"] + " @ " + EnsureST(datetime.now()))

        self.mainLoop()

    '''
        Validate a username and password
    '''
    def validate_user(self, user, password):
        return user == self.data["user"] and str(self.hash_password(password, self.data["seed"])) == self.data["pass_hash"]

    '''
        Ensure the validity of the user against known data
    '''
    def ensure_user(self):
        user = droid.dialogGetInput('Username', 'Please enter your username: ').result
        password = droid.dialogGetPassword('Password', 'Please enter account password').result
        if self.validate_user(user, password):
            return True
        else:
            notify("BRAIN", "Failed login - " + self.data["user"] + " @ " + EnsureST(datetime.now()))
            alert_dialog_n_response("Login Failed", "You just failed to login. Please re-launch.")
            exit(1)

    '''
        Hash a plain text password with a seed
    '''
    def hash_password(self, plain_text, seed):
        h = sha256()
        h.update(plain_text.encode('utf-8'))
        h.update(str(seed).encode('utf-8'))
        return h.hexdigest().encode('utf-8')
       
    '''
        Generate a user's password and seed
    '''
    def generate_password(self):
        password = droid.dialogGetPassword('Password', 'Please enter an account password').result
        seed = str(randint(0,2**64)).encode('utf-8')
        if password is not None:
            return self.hash_password(password, seed), seed
        return None, None

    '''
            MAIN BODY
    '''
    def mainLoop(self):
        self.system_start = datetime.now()
        ClearScreen()
        cout("cyan", "Current user: " + self.data["user"])
        cout("cyan", "System up at: " + EnsureST(self.system_start))
        self.prompt = ctxt("cyan", self.data["user"]) + ctxt("red", "> ")

        try:
            while True:
                x = input(self.prompt).strip()

                if x == 'h' or x == 'help' or x == '?':
                    cout("yellow", '''
           
    Interactions
    -------------------------------
    cls, clear ... Clear the screen
    h, help ...... Help information
    q, quit ...... Quit program
    jrnl ......... Start journal interface
    -------------------------------
                    ''')

                    cout("red", '''
    Daemons
    -------------------------------
    senses ....... Sensors
        senses [start|stop|about]

    comms ........ Communications
        communit [start|stop|about
                  screen|check]

    network ...... Networkinf Info
        network [scan|ping|reach]
    -------------------------------
                    ''')

                elif x == 'q' or x == 'quit':
                    self.shutdown()

                elif x == 'clear' or x == 'cls':
                    ClearScreen()

                else:
                    x = x.split(" ")
                    script = x[0]
                    arguments = x[1:]

                    returnInfo = None

                    if script in ModuleRouter:
                        cout("lightgreen", "\t[ModuleRouter]")
                        returnInfo = ModuleRouter[script].entryCall(arguments)

                    if script in DaemonRouter:
                        cout("lightgreen", "\t[DaemonRouter]")
                        returnInfo = DaemonRouter[script].entryCall(arguments)

                    if returnInfo is not None:
                        print(ctxt("blue", "Title: ") + ctxt("yellow", returnInfo.info))
                        print(ctxt("blue", "Messages: "))
                        for m in returnInfo.messages:
                            cout("yellow", m)

        except KeyboardInterrupt:
            self.shutdown()

    def shutdown(self):
        ClearScreen()
        cout("red", "...Program terminating...")
        exit(1)




if __name__ == '__main__':
    HumanComputerInterface()

