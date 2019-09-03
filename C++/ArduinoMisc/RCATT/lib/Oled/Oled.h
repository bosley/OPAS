#ifndef OLED_HEADER
#define OLED_HEADER

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

/*
    NANO:
        SDA: A4
        SCL: A5
*/

#define OLED_TEXT_HEADER "  [ RCATT ]"


class Oled
{
public:
    Oled();

    /*
        Draw screen
    */
    void flip(const char *status, const char *message0, const char *message1);
};

#endif