#ifndef JOYSTICK_HEADER
#define JOYSTICK_HEADER
#include <Arduino.h>

// FWD for controller
class RCATTController;

//
//  Remote Control All The Things - Joystick
//  
//  A class to help polling joystick modules
//
class RCATTJoystick
{
public:

    //
    //  Register a joystick giving the x, y, and switch (push-down) pin.
    //  Tolerance is to offset any minor error in read or manufact defect
    //
    RCATTJoystick(
                  uint8_t local_address,
                  uint8_t x_analog, uint8_t y_analog, uint8_t sw_digital,
                  uint8_t x_min,    uint8_t x_max,    uint8_t x_rest,
                  uint8_t y_min,    uint8_t y_max,    uint8_t y_rest,
                  uint8_t rest_tolerance,
                  uint8_t sw_rest);

    //
    //   Register controller for calling back on activity
    //
    void registerController(RCATTController *controller);

    //
    //  Check the joystick activity
    //
    void pollJoystick();

private:
    uint8_t _local;
    uint8_t _xan;
    uint8_t _yan;
    uint8_t _swd;
    uint8_t _xmin;
    uint8_t _xmax;
    uint8_t _xrest;
    uint8_t _ymin;
    uint8_t _ymax;
    uint8_t _yrest;
    uint8_t _rt;
    uint8_t _swrest;

    RCATTController *_controller;
};

#endif