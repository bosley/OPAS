
#ifndef VEHICLE_ENDECODE_HEADER_GUARD
#define VEHICLE_ENDECODE_HEADER_GUARD
#include <Arduino.h>

namespace COMM
{
    struct wData
    {
        uint8_t joystick; // [0, +1]
        uint8_t control;   // [0, +3]
        uint8_t value;    // [0, +255]
    };

    struct wBytes
    {
        uint8_t b0;
        uint8_t b1;
    };

    inline wBytes encode(wData data)
    {
        wBytes encoded = {0, data.value};
        encoded.b0 =  (data.joystick  << 7);
        encoded.b0 |= (data.control   << 2);
        return encoded;
    }

    inline wData decode(wBytes encoded)
    {
        wData data;
        data.value      =  encoded.b1;
        data.joystick   = (encoded.b0 & 0xF0) >> 7;
        data.control    = (encoded.b0 & 0x0F) >> 2;
        return data;
    }
} // COMM


#endif