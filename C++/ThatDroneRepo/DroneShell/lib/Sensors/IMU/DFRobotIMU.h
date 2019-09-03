#ifndef DFROBOT_IMU_H
#define DFROBOT_IMU_H

#include <IMU/IMUDeviceIf.h>
#include <IMU/IMUCallbackIf.h>

namespace SENSORS { namespace IMU {

class DFRobotIMU: public SENSORS::IMU::DeviceIf
{
public:
    DFRobotIMU(CallbackIf * callback);
    ~DFRobotIMU();

    virtual void tick() override;

private:

    CallbackIf *imuCallback;

    double eulerAngleX;
    double eulerAngleY;
    double eulerAngleZ;

    double angularVelX;
    double angularVelY;
    double angularVelZ;

    double initX;
    double initY;
    double initZ;

    struct pair
    {
        double ypr;
        double vel;
    };
    
    pair yaw;
    pair pitch;
    pair roll;

    double tempC;

    double pressurePa;

    double altitudeM;

    void readAll();

    void getYaw(pair &yaw);
    void getPitch(pair &pitch);
    void getRoll(pair &roll);
};

}}

#endif