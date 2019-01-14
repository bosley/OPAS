#
#       Variables found here are set once, and only once
#       The purpose of this is to share across code
#
from sys import exit
from json import load as jl

def loadFile(f):
    # Load the settings file
    try:
        with open(f) as jfile:
            try:
                inf = jl(jfile)
            except ValueError:
                print("Error: Invalid " + f + " file [Potential syntax error]")
                exit(1)
    except IOError:
        print("Error: Unable to locate " + f + " [File not found]")
        exit(1)

    if f != "xnscommon/device_settings.json":
        # Ensure all settings are valid
        for key, val in inf.items():
            if val == "[UNSET]":
                print("Error: Incompleted " + f + " file [Some values are unset]")
                exit(1)
    return inf

# Contains the settings for the XNS application
settings = loadFile("xnscommon/node_settings.json")

# Contains a list of hardware available, and the commands that can be
# issued to them
device_settings = loadFile("xnscommon/device_settings.json")