'''
    Josh A Bosley
    The core is in charge of processing user-defined operations
    on the node. This includes the sending/receiving data to 
    nodes based on conditionals within their script.

    The scripting for the core will be different than standard 
    functional programming. Ever half second the core 'rotatates'
    and re-executes the script. 
'''

import time
import threading
from sys import exit
from datetime import datetime
from xnscommon import settings
from xnscommon import device_settings
from xnscomm import NetworkTransmitter
from xnscommon import OmniClock
from xnsdisplay import cout

import multiprocessing

class Core(threading.Thread):
    
    def __init__(self):
        
        threading.Thread.__init__(self)
        self.signal = True

        '''
            While these are operated on by the core
            and the scripts ran through it, the information
            in the signal_pool, and slot_router are maintained
            entirely by DeviceManager (Handed to on init)
            Core only reads signals, as they are volatile.
        '''
        self.signal_pool = {}
        self.slot_router = {}
        self.completed_requests = {}

        # A clock to manage timed events
        self.oclock = OmniClock()

        # Tickets given to us from requests out to other nodes
        # Each item should be a dict {address, resp_id, ticket_number)
        # Where resp_id is the location core will place the response
        self.pending_tickets = []

        self.completed_tickets = []

        # Requested information from other nodes will be stored here
        self.request_responses = {}

        # Flag to ensure the DataManaager
        # has set up the pool and the router
        self.dm_init = False

        # Depending on launch mode, handle how rotate operates
        self.handle_rotation = False

        # Local network transmitter for sending out requests
        self.transmitter = NetworkTransmitter(settings)

        # Instruction Index
        self.iIndex = 0;

        # The instruction set to operate on
        self.instruction_set = []

        # Instructions to be triggered if a condition is emt
        self.trigger_set = []

        # Local user-defined path variables
        self.variable_space = {}

        # Local user-defined data variables
        self.data_space = {}

        # Local execution results - bound to change often
        self.volatile_space = {}

    '''
        This is where the instructions are folded into each other
        that-is to say, The left-hand side is executed with information
        from the right hand side. Rhs could be a method, or data. It depends
        on the specified isntruction
    '''
    def rotate(self):
            
        '''
                    TRIGGER SET IS WORKING-ISH
                    
                    
                    DONT REMOVE REQUESTS!!! JUST ENSURE THAT THEY ARE BEING UPDATED EACH ROTATION
                    AND NOT DUPED 


        '''

        # Process any potential triggers from the last go-around
        for x in self.trigger_set:
            
            # Make sure the required response 
            condition_raised = True
            if x["lhs"] in self.data_space:
                lhs = self.data_space[x["lhs"]]
            elif x["lhs"] in self.request_responses:
                lhs = self.request_responses[x["lhs"]]
            else:
                condition_raised = False

            if x["rhs"] in self.data_space:
                rhs = self.data_space[x["rhs"]]
            elif x["rhs"] in self.request_responses:
                rhs = self.request_responses[x["rhs"]]
            else:
                condition_raised = False

            if condition_raised:
                if x["op"](lhs,rhs):
                    print("Light On!")
                    print(x["action"])
                    self.generateRequest((x["addr"], x["action"]))
                else:
                    print("Light off")

            
                
        # Process the instruction set
        for x in self.instruction_set:
            x[0](x[1])
            self.iIndex += 1

    '''       Once the signal is set, and dm_init, start ticket checking and rotating   '''
    def run(self):
        while self.signal:
            
            # Ensure everything is loaded
            if self.dm_init:
                self.checkTickets()

            # Ensure that the core is suppose to rotate its self
            if self.handle_rotation:
                
                # The rotation will potentially eat the instructions, so copy out
                self.temp_ins = list(self.instruction_set)

                # Rptate the instructions
                self.rotate()

                # Copy ins back
                self.instruction_set = list(self.temp_ins)

                # Back at the top of instruction
                self.iIndex = 0

            # Don't eat the cpu
            time.sleep(0.5)

    '''     Check for updates on our tickets                        '''
    def checkTickets(self):
        
        if len(self.pending_tickets) == 0:
            self.completed_tickets = []
            return
    
        #   Use an omniclock instance to ensure we only spend a little bit of time per-cycle 
        #   checking up on tickets. After extensive tests between this function and a multiprocess 
        #   queue it they two averaged out to be the same in the amount of time it took to process
        #   all tickets in the system. 

        self.oclock.newCounter("___CORE_CLOCK___")
        for x in self.pending_tickets:
            
            # This method, and multiprocessing does around 5 tickets per second
            # Thats including (Sending the request, getting a ticket, them processing it, us updating)
            if self.oclock.secondsPassed("___CORE_CLOCK___") >= 1:
                break

            # Create the request to get the ticket
            req = "U$" + x["ticket_number"]

            # Send it to the issuing node
            resp = self.transmitter.outgoingQuery(x["address"],settings["port"],req)

            # This means something aweful has happened. Kill everything
            if resp == None:
                print("XNSCore->checkTickets->Fatal Error on response from : ", x["address"], ". About : ", req)
                exit(1)

            # If its not an error, add the result
            if resp != settings["error"]:
                self.request_responses[x["resp_id"]] = resp
                
                # If its no longer pending, add it to completed
                if resp != settings["pending"]:
                    self.completed_tickets.append(x)
            else:
                print("XNSCore->checkTickets->", settings["error"], " : returned from resp. ", req)
                print("This could be because 'pending' is not in the remote node's config")
                exit(1)

        # Don't need this after this
        self.oclock.removeClock("___CORE_CLOCK___")
        # Remove all of the completed tickets from pending_tickets
        for x in self.completed_tickets:
            if x in self.pending_tickets:
                self.pending_tickets.remove(x)

    ''' Take an instruction in, and generate the request            '''
    def generateRequest(self, ins):
        
        # Submit the request and get a ticket
        ticket = self.transmitter.outgoingQuery(ins[0], settings["port"], ins[1])

        # Store the information for checking the ticket later
        self.pending_tickets.append(
            {
                "address": ins[0],
                "resp_id": ins[1],
                "ticket_number": ticket,
            }
        )

    ''' Trigger the thread to start rotating on its own '''
    def triggerRotation(self, ins):
        self.instruction_set = list(ins)
        self.handle_rotation = True

    '''     Oblivion                            '''
    def devNull(self,ins):
        # This is useful-ish 
        return
    
    '''     Perform a loop with rotation        '''
    def rotateLoop(self, ins):
        
        # The list is broken out to make more sense. 
        cur = ins[0]
        lim = ins[1]
        action = ins[2]
        opt = ins[3]

        # Perorm the action 
        action(cur)
        
        # If cur = lim
        if cur == lim:
            return

        # Modify curr by opt lambda method passed in
        cur = opt(cur)

        # self.iIndex is the location of the call to this function,
        # so we'll add a new call in the next slot until base-case is hit
        self.instruction_set.insert(
            self.iIndex + 1, (self.rotateLoop, (cur, lim, action, opt))
        )
