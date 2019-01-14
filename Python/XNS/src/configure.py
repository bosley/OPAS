#!/usr/bin/python3

import sys
from os import listdir, remove
from os.path import isfile, join
from xnscommon import settings
from xnscommon import NodeSetup
import json


def help():
    o = '''
    \n\n\n
    Available commands : 

        source
            - config        (Configure node settings)
            - devices       (Reload attatched devices)

            [ ./configure.py source config ]

        gen
            - devices       (Create new devices from skeleton file)
            
        clean 
            - devices       (Remove and re-generate device files)
            - node          (Remove general xns database)
            - network       (Remove xns global beacon database)
            - databases     (Remove node database, and beacon database)
    '''
    print(o)


def source(args):
    
    if args[2] == "config":
        ns = NodeSetup(settings)
        ns.sourceDevice()
        return

    if args[2] == "devices":
        devs = "xnsexternal"
        files = [f for f in listdir(devs) if isfile(join(devs, f))]
        
        router = "_router.py"
        initf = "__init__.py"

        if router not in files or initf not in files:
            print("Error : xnsexternal is missing key files. Exiting")
            sys.exit(1)
        
        files.remove(router)
        files.remove(initf)

        if len(files) == 0:
            with open("./xnsexternal/_router.py", "w+") as r:
                r.write(
                    '''
\'''

    Josh A. Bosley 
    The device router serves as a means to interact with devices in a 
    signal/slot-like relation. The device monitor thread of XNS will
    routinely run each device's methods stored in their respective
    routine dictionaries. If an action is meant to trigger a resoinse,
    the method will return a command string back to the device monitor to
    carry out the action.
    When requested, xns will run the device's methods stored in their slot 
    dictionaries as-to allow remote control off a device (From another node.)

    This file is rewritten when running the command : 

            ./configure source devices

\'''
''')
                r.write("\n\ndevRouter = {}")
            print("Complete : No devices listed in xnsexternal")
            sys.exit(1)

        imports = []
        device_list = []
        lines = ["\n\ndevRouter = {"]

        for name in files:
            try:
                nname = name.split(".")[0]
                extension = name.split(".")[1]
            except:
                print("Invalid file in xnsexternal : ", name, "\nExiting.")
                sys.exit(1)

            if extension != "py":
                print("Invalid file in xnsexternal : ", name, "\nExiting.")
                sys.exit(1)

            if nname in device_list:
                print("Error : Duplicate device found. File name must be unique.")
                sys.exit(1)

            imports.append("from ." + nname + " import routines as " + nname + "_routines")
            imports.append("from ." + nname + " import slots as " + nname + "_slots")
            lines.append("\t\""+nname+"\": (" + nname + "_routines, " + nname + "_slots),")

        lines.append("}\n\n")

        output = ['''
\'''

    Josh A. Bosley 
    The device router serves as a means to interact with devices in a 
    signal/slot-like relation. The device monitor thread of XNS will
    routinely run each device's methods stored in their respective
    routine dictionaries. If an action is meant to trigger a resoinse,
    the method will return a command string back to the device monitor to
    carry out the action.
    When requested, xns will run the device's methods stored in their slot 
    dictionaries as-to allow remote control off a device (From another node.)

    This file is rewritten when running the command : 

            ./configure source devices

\'''
''']
        output = output + imports + lines
        with open("./xnsexternal/_router.py", "w+") as r:
            for x in output:
                r.write(x+"\n")

