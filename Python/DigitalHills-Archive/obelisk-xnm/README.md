# Obelisk - X Network Messaging

The obelisk network project was meant to enable the communication of
devices behind a NAT by way of a public server. Since having a single centralized 
server comes with a severe weak point, it was decided that a distributed network
of synchronized servers was a better option. 

The obelisk network pillars will synchronize network activity to ensure a copy of data
on each pillar such-that if a node is to lose contact with its primary provider it can 
automatically switch over to another server and continue communications.

### Node Process

Each node (usage found in main.py) once started for the first time requires a server
to be manually added to its database. Once this server is added, the node will reach out
and pull all known servers, all known nodes, and all of its messages (if any exist.)
After initial contact, the node can continue operation provided that a single server remains
online. If it loses connection to its current provider, it simply selects a different server
to provide for it.

### Server Process

Once a server (pillar) is created in the Obelisk network, any other existing pillar will have to
be injected into its database (example in main.py.) If a server appears in its database, the server
will reach out and attempt to synchronize.

### RSA Signing 

Each node when attempting to contact a server will send a registration request which allows the 
passing of public keys to and from the server. This enables each server to ensure the validitiy 
of the message. A similar process takes place from server to server as well. 

### Further development

As-of the creation of this document, the signature process is working, but creates the same key-pair
on each generation. This is not safe for deployment, and would need to be edited before ensuring 
any protection against MITM attacks. 

The system as-is is built for inclusion in a project. Main.py is only an example of how the node and pillar
construct can be used within other projects.



