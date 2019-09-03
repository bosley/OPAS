#ifndef IMU_CALLBACK_h
#define IMU_CALLBACK_h

namespace SENSORS { namespace IMU {

class CallbackIf
{
public:
    CallbackIf() = default;

    virtual void yaw(double z, double angVelocity) = 0;

    virtual void pitch(double x, double angVelocity) = 0;

    virtual void roll(double y, double angVelocity) = 0;

    virtual void temperature(double c) = 0;

    virtual void pressure(double pa) = 0;

    virtual void altitude(double m) = 0;
};

} }
#endif