'''

    Reads in a device file and creates a file in xnsexternals
    for all, if they don't already exist. 


    Updates xnscommon device.json with all information

    Calls 'source' to finalize 


    Example file : 

        name:temp_sensor
        slot:current_temp
        routine:check_for_overheat
        stream:F,C,K
        #
        name:light_switch
        slot:turn_on
        slot:turn_off
        slot:toggle


'''
def genAllDevice():
    
    c = input("Location of settings file : ")
    
    # Get each device block
    with open(c, 'r') as f:
        devices = ''.join(f.readlines()).split("#")

    files = []
    device_structs = []
    for d in devices:
        name = ""
        slots = []
        routines = []
        slot_dict = ["slots = {"]
        rout_dict = ["routines= {"]
        device_struct = {}
        for l in d.split("\n"):
            data = l.split(":")
            if data[0] == "name":
                name = data[1].replace("\n", "")
                device_struct["id"] = name 
                device_struct["slots"] = [] 
                device_struct["routines"] = []
                device_struct["stream"] = "_UNSET_"

            elif len(data[0].replace("\n", "")) > 0:
                if name == "_UNSET_":
                    print("Error: Name must be set!")
                    sys.exit(1)

                if data[0] == "slot":
                    mid = "slot_" + data[1] + "_" + str(len(slots)) 
                    device_struct["slots"].append(data[1])
                    slots.append("\ndef " + mid + "(stream):\n    return \"_UNSET_\"")
                    slot_dict.append("    \""+data[1]+"\": "+ mid + ",")

                elif data[0] == "routine":
                    mid = "routine_" + data[1] + "_" + str(len(routines))
                    device_struct["routines"].append(data[1])
                    routines.append("\ndef " + mid + "():\n    return None")
                    rout_dict.append("    \""+data[1]+"\": "+ mid + ",")
                elif data[0] == "stream":
                    device_struct["stream"] = data[1].replace("\n", "")
                else:
                    print("Error: Unknown key in device list", data[0])
                    sys.exit(1)
            

        device_structs.append(device_struct)
        rout_dict.append("}")
        slot_dict.append("}")
        files.append( 
            (name, [slots, routines, slot_dict, rout_dict, device_struct])
        )

    # Write files
    for file in files:
        file_loc = "./xnsexternal/" + file[0] + ".py"
        if not isfile(file_loc):
            with open(file_loc, 'w+') as f:
                f.write(
                    '''
\'''
        This file was generated by configure.py

        slots: 
            Use slots to respond to an inquiry from
            a remote node. 

        routines:
            Routines are constantly ran to allow
            changes in a device (measurement, etc) 
            a chance to trigger an event.

\'''
                    '''
                )

                f.write("\n# Generated Slots\n")
                for x in file[1][0]:
                    f.write(x + "\n")

                f.write("\n# Generated Routines\n")
                for x in file[1][1]:
                    f.write(x+ "\n")
                
                f.write("\n# Routed dictionaries\n")
                if len(file[1][2]) == 2:
                    f.write("slots = {}\n")
                else:
                    for x in file[1][2]:
                        f.write(x + "\n")

                if len(file[1][3]) == 2:
                    f.write("routines = {}\n")
                else:
                    for x in file[1][3]:
                        f.write(x + "\n")
        else:
            print("\nSkipping ", file[0], " it already exists.")

        # Write out the hardware settings json file
        with open("./xnscommon/device_settings.json", 'w+') as f:
            f.write(json.dumps(device_structs, indent=4))

        source(["nil", "source", "devices"])

def generate(args):
    
    if args[2] == "devices":
        genAllDevice()
        
'''
    Get a list of files in xnsexternal, and delete all that aren't __init__.py
    or _router.py. Once this is performed, source devices is called
'''
def deviceClean(args):
    
    if args[2] != "devices":
        help()
        return

    devs = "xnsexternal"
    files = [f for f in listdir(devs) if isfile(join(devs, f))]
    
    router = "_router.py"
    initf = "__init__.py"

    if router not in files or initf not in files:
        print("Error : xnsexternal is missing key files. Exiting")
        sys.exit(1)
    
    files.remove(router)
    files.remove(initf)

    if len(files) == 0:
        with open("./xnscommon/device_settings.json", 'w+') as f:
            f.write("[]")
        source(["nil", "source", "devices"])
        return
    for f in files:
        try:
            remove("./xnsexternal/"+f)
        except:
            print("Unable to remove : ", f)

    with open("./xnscommon/device_settings.json", 'w+') as f:
        f.write("[]")

    source(["nil", "source", "devices"])

def nodeClean():
    location = "./xnsdb/xns.db"
    try:
        remove(location)
    except:
        print("\nError. Unable to remove [", location, "].")

def netClean():
    location = "./xnsglobal/xSRH/xSRH.db"
    try:
        remove(location)
    except:
        print("\nError. Unable to remove [", location, "].")

def clean(args):
    if args[2] == "devices":
        deviceClean(args)
    elif args[2] == "node":
        nodeClean()
    elif args[2] == "network":
        netClean()
    elif args[2] == "databases":
        nodeClean()
        netClean()
    else:
        help()
    
if __name__ == '__main__':
    
    if len(sys.argv) < 3:
        help()
        sys.exit(1)

    confs = {
        "source" : source,
        "gen" : generate,
        "clean": clean,
    }

    try:
        confs[sys.argv[1]](sys.argv)
    except KeyError:
        help()
