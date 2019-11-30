
from time import sleep
from xnmlib import XNMNode

from platform import uname 
from os import chdir, getcwd, listdir
from os.path import isfile, join
from subprocess import Popen, PIPE, call

with open("local_node.config", "r") as r:
    lnode = r.readline()

local_node = XNMNode(lnode)
local_node.supressText = False
local_node.start()

IDLE = True
DEBUG = True
NODEWD = getcwd()
TEMPWD = None
TOOLS = NODEWD + "/tools/"
SYSINFO = ':'.join(uname())
TOOLLIST = '\n'.join([f for f in listdir(TOOLS) if isfile(join(TOOLS, f))])

def run_cmd(cmd):

    global TEMPWD
    global TEMPWD
    global IDLE
    global DEBUG
    global SYSINFO
    global TOOLLIST

    if DEBUG:
        print("DO: ", cmd)

    pieces = cmd.split(" ")

    if pieces[0] == "KILL":
        if DEBUG:
            print("Going idle")
        IDLE = True
        return ""

    if pieces[0] == ":":

        if pieces[1] == "cd":
            if pieces[2] == "BWD":
                TEMPWD = None
                chdir(NODEWD)
                return "wd changed to '" + NODEWD + "'" 
            else:
                TEMPWD = pieces[2]
            return "wd changed to '" + pieces[2] + "'" 
        else:
            if TEMPWD is not None:
                chdir(TEMPWD)

            p = Popen(pieces[1:], stdin=PIPE, stdout=PIPE, stderr=PIPE)
            output = p.stdout.read().decode("utf-8") 

            if TEMPWD is not None:
                chdir(NODEWD)
            return output

    if pieces[0] == ">":
        try:
            local = TOOLS + pieces[1]
        except:
            return "Command error"

        ################################################################################
        '''
                TODO:
                THERE NEEDS TO BE A WAY TO DIFFERENCIATE .SH AND .PY TOOLS
                ALSO NEED TO EXPAND CALL TO ENCOMPASE THE PASSING OF VARS
                INTO THE TOOLS
        '''
        ################################################################################

        r = call(["python3", local])
        if r != 0:
            return "Error running " + pieces[1] + ". - Perhaps it doesn't exist"
        return "success"

    if pieces[0] == "?":
        return SYSINFO

    if pieces[0] == "tools":
        return "\nTool List: \n\n" + TOOLLIST

    return "Unknown command"


if __name__ == '__main__':
    msgs = []
    while True:

        if IDLE:
            wait = True
            msgs.clear()
            while wait:
                sleep(0.5)
                if not local_node.received_messages.empty():
                    msgs.append(local_node.received_messages.get())
                    wait = False

            for m in msgs:
                if m["fro"][:5] == "z3cnc":
                    if m["message"][:4] == "INIT":
                        IDLE = False
                        local_node.send_message(m["fro"], "LISTENING", previous_hash=m["hash"])

                        if DEBUG:
                            print("z3cnc connected. Listening.")
        else:
            msgs.clear()
            while not IDLE:
                sleep(0.5)
                if not local_node.received_messages.empty():
                    msgs.append(local_node.received_messages.get())

                if len(msgs) > 0:
                    for m in msgs:
                        if m["fro"][:5] == "z3cnc":
                            if DEBUG:
                                print("Got a new message from z3cnc")
                            local_node.send_message(m["fro"], run_cmd(m["message"]), previous_hash=m["hash"])
                            msgs.clear()


local_node.kill()