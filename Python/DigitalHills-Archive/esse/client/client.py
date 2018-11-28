'''
    Copyright 2017 Digital Hills, LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
'''

'''

        This file is meant to serve as a usable TX/Address generator
        as-well-as example code for interacting with the nexus for 
        the creation of wallets, etc

'''
from os import name, system
from pathlib import Path
from json import loads
from re import match
from time import sleep

from display import cout, ctxt
from settings import node_settings
from client import generate_address
from client import generate_transaction
from client import send_transaction
from chain import transaction

'''
        Command Line interface for client methods
'''
class EsseClient:

    def __init__(self, _node_nexus):

        self.cls = "clear"
        if name == 'nt':
            self.cls = "cls"

        # Make sure a data folder and txs folder is present
        Path("./essedata/txs").mkdir(parents=True, exist_ok=True) 

        # Reference to node's nexus thread
        self.node_th_nexus = _node_nexus

        self.help = '''

    exit .................... Exit the client
    clear ................... Clear the screen
    new address  ............ Create a new address 
    new tx .................. Generate a tx
    load tx ................. Load file of pre-defined txs
    pending tx .............. List pending txs
    push tx ................. Push all pending txs to server
    
        '''

        self.pending_txs = []

        self.interface()

    def interface(self):
        system(self.cls)
        cout("fail", "[Use 'exit' to stop client]")
        
        while True:
            try:
                uin = input(ctxt("blue", "esse-client> "))
                if uin == "exit":
                    break
                if uin == "help" or uin == "h":
                    cout("yellow", self.help)
                if uin == "clear":
                    system(self.cls)
                if uin == "new address":
                    self.form_address()
                if uin == "new tx":
                    self.form_transaction()
                if uin == "load tx":
                    self.load_transaction()
                if uin == "pending tx":
                    self.show_pending_txs()
                if uin == "push tx":
                    self.push_pending_txs()
            except KeyboardInterrupt:
                cout("fail", "\n[Use 'exit' to stop client]")
                pass


    '''
        Create an address and key-pair
    '''
    def form_address(self):
        cout("fail", "WHEN CREATING AN ADDRESS, [ USE A UNIQUE KEY NAME ], OTHERWISE OLD KEYS COULD BE OVERWRITTEN")
        got_something = False
        try:
            while not got_something:
                got = input(ctxt("yellow", "Enter name of key (for file)> "))
                if match('^[\w-]+$', got) is None:
                    cout("fail", "Invalid key ... Must be alphanumeric")
                    pass
                else:
                    confirm = input(ctxt("fail", ("Is [" + got + "] correct (y/n)?")))

                    if confirm == "y":
                        got_something = True
                    elif confirm == "n":
                        pass
                    else:
                        cout("bold", "Please use 'y' or 'n'")
        except KeyboardInterrupt:
            print("")
            return        
        # Ensure keys dir exist
        kpath = "./essedata/keys/" + got 
        Path(kpath).mkdir(parents=True, exist_ok=True) 

        # Make the address, and files
        parts = generate_address()
        pubk = kpath + "/public.pem"
        prik = kpath + "/private.pem"
        addk = kpath + "/address.txt"

        # Write
        try:
            open(pubk,"w+").write(parts[0].decode('utf-8'))
            open(prik,"w+").write(parts[1].decode('utf-8'))
            open(addk,"w+").write(parts[2])
        except OSError:
            cout("fail", "Failed to write files!")

        print(
            ctxt("cyan", "Files written to: [") + ctxt("yellow", kpath) + ctxt("cyan", "] directory.")
        )
        print(ctxt("lightgreen", "Your new address is : ") + ctxt("yellow", parts[2]))

    '''
        Create a new tx
    '''  
    def form_transaction(self):
        cout("fail", "[REMINDER: ENSURE THAT THE ADDRESSES USED ARE ON THE CORRECT NETWORK]")
        got_something = False
        try:
            while not got_something:
                got = input(ctxt("yellow", "Enter name of key folder (in ./essedata/keys/)> "))
                if match('^[\w-]+$', got) is None:
                    cout("fail", "Invalid key ... Must be alphanumeric")
                    pass
                else:
                    confirm = input(ctxt("fail", ("Is [" + got + "] correct (y/n)?")))

                    if confirm == "y":
                        got_something = True
                    elif confirm == "n":
                        pass
                    else:
                        cout("bold", "Please use 'y' or 'n'")
        except KeyboardInterrupt:
            print("")
            return     
        key_used = got
        kpath = "./essedata/keys/" + got 
        pubk = kpath + "/public.pem"
        prik = kpath + "/private.pem"
        addk = kpath + "/address.txt"
        
        try:
            public_key = open(pubk,"rb").read().decode('utf-8')
            private_key = open(prik,"rb").read().decode('utf-8')
            address = open(addk,"rb").read().decode('utf-8')
        except IOError:
            cout("fail", "Unable to open key information..")
            return

        cout("lightgreen", "[Keys Loaded]")
        cout("fail", "If address is listed twice, a double-pay may occur.")
        cout("fail", "If an address doesn't exist, the TX will be dropped!")        
        while True:
            num = input(ctxt("yellow", "How many addresses to send to?> "))
            try:
                num = int(num)
                if num > 0:
                    break
                else:
                    cout("fail", "Integer must be greater than 0.")
            except ValueError:
                cout("fail", "Type Error: Need to enter an integer!")

        to_addresses = []
        for i in range(1,num+1):
            to_addresses.append(input(ctxt("yellow", ("To-Address [" + str(i) + "]> "))))

        amount_to_send = None
        while True:
            num = input(ctxt("yellow", "How much to send to the addresses?> "))
            try:
                num = float(num)
                if num > 0:
                    amount_to_send = num
                    break
                else:
                    cout("fail", "Amount must be greater than 0.")
            except ValueError:
                cout("fail", "Type Error: Need to enter an float!")

        if amount_to_send is None:
            cout("fail", "Error: amount_to_send is None!")
            return(False, "None Amount")
                
        cout("cyan", "Please ensure that the following addresses are correct: ")
        [cout("lightgreen", ("=>\t" + x)) for x in to_addresses]

        cout("lightgreen", "If all addresses are correct, and the value is correct, please type 'correct' ")
        
        confirmation = input(ctxt("fail", ": "))

        if confirmation != "correct":
            cout("fail", "Transaction creation cancelled, failed to type 'correct'")
            return(False, "Didn't confirm!")

        cout("yellow", "...Creating TX...")

        tx = (generate_transaction(address, to_addresses, amount_to_send, public_key, private_key))
    
        if not isinstance(tx, transaction):
            cout("fail", "Error generating transaction: ")
            print(tx)
        else:
            self.pending_txs.append(tx)
            cout("lightgreen", "Transaction added to pending transaction list.")

        # Write tx to file
        txpath = "./essedata/txs/" + key_used + "-" + str(tx.txId) + ".tx"
        try:
            open(txpath, "w+").write(tx.json())
        except IOError:
            cout("fail", "Unable to save TX to file.")

    '''
        Load transaction(s) from file
    '''
    def load_transaction(self):

        if len(self.pending_txs) > 0:
            cout("fail", "There are pending transactions preventing load function.")
            cout("fail", "Please push current transactions before loading tx from file.")
            return

        print("LOAD SOME TRANSACTION(s)")
        got_something = False
        try:
            while not got_something:
                got = input(ctxt("yellow", "Enter name of key folder (in ./essedata/keys/)> "))
                if match('^[\w-]+$', got) is None:
                    cout("fail", "Invalid key ... Must be alphanumeric")
                    pass
                else:
                    confirm = input(ctxt("fail", ("Is [" + got + "] correct (y/n)?")))

                    if confirm == "y":
                        got_something = True
                    elif confirm == "n":
                        pass
                    else:
                        cout("bold", "Please use 'y' or 'n'")
        except KeyboardInterrupt:
            print("")
            return     

        tag = got + "-*.tx"
        pathlist = Path("./essedata/txs/").glob(tag)

        index = 0
        cpaths = []
        for path in pathlist:
            cpaths.append(str(path))
            cout("cyan", ( str(index) + "  => " + str(path)))
            index += 1

        cout("yellow", "Which tx(s) would you like to load ( [n] or [n-m] )? ")
        selection = input(ctxt("fail", ":"))
        m = None

        if "-" in selection:
            selection = selection.split("-")
            if len(selection) < 2: 
                cout("fail", "Need form 'n-m'")
                return
            try:
                n = int(selection[0])
                m = int(selection[1])
            except ValueError:
                cout("fail", "n, and m must be integers!")
                return

            if m <= n or m > index-1:
                cout("fail", "m must be greater than n, and less than or equal to index!")
                return
        else:
            try:
                n = int(selection)
            except ValueError:
                cout("fail", "n must be an integer!")
                return

        if n > index-1 or n < 0 :
            cout("fail", "Invalid n")
        if m is None:
            m = n

        for x in range(n, m+1):
            try:
                ctx = open(cpaths[x],"rb").read().decode('utf-8')
            except IOError:
                cout("fail", "Unable to open TX!")
                return
            ctx = loads(ctx)
            _ctx = transaction(ctx["type"], ctx["data"], ctx["key"], ctx["sig"], ctx["time"], ctx["id"])
            self.pending_txs.append(_ctx)

        cout("lightgreen", ("Loaded [" + str((m-n)+1) + "] txs into pending txs."))
        cout("yellow", "You will need to push these transactions before loading more from file(s).")

    '''
        Show Pending TXs
    '''
    def show_pending_txs(self):
        if len(self.pending_txs) == 0:
            cout("cyan", "\nThere are no pending transactions!")
            return
        print("\n\n")
        for tx in self.pending_txs:
            cout("cyan", ("\t" + tx.txId + " => " + tx.timestamp ))
        print("\n")

    '''
        Push the pending TXs
    '''
    def push_pending_txs(self):
        if len(self.pending_txs) == 0:
            cout("yellow", "There are no pending txs to push")
            return
    
        cout("yellow", "[Sending all pending transactions]")
        # Attempt to send all tx, if they return an error, add them back into pending
        unsent = []
        for _tx in self.pending_txs:
            result = send_transaction(_tx.json())
            if result[0]:
                cout("lightgreen", (_tx.txId + " => " + result[1] ))
            else:
                cout("fail", (_tx.txId + " => " + result[1]))
                unsent.append(_tx)
        # Re-add all un-sent tx into pending
        self.pending_txs = unsent