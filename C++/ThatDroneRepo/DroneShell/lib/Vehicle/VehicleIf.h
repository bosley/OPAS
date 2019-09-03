#include <EnDecode.h>

#ifndef VEHICLE_INTERFACE_HEADER_GUARD
#define VEHICLE_INTERFACE_HEADER_GUARD

namespace VEHICLE
{
    class VehicleIf
    {
    public:
        VehicleIf() = default;
        
        virtual void tick() = 0;
        virtual void handleRcData(COMM::wData data) = 0;
    };
} // VEHICLE


#endif