#ifndef CONTROLLER_HEADER
#define CONTROLLER_HEADER
#include <Arduino.h>
#include <Jdll.h>

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#include <Settings.h>

// FWD for joystick
class RCATTJoystick;

// FWD for oled
#ifdef SETTINGS_USE_OLED
    class Oled;
#endif

//
//  Controller class that manages the entire controller and all of its
//  assets.
//
class RCATTController
{
public:


    RCATTController();

#ifdef SETTINGS_USE_OLED
    //
    //  Add a screen
    //
    void registerOled(Oled *oled);
#endif

    //
    //  Inform controller program about to begin
    //
    void begin();

    //
    // Add a joystick
    //
    void registerJoystick(RCATTJoystick *joystick);

    // Cycle program
    void tick();

    ////// Callback functions //////
    
    // -Joystick
    void joystick_x(uint8_t  address, uint8_t value);
    void joystick_y(uint8_t  address, uint8_t value);
    void joystick_sw(uint8_t address);

    // -Buttons
    // -Power down ?

private:
    // Joysticks
    UTIL::Jdll<RCATTJoystick*> joysticks;

    enum class Status
    {
        LOADING,        // Startup
        OPERATIONAL,    // Fully Operational
        WARNING,        // Operational error
        NO_RESP,        // Cant contact receiver
        ERROR           // Non-operational error
    };

    Status controllerStatus;
#ifdef SETTINGS_USE_OLED
    Oled * loled;

    bool updateScreen;
#endif

#ifdef NRF24_ACTIVE
    //
    //  Transmit data using RadioHead datagrams
    //
    //  \param joystick     The Joystick address of the sender
    //  \param control      X, Y, SW     (0,1,2)
    //  \param value        Value sent by control
    //
    void transmit(uint8_t joystick, uint8_t control, uint8_t value);
#endif

};

#endif