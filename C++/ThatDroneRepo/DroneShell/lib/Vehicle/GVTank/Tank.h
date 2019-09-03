#include <Arduino.h>
#include <VehicleIf.h>

namespace VEHICLE
{
    class Tank : public VehicleIf
    {
    public:
        Tank();
        ~Tank();


        // From vehicleIf
        virtual void tick();
        virtual void handleRcData(COMM::wData data);
    };
    
} // VEHICLE
