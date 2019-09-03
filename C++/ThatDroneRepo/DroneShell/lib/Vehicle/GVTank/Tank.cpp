#include <Arduino.h>
#include <Settings.h>
#include <EnDecode.h>
#include <GVTank/Tank.h>

namespace VEHICLE
{
Tank::Tank()
{

}

Tank::~Tank()
{

}

void Tank::tick()
{
    /*
    
            Read from some sensors and do some things
    
    */
}

void Tank::handleRcData(COMM::wData data)
{

#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Tank::execute(");
    Serial.print(data.joystick);
    Serial.print(", ");
    Serial.print(data.control);
    Serial.print(", ");
    Serial.print(data.value);
    Serial.println(")");
#endif
}

}