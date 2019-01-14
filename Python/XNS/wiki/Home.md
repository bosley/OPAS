# X N S Wiki

## Getting Started : 

Until things take great shape, they will by roughly outlined here. Once a segment becomes a considerable size, they will have their own area.

## Setting up a new node - Basic configurations

For all configuration options run :
```
./configure.py
```

The options under source deal with configurations of the current node. On first-use,
```
./configure.py source config
```
must be ran to ensure a proper settings file is generated. This will generate a unique id 
for the node, and it will asked to be named, and located.


To configure devices / actions that can be triggered or used to trigger actions on the network, run: 
```
./configure.py gen devices
```
This will ask you for a skeleton file that describes the devices that you want to load. 
(The specifics of which need to be made into their own page [signals, and routines])

Once the devices are generated 'xnsexternal' will hold all of their respective .py files that were created, and they will be added into the router. When writing device files always remember that returning 'None' from anything will send back an error code, anything that doesn't produce an error needs to be an arbitrary string value. 

To ensure that everything goes smoothly, run:
```
./configure source devices
```
Running source on devices is done automatically, at the end of the "gen devices" command, but it may make you feel better knowing that your changes were molded into the router. Its also good for testing and fixing poorly manually-edited routers. 


## Setting up a new node - Starting the node

```
sudo ./entry.py
```

Once the node is started up, you can use 'help' to list all of the available commands. Once commands come to completion in development, they will have their own place in the wiki to discuss their usage and their configurations in more detail. 
**Core, Global Network, Beacons, Transmission, Work requests, Tickets, Hardware scheduling**


## Setting up a new node - Resetting the node

If something goes wrong, and it doesn't warrant the purge-and-rebirth then these commands may come in handy:


```
sudo ./configure.py clean devices
```
Purges all device files (no backups), and resets the router.

```
sudo ./configure.py clean node
```
Remove the database containing all known nodes, and all logs. 


```
sudo ./configure.py clean network
```
Remove all global network information stored locally.

```
sudo ./configure.py clean databases
```
Runs the previous two commands to clean the node, and the network settings



# Pages needed to be made:

* Those that appear on this page in bold.
* Skeleton files
* Core instructions
* GN addressing
* Color scheme and NodeNexus
* Encryption, and unavailable chars