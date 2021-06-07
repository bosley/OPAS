# Overview

### Foreward - an acronym / word list

HTML    - HyperText Markup Language
TCP     - Transmission Control Protocol
UDP     - User Datagram Protocol
IPC     - Inter-Process Communication
DNS     - Domain Name Server
IP      - Internet Protocol
Service - Defined here to mean a subset of software that operates independently of other functionality

(Others defined as they are used)

### The Concept

When it comes to connecting devices there are a myriad of means to do so. TCP, UDP, Radio Frequencies, light pulses, the list goes on. When an individual or business needs to connect their devices together they tend to spin-up servers or devices that handle particular subsets of their needs to ensure the whole of their idea / product comes together. One machine gathers data and another executes or stores. The point from A -> B usually requires an implementation of some communication network or service that they pay for to ensure their data arrives safely. 

The Obelisk POC aims to aide the '->' above by ensuring the data-in-transit is secure, and gets where it is going. Usually, an individual or business needs to open a server up to the public and hope their security team ensures nobody can break in. The ObeliskPOC offers up a means to safely send data from private network to private network without needing to expose any part of that network directly to the public. Essentially creating a 'secure line' from A -> B. Kind-of like the red phone often depicted in Hollywood movies that you can contact POTUS on.

### CIPC (Command IPC) - Required

Nodal service that facilitates the communication of
local software to the node for the sake of controlling
the state of the node (online, offline, etc)

### NIS (Node Information Service) - Required

A service that is exposed to the outside world that handles queries to/from the node over HTML, JsonRPC regarding the configuration of the node

### NSS (Node Sender Service) - Optional

A gateway-of-sorts that facilitates connection to the Obelisk network (outbound) for software running on the same machine as the node.

### NRS (Node Receiver Service) - Optional

Facilitates receiving data from the Obelisk network (inbound) for software running on the same machine as the node.

    NOTE: Write an app that talks to NRS and NSS communication to demonstrate interaction with the services the way a user would. Consider naming that demo app 'red phone' 

### NPS (Node Pillar Service) - Optional

A designation as-well-as a service dictating that the node software will serve as a pillar of the Obelisk network. This meaning that the node will be used to repeat/extend/strengthen the network by facilitating a hop functionality. Essentially becoming a relay in the network. 

### NOS (Node Oracle Service) - Optional

Another designation as-well-as a service. The Oracle will monitor pillars on the Obelisk network and examine traffic to aide pillars in determining the best route from a sending node to a receiving node. Oracles will also share information with each other regarding pillar up/down status and network statistics. Oracles also serve as a network DNS to map Obelisk Pillar IDS to the public IP address of which they are associated (not all pillars will contain public addresses, so only Obelisks with public IPS will be listed). 

