#include <Arduino.h>
#include <EnDecode.h>
#include <VehicleIf.h>

#ifndef DRONE_TXRX_HEADER_GUARD
#define DRONE_TXRX_HEADER_GUARD

namespace COMM
{
class WirelessTXRX
{
public:
    WirelessTXRX(VEHICLE::VehicleIf *vehicle);
    ~WirelessTXRX();

    void tick();

private:
    VEHICLE::VehicleIf *vehicle;
};
}
#endif