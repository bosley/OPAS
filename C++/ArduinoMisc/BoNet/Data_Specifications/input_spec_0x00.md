# Specification 0x00

The specification ID for this particular data layout is **0x00**, meaning the *Spec ID* field should be encoded to **0x00** to conform to *network_data.md*.

# Inputs

Each ID starts at byte 5 (1-indexed) of the transmission data, with the first byte being the spec ID, the second being the node ID, the third being the sensor type ID, and the fourth being the sub ID (see network_data.md). Each ID then has a remaining 28 bytes to pack a message for consumption by the base station.

| Sensor Type ID   | Title                    |
|--                |--                        |
| 0                | Device Status            | 
| 1                | Temperature Sensor       |
| 2                | Humidity Sensor          |
| 3                | Basic Movement Sensor    |
| 4                | Button                   |
| 5                | Light Sensor (basic)     |

[ As more are added, they will be listed ]

____

### **ID 0**- Device Status

Rate: Once on boot when device is ready. 

Data:

Sensor ID must be set to '0'

The next byte of data will indicate how many inputs the node contains, with each following byte of data containing ID of an input that is supplied by the node. This means that any given node can contain up-to 27 inputs.

____

### **ID 1** - Temperature Sensor

Rate: 1x 30 Seconds

Data:

| Byte 1 | Byte 2 - 5  |
|--      |--           |
| Unit   | Measurement |

Measurements will be 4 bytes containing integer representation of temperature (temp * 100.0)

Units:
  
  - 0 - Fahrenheit
  - 1 - Celsius 
  - 2 - Kelvin 
  - 3 - Rankine

____

### **ID 2** - Humidity Sensor 

Rate: 1x 30 Seconds

Data:

Humidity measurements will be in 4 bytes where the value will be (humidity * 100.0)

Note:

It is common to have temperature measurements come in via humidity sensors. If the humidity sensor also supplied this measurement it should come in as a separate 'temperature' sensor id.

____

### **ID 3** - Basic Movement Sensor

Rate: iff movement is detected or ended

Data:

The movement sensor will transmit only 1 byte. This byte will contain '1' if movement is detected, and a '0' if the movement has stopped.

____

### **ID 4** - Button

Rate: On change

Data:

The button sensor will transmit only 1 byte. This byte will contain '1' if the button is 'on' or 'active' and '0' if 'off' or 'inactive'.

____

### **ID 5** - Light Sensor (basic)

Rate: On change

Data:

The data will be 2 bytes representing an unsigned 16 bit integer indicating the level of brightness read from the sensor.


