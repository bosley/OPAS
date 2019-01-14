'''
    Josh A. Bosley
    XNSRequestHandler is given all of the requests that have been
    decrypted by the TCP server. Here, the requests are delegated
    to the appropriate handlers.
'''

from json import dumps as jd
from json import loads as jl
from xnsdb import DataStore
from xnscommon import settings
from xnscommon import device_settings

# External to class for export
def probeInfo(args):
    return ('{"UID": "' + settings["UID"] + 
			'", "name": "' + settings["name"] + 
			'", "location": "' + settings["location"] + 
			'", "address": "' + settings["address"] + 
			'", "nest_address": "' + settings["nest_address"] + 
			'", "devices": ' + jd(device_settings) + '}')

class XNSRequestHandler:

    def __init__(self, deviceManager):

        # Database access
        self.ds = DataStore()

        # Access to the device manager
        self.deviceManager = deviceManager

        # Router for requests
        self.req_map = {
            'A': self.action,
            'U': self.update,
            'Q': self.respondToProbe,
            '@': self.initScript,
        }	

    '''     Log status messages to the DB           '''
    def logMessage(self, message):
        self.ds.addLogEntry(["REQUEST_HANDLER", message])

    '''     Handle a probe request                   '''
    def respondToProbe(self, req):
        self.ds.addNodeActivity(jl(req[1]))
        self.logMessage("[Probed]")
        return probeInfo(True)

    '''     Route the request to the specific handler '''
    def analyze(self, request):
        
        # The hault command just helps kill the server thread
        if request == "__HALT__":
            return settings["accepted"]

        # Ensure we can split up the chunks
        try:
            chunks = request.split('$')
        except:
            self.logMessage("Error splitting request")
            return settings["error"]
            
        # Map the lead chunk to its function, and send the data
        # if it doesn't exist, return the error
        try:
            return self.req_map[chunks[0]](chunks)
        except KeyError:
            return settings["error"]

        # If we get here something went wrong, return error
        return settings["error"]

    '''     Perform an action on a connected device     '''
    def action(self, req):
        
        # req = [A, UID, DEVICE, SLOT_OR_ROUTINE, STREAM]
        # Make a dictionary out of the request, ensuring all 
        # necissary data is in

        print(req)
        try:
            _req = {
                "origin":req[1],
                "sor": req[2],
                "stream": req[3],
            }
        except IndexError:

            # Once this is ironed out, change this to a logMessage
            print("xnsrequesthandler->action->Index Error")
            print(req)
            return settings["error"]

        # Enqueue the request dictionary for processing,
        # and send back the ticket number generated from
        # the request to process
        return self.deviceManager.enqueueRequest(_req)

    '''     Check someone's ticket, and reply           '''
    def update(self, req):

        # req = [U, TICKET]

        # If the ticket is in the list, it isn't done
        # return a 'Not Yet Done' 
        if req[1] in self.deviceManager.request_id_list:
            return settings["pending"]
        
        # If its been done, it should be in here, but if its not
        # for some reason, return an error
        try:
            return self.deviceManager.completed_requests[req[1]]
        except KeyError:
            return settings["error"]

    '''      
        Unlike core requests dealing with the control of the system
        as it relates to hardware/device interaction, this will allow
        the running of a command on the box. 

        The security of this must be double checked

    '''
    def initScript(self, req):
        print(req)

