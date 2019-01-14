'''

    Josh A Bosley
    This class handles the parsing of text into rotating instructions
    for the XNS core. After the text is processed, if not engaged 
    with the CLI, the XNS Core will continue to rotate the given 
    instructions.

        Example instruction:
                    x = (function, <function | data>) 

        Execution:
                    x[0](x[1])

        Using methods in Core and lambdas, this is an effective method
        for continuously executing user instructions without having to
        parse them every time we wish to execute them 
'''

import string
from sys import exit
from json import dumps as jdumps
from json import loads as jloads

from xnsdb import DataStore
from xnscommon import settings
from xnscommon import device_settings
from xnsdisplay import cout, ctxt

class CoreInteraction:

    def __init__(self, core):
        self.core_th = core
        self.crun = False
        self.forgiving = False
        self.inCliMode = False
        self.core_rolling = False

        # Data base 
        self.ds = DataStore()
        self.known_nodes = self.ds.getNodeActivity()

        # Primary tokens
        self.ptokens = {
            "commit": self.handInstructions,
            "print": self.printInstruction,
            "req":self.requestInstruction,
            "loop": self.loopInstruction,
            "sp": self.assignInstruct,
            "sd": self.dataInstruct,
            "if": self.ifInstruct,
        }

        # Tokens that map to core, or node settings
        self.self_relating_tokens = {
            "pool": self.core_th.signal_pool,
            "tickets": self.core_th.pending_tickets,
            "retinfo": self.core_th.request_responses,
            "comptick": self.core_th.completed_requests,
            "sp": self.core_th.variable_space,
            "sd": self.core_th.data_space,
            "self": device_settings,
            settings["name"]: settings["UID"],
            "address": settings["address"],
            "nodes": self.known_nodes,
        }

        # Instructions to carry out
        self.instruction_set = []

        # Reserved chars
        self.reserved = ["$", "."]

    '''         This is called to load from file           '''
    def loadFromFiles(self, files):
        
        # update nodes
        self.known_nodes = self.ds.getNodeActivity()
        self.self_relating_tokens["nodes"] = self.known_nodes

        # Load in each line after doing some trimming
        total_lines = []
        for x in files:
            try:
                with open(x, 'r') as file:
                    for line in file.readlines():
                        line = line.replace("\n", "").replace("\t", "").replace(" ", "")
                        if len(line) >= 1 and line != "":
                            total_lines.append(line)
            except OSError:
                cout("red", "File [" + x + "] was not found. Aborting.")
                return
        
        # Ensure the 'commit' command is at the end.
        if "commit" not in total_lines:
            total_lines.append("commit")

        # Send all the lines to be parsed
        [self.parseInstruct(x) for x in total_lines]


    '''         Parse input from cli                        '''
    def cliBegin(self):
        
        if self.core_rolling:
            cout("fail", "\n\t Cannot engage active core. \n\n")
            return
        
        info = '''

    When engaging with the core in CLI mode, 
    here are some things to keep in-mind :
    > To roll the core once, you must type 'commit'
    > Requests to nodes caused by 'req' are only executed once
        - They must be re-entered post commit to run again
    > Triggers created by 'if' statements are kept in memory
        and do not need to be re-added. With that said, any 
        trigger entered will act on 'old' requests if a new 'req'
        isn't entered. 

    This might not seem ideal for run-time but thats because cli mode
    is meant for testing scripts and debugging, not production.

    (Type 'exit' to return to node's human interface)

        '''
        cout("cyan", info)

        # update nodes
        self.known_nodes = self.ds.getNodeActivity()
        self.self_relating_tokens["nodes"] = self.known_nodes

        # We are running!
        self.crun = True

        # Tndicate that we are in acutal cli mode
        self.inCliMode = True

        # Tell it to ignore bad commands 
        self.forgiving = True

        cprompt = ctxt("red", "core") + "@" + ctxt("yellow", settings["address"]) + "> "

        # Begin run
        while self.crun:
            
            x = input(cprompt)

            if x == "exit":
                self.shutDown()
            elif x == "help":
                print("\n"*5)
                cout("yellow", "\n\tType 'exit' to return\n\n\n")
            else:
                if x != "":
                    self.parseInstruct(x)

    '''         Ensure the core is stable before exit       '''
    def shutDown(self):
        
        self.crun = False

    '''         Handle a parsing exception                   '''
    def parseException(self, e):
        print(ctxt("fail", "\nException : ") + ctxt("yellow", e))
        print("")
        if self.forgiving:
            return
        else:
            exit(1)
    
    '''         Entry point for parsing an instruction        '''
    def parseInstruct(self, ins):

        '''
             Turn a human command into something for the core
             Core will take a list of tuples for isntruction.
             (methodToCall, data)
        '''

        '''  Commit entirety of code to endinge, and execute    '''
        if "commit" == ins:
             return self.ptokens["commit"](True)

        '''     Call to execute an isntruction                    '''         
        if '@' == ins[0]:
            return self.executeInstruct(ins)

        if '#' == ins[0]:
            return

        ''' Statement Instruction   '''
        if "." in ins:
            
            pieces = ins.split(".")

            # Check to see if the lead tag is valid. If it is, send the data to it
            func = None
            if len(pieces) == 1:
                args = []
            else:
                args = pieces[1:]
            try:
                func = self.ptokens[pieces[0]]
            except KeyError:
                self.parseException("Invalid line : [" + ins + "]")

            # The call was removed, as variable assigning produced KeyErrors when
            # given new keys. There may be a better way to handle this though,
            if func != None:
                func(args)
            else:
                self.parseException("Invalid line : [" + ins + "]")

    '''         Pass the instruction set over to the core       '''
    def handInstructions(self, args):

        # If we aren't in command 
        if not self.inCliMode:
            
            self.core_rolling = True
            # Get the core rolling
            self.core_th.triggerRotation(self.instruction_set)

            # If parsing is complete, and core is loaded its time to leave
            # The core will continue running the instructions provided
            return
        else:
            # If we are in command line mode, we want to 'process once' 
            # followed up by a reset of core instructions
            # Create a copy of the ins, and the core instructions to it
            self.core_th.instruction_set = list(self.instruction_set)

            # Start rotation
            self.core_th.rotate()

            # If we are in cli, clear the instructions, and keep going
            self.core_th.instruction_set = []
            self.core_th.temp_ins = []
            self.core_th.iIndex = 0
            self.instruction_set = []

    '''         A means to ensure data is passed through XNS       '''
    def sanatizeData(self, data):
        for x in self.reserved:
            data = data.replace(x, "")
        return data

    '''         Check to see if an XNS Network path is valid       '''
    def xnspathIsValid(self, ins):
        
        # Its best to double check
        self.known_nodes = self.ds.getNodeActivity()
        self.self_relating_tokens["nodes"] = self.known_nodes

        # Returns information to do a request on that path in addition
        # to the path input
        if len(ins) < 3:
            self.parseException("Error: xnsPath must contain at least <node>.<smodule>.<routine | slot>")
            return None
        
        # Flags for idendtifying at least 1 node is found
        a = False
        addr = "_UNSET_"
        for node in self.known_nodes:
            # Locate the node
            if node["uid"] == ins[0] or node["name"] == ins[0] or node["location"] == ins[0]:
                a = True

                # Ensure that 0 was the id. 
                ins[0] = node["uid"]
                addr = node["address"]

                # Convert the string into json acceptable format, and then load into devices
                devices = jloads(node["devices"].replace("'", "\""))

                b = False
                # Check all devices for the given dev
                for dev in devices:
                    
                    # Found the device, now ensure that its a routine, or a slot
                    if dev["id"] == ins[1]:
                        b = True

                        # if it is, ensure it has a length of 4 (data stream) in case one wasn't given
                        if ins[2] in dev["routines"]:
                            if len(ins) != 4:
                                ins.append("_UNSET_")
                        elif ins[2] in dev["slots"]:
                            if len(ins) != 4:
                                ins.append("_UNSET_")
                        else:
                            self.parseException("Error: Request has invalid slot/module. Scan, then try again.")
                            return None
                if b == False:
                    self.parseException("Error: Request has unknown device name. Scan, then try again.")
                    return None
        if a == False:
            self.parseException("Error: Request has unknown node specified. Scan, then try again.")
            return None
        
        # If this is reached, all is well
        ins.append(addr)
        return ins

    '''         Create print instruction                           '''
    def printInstruction(self, ins):
        
        # Take everything to print in, cycle
        for i in ins:
            
            # if they are in the self_related category 
            if i in self.self_relating_tokens:
                
                # Create a lambda rotation
                self.instruction_set.append(
                    (lambda x : print(jdumps(x, indent=4)), self.self_relating_tokens[i])
                )

    '''         Load an instruction that will generate a request   '''
    def requestInstruction(self, ins):

        # See if another function called us, they'll want the function, 
        # not an instruction to be made
        flag_react = False
        if ins[-1] == "__RETURN_INST_TO_CALLER__":
            flag_react = True
            del ins[-1]

       # # Clean up instruction
        ins = [self.sanatizeData(x) for x in ins]
        ins = [x for x in ins if x != ""]

        # Check to see if it is a variable
        if len(ins) == 1:
            
            # If it is, and its valid : Create the isntruction
            if ins[0] in self.core_th.variable_space:
                ins = self.core_th.variable_space[ins[0]].split(".")

        # Ensure all required pieces are passed in
        if len(ins) < 3:
            self.parseException("Requests require (<node>.<device>.<routine | slot>) at-minimum")
            return
        
        # Ensure the path has a valid node, device, and router/slot.
        res = self.xnspathIsValid(ins)

        if res == None:
            self.parseException("None-type returned from xnspathIsValid (requestInstruction)")
            return

        # Concatenate the query and get the ip out of ins (from calling xnspathIsValid()
        datas = "$" + ins[3]
        del ins[3]
        addr = ins[-1]
        del ins[-1]
        mid = "$" + '.'.join(ins)
        req = "A$" + settings["UID"] + mid + datas

        # If this is set, we don't want to create the isntruction
        if flag_react:
            return (addr, req)

        # Create the generate request instruction, then we're all done
        self.instruction_set.append(
            (self.core_th.generateRequest, (addr,req))
        )

    '''         Hook a routine or slot to a variable                '''
    def assignInstruct(self, ins):
        
        # Clean the assignment 
        ins = [self.sanatizeData(x) for x in ins]
        ins = [x for x in ins if x != ""]

        # input : varName.uid.device.sor
        if len(ins) < 4:
            self.parseException("Illegal path variable assignment. Expect < varName.uid.dev.sor > .ds optional")
            return

        # After ensuring that it has enough info to be a path, we assume it is. 
        # It will be evaluated upon being called. 
        self.core_th.variable_space[ins[0]] = '.'.join(ins[1:])

    '''         Assign data to a variable                            '''
    def dataInstruct(self, ins):

        ins = [self.sanatizeData(x) for x in ins]
        ins = [x for x in ins if x != ""]

        if len(ins) < 2:
            self.parseException("Illegal data variable assignment. Expect < varName.data > ")
            return
        
        # Extract var, and del
        var = ins[0]
        del ins[0]

        # To ensure we allow '.' in the data stream
        data = '.'.join(ins)

        # Store the data
        self.core_th.data_space[var] = data

    '''         Execute an isntrcution                               '''
    def executeInstruct(self, ins):
    
        print("Execute : " , ins)
        self.core_th.volatile_space = {}
        # Check to see if rhs is in vars. If it is, execute it. 
        # If not, check to see if its a valid path and execute it
        # Place result data into 

        return

    '''         Perform a loop with rotations                         '''
    def loopInstruction(self, ins):

        '''
                Call core's rotate loop and input the desired 
                modifier Ex:  (lambda x: x + 1)

        '''
        if len(ins) != 4:
            self.parseException("Invalid loop. Loops require <from>.<to>.<step>.<action>")
            return

        try:
            f = int(ins[0])
            t = int(ins[1])
            s = int(ins[2])
        except ValueError:
            self.parseException("Error: from, to, and step must be integers!")
            self.parseException("Loops require <from>.<to>.<step>.<action>")
            return

        # Make sure the user isn't trying to kill the computer with an infinite loop
        if abs(f-t) % s != 0:
            self.parseException("Infinite loop detected! Exiting")
            return
        
        # Set a function for inc or dec 
        if f < t:
            opt = (lambda x: x + s)
        if t < f:
            opt = (lambda x: x - s)

        action = ins[3]

        '''

                Add the calling of a variable to 
                add things like toggling a light 10x etc


        '''

        # Right now only printing is supported
        if action == "print":
            action = print
        else:
            action = self.core_th.devNull

        self.instruction_set.append(
            (
                self.core_th.rotateLoop,
                 (f, t, action, opt )
            )
        )
        return

    '''     Create a reaction to a request          '''
    def ifInstruct(self, ins):

        ins = [self.sanatizeData(x) for x in ins]
        ins = [x for x in ins if x != ""]

        if len(ins) != 4:
            self.parseException("If expects : if.<lhs>.<comparison>.<rhs>.<action>")
            return

        # Comparisons
        comp = {
            "gt": lambda lhs, rhs : True if lhs > rhs else False,
            "lt": lambda lhs, rhs : True if lhs < rhs else False,
            "gte": lambda lhs, rhs : True if lhs >= rhs else False,
            "lte": lambda lhs, rhs : True if lhs <= rhs else False,
            "eq": lambda lhs, rhs : True if lhs == rhs else False,
        }

        if ins[1] not in comp:
            self.parseException("Illegal comparison")
            return

        # Operation
        op = comp[ins[1]]

        # If the lhs is going to be in request_responses
        if ins[0] in self.core_th.variable_space:
            
            # Grab the path variable, and construct a request
            lhs = self.core_th.variable_space[ins[0]]
            lhs = lhs.split(".")
            lhs.append("__RETURN_INST_TO_CALLER__")
            lhs = self.requestInstruction(lhs)[1]

        # Else, it might be in the data space as a plain-text var
        elif ins[0] in self.core_th.data_space:

            # Assign the lhs
            lhs = ins[0]

        # if not, then its bad
        else:
            self.parseException("Invalid LHS. Must be a path, or a data variable")
            return
            
        # If the rhs is going to be in request_responses
        if ins[2] in self.core_th.variable_space:
            
            # Grab the path variable, and construct a request
            rhs = self.core_th.variable_space[ins[2]]
            rhs = rhs.split(".")
            rhs.append("__RETURN_INST_TO_CALLER__")
            rhs = self.requestInstruction(rhs)[1]

        # Else, it might be in the data space as a plain-text var
        elif ins[2] in self.core_th.data_space:

            # Assign the rhs
            rhs = ins[2]

        # if not, then its bad
        else:
            self.parseException("Invalid RHS. Must be a path, or a data variable")
            return
            
        # Get the variable data, and construct what the query would look like
        # So we can reference the retinfo data
        if ins[3] in self.core_th.variable_space:
            
            action = self.core_th.variable_space[ins[3]]
        else:
            print(ins[3], " not in var space")
            return
        
        # Retrieve the action to perform
        action = self.core_th.variable_space[ins[3]]
        action = action.split(".")
        action.append("__RETURN_INST_TO_CALLER__")
        addr, action = self.requestInstruction(action)

        # Make the trigger
        trigger = {
            "lhs": lhs,
            "rhs": rhs,
            "op": op,
            "addr": addr,
            "action": action,
        }

        '''

            Don't have to make an instruction, because triggers aren't altered by
            instructions, or other triggers thus they aren't ephemeral in nature.
            Once they are placed, they can stay

        '''

        # There can be only one!
        if trigger not in self.core_th.trigger_set:
            self.core_th.trigger_set.append(trigger)
        return

        