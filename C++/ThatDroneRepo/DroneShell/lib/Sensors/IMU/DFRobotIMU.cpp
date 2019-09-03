
#include <Settings.h>
#include <IMU/DFRobotIMU.h>
#include <DFRobot_BNO055.h>
#include <DFRobot_BMP280.h>

namespace SENSORS { namespace IMU {

DFRobot_BNO055 mpu;
DFRobot_BMP280 bmp;

DFRobotIMU::DFRobotIMU(CallbackIf * callback) : imuCallback(callback),
                                                eulerAngleX(0.0),
                                                eulerAngleY(0.0),
                                                eulerAngleZ(0.0),
                                                angularVelX(0.0),
                                                angularVelY(0.0),
                                                angularVelZ(0.0),
                                                tempC(0.0),
                                                pressurePa(0.0),
                                                altitudeM(0.0)
{
    if(mpu.init())
    {
#ifdef SETTINGS_SERIAL_OUTPUT
            Serial.println("Good: Mpu init");
#endif
        if(bmp.begin())
        {
#ifdef SETTINGS_SERIAL_OUTPUT
            Serial.println("Good: Bmp init");
#endif
            // Now that both are initd do an initial read
            delay(10);
            readAll();

            // Grab init values
            initX = eulerAngleX;
            initY = eulerAngleY;
            initZ = eulerAngleZ;
        }
        else
        {
#ifdef SETTINGS_SERIAL_OUTPUT
            Serial.println("Error: Bmp init fail");
#endif
        }
    }
    else
    {
#ifdef SETTINGS_SERIAL_OUTPUT
        Serial.println("Error: Mpu init fail");
#endif
    }
}

DFRobotIMU::~DFRobotIMU()
{

}

void DFRobotIMU::tick()
{
    readAll();

    yaw   = {0.0, 0.0};
    pitch = {0.0, 0.0};
    roll  = {0.0, 0.0};

    getYaw(yaw);
    getPitch(pitch);
    getRoll(roll);

    imuCallback->yaw(yaw.ypr, yaw.vel);
    imuCallback->pitch(pitch.ypr, pitch.vel);
    imuCallback->roll(roll.ypr, roll.vel);

    imuCallback->temperature(tempC);
    imuCallback->pressure(pressurePa);
    imuCallback->altitude(altitudeM);
}

void DFRobotIMU::readAll()
{
    mpu.readEuler();
    eulerAngleX = mpu.EulerAngles.x;
    eulerAngleY = mpu.EulerAngles.y;
    eulerAngleZ = mpu.EulerAngles.z;

    mpu.readAngularVelocity();
    angularVelX = mpu.GyrData.x;
    angularVelY = mpu.GyrData.y;
    angularVelZ = mpu.GyrData.z;

    tempC = bmp.readTemperatureValue();
    pressurePa = bmp.readPressureValue();
    altitudeM = bmp.readAltitudeValue();
}

/*
    The following seem backasswords, but its what the API states.
    Z = Yaw     = Euler X
    X = Pitch   = Euler Y
    Y = Roll    = Euler Z

    ===============================

    These are functioned out instead of setting the variables directly in-case 
    error correcting is added in for each of the values
*/

void DFRobotIMU::getYaw(pair &yaw)
{
    /* In order to match the API of the upper computer, Z ----> yaw  */
    yaw.ypr = eulerAngleX;
    yaw.vel = angularVelZ; // Matching vel is not X
}

void DFRobotIMU::getPitch(pair &pitch)
{
    /* In order to match the API of the upper computer, X ----> pitch  */
    pitch.ypr = eulerAngleY;
    pitch.vel = angularVelY;
}

void DFRobotIMU::getRoll(pair &roll)
{
    /* In order to match the API of the upper computer, Y ----> roll  */
    roll.ypr = eulerAngleZ;
    roll.vel = angularVelX ; // Matching vel is not Z
}

}}