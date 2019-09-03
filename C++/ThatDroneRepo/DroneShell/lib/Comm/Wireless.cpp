#include <Wireless.h>

#include <Settings.h>
#include <Arduino.h>
#include <EnDecode.h>

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

namespace COMM
{

namespace {
    //
    //  Radio Pins CE, CSN
    //
    RH_NRF24 driver(NRF24_CE_PIN, NRF24_CSN_PIN);

    //
    //  Datagram manager
    //
    RHReliableDatagram manager(driver, ADDRESS_RECEIVER);

    uint8_t ack[] = "ack";
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
}

WirelessTXRX::WirelessTXRX(VEHICLE::VehicleIf *vehiclePtr) : vehicle(vehiclePtr)
{
  if (!manager.init())
  {
    #ifdef SETTINGS_SERIAL_OUTPUT
        Serial.println("wireless init failed");
    #endif
  }
}

WirelessTXRX::~WirelessTXRX()
{

}

void WirelessTXRX::tick()
{
    if(manager.available())
    {
        uint8_t len = sizeof(buf);
        uint8_t from;

        if (manager.recvfromAck(buf, &len, &from))
        {
            // Decode the data, and send it to the vehicleIf
            vehicle->handleRcData(COMM::decode({buf[0], buf[1]}));

            // Send a reply back to the originator client
            if (!manager.sendtoWait(ack, sizeof(ack), from))
            {
            #ifdef SETTINGS_SERIAL_OUTPUT
                Serial.println("sendtoWait failed, but its okay =/");
            #endif
            }
        }
    }
}

}
