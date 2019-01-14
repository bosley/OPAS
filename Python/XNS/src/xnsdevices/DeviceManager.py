'''
    Josh A. Bosley
    The Device Manager exists as a thread from XNS entry.
    The thread is handed to the TCP request handler, and
    access to the device manager thread is given to each
    thread spawned from tcp connections. This is to ensure
    a queue-d request system as-to avoid conflicting commands
    from multiple nodes.

    In addition to serving requests from TCP handler, this class
    also is in charge of initiating device routines stored in
    xnsexternal. This class cycles between operating a single
    request, and then running all routines. This is to ensure
    the compeltion of triggers caused by requests.

    Signals from routines are stored in self.device_signal_pool
    in the standard form key => value represented as:
        uid.device.routine => last_return_value

    Slots are mapped as-such to all slots of a their respective devices
    for easier mapping of requests in self.slot_router
        uid.device.slot => slot_method
'''

import time
import threading
from xnscommon import settings
from xnscommon import device_settings
from xnsexternal import devRouter

class DeviceManager(threading.Thread):

    def __init__(self, core_th):
        threading.Thread.__init__(self)
        self.signal = True
        self.request_queue = []
        self.request_id = 0
        self.request_id_list = []
        self.completed_requests = core_th.completed_requests

        # Storage for signal values returned from the devices
        # Each key will be unique due to constraints placed on
        # the merging of hardware into XNS
        self.device_signal_pool = core_th.signal_pool

        # Router for calls to slots. 
        # This will make it easier/faster to get slot data
        self.slot_router = core_th.slot_router

        for device in devRouter:
            # Build the dictionary before-hand to allow return None's
            # in device scripts. This will allow us to not make so many
            # dictionary accesses
            origin = (settings["UID"] + "." + device + ".")
            for routine in devRouter[device][0]:
                rorig = origin + routine
                self.device_signal_pool[rorig] = None

            # Create the slot router to best process requests
            for slot in devRouter[device][1]:
                sorig = origin + slot
                # This will route incomming commands directly to the slot
                #
                # Ex: 0001.temp_sensor.get_temp => slot_get_temp_0
                #
                self.slot_router[sorig] = devRouter[device][1][slot]
        
        # Indicate to the core, that the pool and router all all setup
        core_th.dm_init = True
        
    ''' Check the attatched devices for signals             '''
    def runDeviceRoutines(self):
        for device in devRouter:
            for routine in devRouter[device][0]:
                ret = devRouter[device][0][routine]() 

                # if a signal was sent back, store it in the signal pool
                if ret != None:
                    origin = (settings["UID"] + "." + device + "." + routine)

                    # Placing according to origin, this can now be processed by a user's script
                    self.device_signal_pool[origin] = ret

    '''     Add a dict request from another node to the queue '''
    def enqueueRequest(self, request):
        sid = str(self.request_id)
        self.request_id += 1
        self.request_queue.append(
            (sid, request)
        )
        self.request_id_list.append(sid)
        return sid

    '''    Take a request from the queue, and complete it   '''
    def processQueue(self):
    
        '''
           _req = {
                "origin":req[1],
                "sor": req[2],
                "stream": req[3],
            }

            Origin is the UID of the node that sent the request
            sor could be a slot, or it could be a routine
        '''
        
        # Request is a tuple (sid, _req_dict)
        request = self.request_queue.pop(0)

        data_stream_to_return = "_UNSET_"

        # If its in the signal pool ,they want the var (from a routine)
        if request[1]["sor"] in self.device_signal_pool:
            data_stream_to_return = str(self.device_signal_pool[request[1]["sor"]])

        # If it isn't in the pool we'll need to call on a slot, passing in the stream info
        # The line looks horrible, but the xnsrequesthandler will ensure req is valid before
        # placing it in the queue
        if request[1]["sor"] in self.slot_router:
            data_stream_to_return =  str(self.slot_router[request[1]["sor"]](request[1]["stream"]))

        # If it wasn't in a pool, and it wasn't a slot, _UNET_ is returned

        # Add to the completed dict [ticket, datastream]
        self.completed_requests[request[0]] = data_stream_to_return

        # Remove the id from the wait list when done
        self.request_id_list.remove(request[0])

    def run(self):
        while self.signal:
                
            # Run the routines attatched to each device 
            self.runDeviceRoutines()

            # if we have anything int he queue, take care of it
            if len(self.request_queue) > 0:
                self.processQueue()

            # So we don't eat the cpu
            time.sleep(0.5)
