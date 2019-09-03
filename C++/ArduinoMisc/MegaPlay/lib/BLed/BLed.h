#ifndef MEGA_LED
#define MEGA_LED

#include <Arduino.h>

class RGBLed
{
public:
    RGBLed(unsigned red, unsigned green, unsigned blue)
    {
        pinMode(red,    OUTPUT);
        pinMode(green,  OUTPUT);
        pinMode(blue,   OUTPUT);
        _red = red;     _green  = green; _blue  = blue;
        _sred = false;  _sgreen = false; _sblue = false;
    };
    ~RGBLed(){};

    void enableRed()
    {
        if(_sred)
            return;
        off();
        digitalWrite(_red, HIGH);
        _sred = true;
    }

    void enableBlue()
    {
        if(_sblue)
            return;
        off();
        digitalWrite(_blue, HIGH);
        _sblue = true;
    }

    void enableGreen()
    {
        if(_sgreen)
            return;
        off();
        digitalWrite(_green, HIGH);
        _sgreen = true;
    }

    void off()
    {
        if(_sred)
            digitalWrite(_red, LOW);
        if(_sblue)
            digitalWrite(_blue, LOW);
        if(_sgreen)
            digitalWrite(_green, LOW);
        _sred = false;  _sgreen = false; _sblue = false;
    }

private:
    unsigned _red, _green, _blue;
    bool     _sred, _sgreen, _sblue;
};

#endif