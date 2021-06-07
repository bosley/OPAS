# Network Channel

The wireless sensor network should be explicitly set to use channel 100. This is to ensure that there isn't interference from other NRF24L01 modules working near by doing something unrelated to sensor reporting.

# Network Data

The maximum data transmission for the NRF24L01+ 2.4GHz transceiver is 32 bytes. Beyond this, messages will need to be split into multiple packets. For the sake of our initial applications, a single 32-byte message should suffice. 

Each node on the network will have its own address for he NRF, but each node should have its own unique identifier so they aren't tightly coupled to the network configuration. 


| b0         | b1         | b2             | b3      |   
|-           |-           |-               |-        |-  
| Spec ID    | Node ID    | Sensor Type ID | Sub ID  |  

____

**Byte 0** - Spec ID

Indicator detailing which data specification the node will be using to encode data from its on-board sensors.

**Byte 1** - Node ID

Sender of the message.

**Byte 2** - Sensor ID

Sensor type - Unique ID for the type of sensor which will allow us to parse the remainder of the message post byte 2.

**Byte 3** - Sub ID

If there are multiple of the given sensor types on board then the Sub ID will be greater than '0' to indicate which of the particular sensors the data relates to. If there is only one of the sensors this field should be '0'.

**Remaining Bytes**

Data of each particular sensor will vary depending on the sensor type and which specification the node is using to report the data. With 32 Max bytes, and 4 being used that leaves 28 bytes for encoded data.


## Notes

Having 4-byte headers is bulky considering the fact that we only have 32 to work with, but this scheme is not meant to handle large amounts of data. 28 should suffice for simple sensor readouts.