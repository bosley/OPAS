# Currently available commands

**Active Screen Display** 

*asd* - This screen will display node information, and all known nodes. The color scheme of the known nodes depicts the last contact they've had with the current node. Green is the most recent, followed by yellow, then red (5, 10, 15 minutes respectively.) If the node hasn't contacted at the end of 15 minutes, a probe command is sent to the node to ensure activity (Carried out by NodeNexus.) If the node isn't able to be contacted, they are removed from known nodes and assumed offline. The screen will refresh whenever new information is gathered by the nexus, and every 5 minutes. 

**Core**

This is still under construction. The core takes care of sending requests of inquiry to other nodes in regards to their hardware status, and handles triggers that arise from this node by requests from self, or remote nodes. Running the core command will allow a user to manually enter commands into the core, but its best if the core is fed from file. In the future, cli access to the core may be removed with the core being required to be fed a file for the functionality to be enabled. --The node can be ran without the core doing anything, it is solely for handling local/remote device triggers.

**dev**

List the devices attached to the local node.

**devs.<node>**

List the devices attached to a given node (Can call by uid, name, or location.)

**global**

This is still under construction. This command starts the global network interface. When started, if there is a known beacon it can be selected, and the node will bind to it. Eligibility of being a beacon relative to the local node is as follows:

A known node must have a separate nest address (Public address)
*or*
A known node must have the name:
```
__FORCED_BEACON__
```

The former need not be named as such, and is meant for nodes that are truly global in their reach. The latter is meant for local nodes that need to be promoted to beacon wither for testing, or for large local networks.

Starting the command as **global.relay** will start the local node as a full-blown relay with no more CLI access.

**help**

Display the help menu.

**load**

Drops the user into a load-file prompt. All files (comma separated) will be fed into the core line by line. For available core syntax, check the docs/ directory.

**ls**

List all of the known nodes.

**man**

Manually add a node by ip. This allows the adding of global nodes that are out-of-reach of the scanner.

**rat**

This is still under construction. Ideally this will allow the remote execution of a command. This is super insecure, and may be pruned all-together. 

**reach**

The reach command allows the user to do perform a manual probe request to a known nod by uid, name or location.

**scan**

The scan function will generate all similar IPV6 addresses (first 3 octets are the same as the local node  192.168.1.*) and attempt to handshake with the address. If the handshake fails, they aren't considered a node device. If it passes, a probe request is sent and the node is logged. 

**stat**

A simple status print out.