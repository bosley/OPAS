# Gobo Drone

This project started as a means practice callbacks, and json RPC serving. 
I realize there may be more efficent means of data transfer amongst networked devices,
but for simple cases this seems to work pretty well. 

The ideas was to create something that would allow for an easy network communication
between multiple devices on a network that may not all share the same functionality. 
For instance:

Device 'A' has temperature sensors, and motion sensors

Device 'B' unlocks a door, and has a remotely triggerable alarm

Device 'C' is a coffee maker

Device 'D' is a toaster with an automated defence mechanism 

In an instance where I wanted to setup communication between the devices to ensure
that if the door was opened but not unlocked, the alarm would go off and the network 
could trigger the toaster to arm its self and bring down the intruder. 
This would also allow me to quickly add a new device with different functionality to 
the network, say, a raspberry pi hooked up speakers in the house that could
be triggered by the toaster to play battle music. 

For examples on usage, checkout the example direcotry