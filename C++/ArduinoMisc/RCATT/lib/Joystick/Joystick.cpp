#include <Arduino.h>
#include <Jdll.h>
#include <Joystick.h>
#include <Controller.h>
#include <RHReliableDatagram.h>
#include <Settings.h>

RCATTJoystick::RCATTJoystick(
                  uint8_t local_address,
                  uint8_t x_analog, uint8_t y_analog, uint8_t sw_digital,
                  uint8_t x_min,    uint8_t x_max,    uint8_t x_rest,
                  uint8_t y_min,    uint8_t y_max,    uint8_t y_rest,
                  uint8_t rest_tolerance,
                  uint8_t sw_rest) : 
                                _local(local_address),
                                _xan(x_analog),
                                _yan(y_analog),
                                _swd(sw_digital),
                                _xmin(x_min),
                                _xmax(x_max),
                                _xrest(x_rest),
                                _ymin(y_min),
                                _ymax(y_max),
                                _yrest(y_rest),
                                _rt(rest_tolerance),
                                _swrest(sw_rest),
                                _controller(nullptr)
{
    pinMode(sw_digital, INPUT);
    digitalWrite(sw_digital, HIGH);
}

void RCATTJoystick::registerController(RCATTController *controller)
{
    if(_controller == nullptr)
        _controller = controller;
}

void RCATTJoystick::pollJoystick()
{
    uint8_t read = digitalRead(_swd);
    if(_swrest != read)
        _controller->joystick_sw(_local);
        
    read = analogRead(_xan);
    if(read > (_xrest + _rt) || read < (_xrest - _rt))
        _controller->joystick_x(_local, read);

    read = analogRead(_yan);
    if(read > (_yrest + _rt) || read < (_yrest - _rt))
        _controller->joystick_y(_local, read);
}
