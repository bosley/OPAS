# RCATT
Remote Controll All The Things!

RCATT is a very basic RF controller. Based on RadioHead and the NRF24L01P module.

Inside RCATT the following classes can be found:
    RCATTJoystick
    RCATTController
    Oled
    JDLL

## RCATTJoystick
A joystick class that is used to define a physical joystick (similar to the ones on XBOX, PS*). It is defined to handle 3 types of controlls: X, Y, Switch (Momentary push down of joystick). 

Each joystick must be given an arbitrary address, 2 analog pins (x,y) and 1 
digital pin (switch) upon constrcution along with the expected range of values
that can be polled from each and their corresponding 'resting' position, and finally, a tolerance value. Tolerance is to limit potential noise that may 
lead to inaccurate readings. 

This class will only relay joystick movements if the data polled from the joystick
exists outside of the tolerance range from its designated resting position.

## RCATTController
A controller class that stores the defined joysticks. Callbacks are setup between joystick and controller to enable reporting joystick movement to the controller during a poll request.

The controller handles system state information, the oled class (if enabled),
and the transmitting of values off of the controller using RadioHead's Datagram
method. Encrypted transmission may later be added as RadioHead incorperates a
block cypher into it's library, but that will be further down the road (if at all).

Inside the main program loop, the controller class is asked to 'tick' each loop.
This tick function will iterate over known joysticks and ask them to poll themselves for any input. Polling was decided on instead of interrupts from the 
controller as I wanted to be able to add an arbitrary number of joysticks to a 
controller while also switching arduino boards. If activity is found, the joystick
will callback to the controller indicating what is going on and the controller will radio the information out.

## Oled
A basic oled handler that is used to display a few lines on a small oled.
This can be ignored on build by commenting out the define for it in Settings.h
It isn't really useful, nor is it required for anything to operate. It may be 
removed later as it doesn't partake in anything to a serious degree. Its just kind
of neat.

## JDLL
Nothing essential to processing information, this is just a templated double linked list data structure used to store unknown lenghts of things inside the controller.

# Settings

To handle simple things like enabling serial output, and some RF information Settings.h was created. Its a lump of defines that everyone references to change various program settings for runtime and compiletime changes.

## Side notes:

Why poll the joysticks instead of using interrupts?
    From what was read, arduino handles interrupts differently depending on the board and some work is required to change he pins you want to call interrupts on. This is lame.

Why use a non-standard storage structure instead of something like std::list ? 
    JDLL was written, and I wanted to see if I could use it here. change it if you want. 

## Testing

This has been run on the Arduino Nano and Mega. The specific ones can be found in the platform.ini file. 

## Pinouts
Joystick (0)  (1)   (NANO & MEGA)
X        A0   A2
Y        A1   A3
SW       D2   D3

NRF24  (NANO)   (MEGA)
MISO    D12      D50
MOSI    D11      D51
SCK     D13      D52
CE      D9       D9
CSN     D10      D53

