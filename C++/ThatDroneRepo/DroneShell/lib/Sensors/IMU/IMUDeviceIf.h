#ifndef IMU_DEFINE_H
#define IMU_DEFINE_H

namespace SENSORS { namespace IMU {

class DeviceIf
{
public:
    DeviceIf() = default;

    virtual void tick() = 0;
};

} }

#endif
