#include <Arduino.h>
#include <Settings.h>
#include <EnDecode.h>
#include <AVQuad/AVQuadCC.h>
#include <AVQuad/QuadCopter.h>


namespace VEHICLE
{
QuadCopter::QuadCopter() : currentState(State::INIT)
{
    imu = new SENSORS::IMU::DFRobotIMU(this);

    readyMotors();
}

QuadCopter::~QuadCopter()
{
}

void QuadCopter::readyMotors()
{
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Setting up ESCs...");
#endif
    FR.attach(SERVO_ONE);
    BR.attach(SERVO_TWO);
    FL.attach(SERVO_THREE);
    BL.attach(SERVO_FOUR);

    /*
        Depending on ESCs used, an arming sequence might be required here
    */

#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.println("complete");
#endif
}

void QuadCopter::tick()
{
   // Step the IMU - It will callback on this 
   imu->tick();
}

void QuadCopter::handleRcData(COMM::wData data)
{
    if(currentState == State::INIT || currentState == State::Error)
    {
        return;
    }

    VEHICLE::AV::QUADCOPTER::cmd command = VEHICLE::AV::QUADCOPTER::commToCmd(data);

#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("QuadCopter::execute(");
    Serial.print(data.joystick);
    Serial.print(", ");
    Serial.print(data.control);
    Serial.print(", ");
    Serial.print(data.value);
    Serial.println(")");

    Serial.print("commToCmd-result::");
    Serial.println(command.pitch);
    Serial.println(command.yaw);
    Serial.println(command.roll);
    Serial.println(command.zAxis);
    Serial.println(command.button0);
    Serial.println(command.button1);
#endif
}

void QuadCopter::yaw(double z, double angVelocity)
{
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Yaw : ");
    Serial.print(z);
    Serial.print(", Velocity : ");
    Serial.println(angVelocity);
#endif
}

void QuadCopter::pitch(double x, double angVelocity)
{
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Pitch : ");
    Serial.print(x);
    Serial.print(", Velocity : ");
    Serial.println(angVelocity);
#endif
}

void QuadCopter::roll(double y, double angVelocity)
{
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Roll : ");
    Serial.print(y);
    Serial.print(", Velocity : ");
    Serial.println(angVelocity);
#endif
}

void QuadCopter::temperature(double c) 
{
#ifdef SETTINGS_SERIAL_OUTPUT
//    Serial.println("Temp > ");
//    Serial.print("C: ");    Serial.print(c);
//    Serial.println("");
#endif
}

void QuadCopter::pressure(double pa) 
{
#ifdef SETTINGS_SERIAL_OUTPUT
//    Serial.println("Pressure > ");
//    Serial.print("PA: ");    Serial.print(pa);
//    Serial.println("");
#endif
}

void QuadCopter::altitude(double m) 
{
#ifdef SETTINGS_SERIAL_OUTPUT
//    Serial.println("Altitude > ");
//    Serial.print("M: ");    Serial.print(m);
//    Serial.println("");
#endif
}

}