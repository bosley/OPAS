
from dynosnode import register_slot, register_routine
from dynosnode import register_definition, ensure_registration
from dynosnode import NodeController, call_slot, call_routine
from dynosnode import get_response

# For dev sake, and for calling other nodes associated with the
# local node, the dynos name generator has been exported:
# Pass in a stringable, or callable item and the id.name will
# be returned
from dynosnode import get_dyno_id

print("\n\nThe current node_id is:\t", get_dyno_id("DyNOS-Node"))

'''     SLOTS   '''

'''
    LED slot
'''
def toggleLed(args):
    print("\n\n\t TOGGLE LED {} \n\n".format(args["state"]))
    if args["state"] == "on":
        return True
    else:
        return False

print("Registering slot:\t", 
    register_slot(
        toggleLed, 
        [("state", str)],
        bool)
    )

'''
    Toaster slot
'''
def toaster(args):

    print("\n\n\t TOASTER SLOT {} \n\n".format(args))

    return "Bread is done!"

print("Registering slot:\t", 
    register_slot(
        toaster,
        [("bread_type", str), ("intensity", int)],
        str)
    )

'''
    Security system slot
'''
def securitySystem(args):

    print("\n\n\t SECSYSTEM SLOT {} \n\n".format(args))

    print("Mode set to:", args["set_mode"])
    return "sec system set"

print("Registering slot:\t", 
    register_slot(
        securitySystem,
        [("set_mode", str)],
        int)
    )

'''     ROUTINES    '''

'''
    Motion Detector routine
'''
from random import randint
def motionDetector():
    if randint(0,11) > 7:

        call_slot("57407529962270.motionDetector", 
            "57407529962270.securitySystem", {"set_mode": "arm"})

        # Hang for response
        
        response_from_security_system = get_response("57407529962270.motionDetector",
            "57407529962270.securitySystem", defer=True)

        print("\n\n\tResponse from security system = {}\n\n\n".format(
            response_from_security_system))
        
    else:
        call_slot("57407529962270.motionDetector", 
            "57407529962270.securitySystem", {"set_mode": "disarm"})
    return True

print("Registering routine:\t", 
    register_routine(
        motionDetector,
        bool)
    )

'''
    Light sensor routine
'''
def lightSensor():
    print("The light is always on...")
    return True

print("Registering routine:\t", 
    register_routine(
        lightSensor,
        bool)
    )

'''
    Heat sensor routine
'''
def heatSensor():
    print("The heat is always on...")
    return 98.6

print("Registering routine:\t", 
    register_routine(
        heatSensor,
        float)
    )

hub_address = "127.0.0.1"
default_hub_port = 9000
local_definition = {
    "address": hub_address,
    "port": default_hub_port,
    "description": "Something to describe this node",
    "location": "Where the node exists"
}

register_definition(local_definition)
ensure_registration()

print("\n\n\tStarting Node Controller:\n")
NodeController(verbose=True)