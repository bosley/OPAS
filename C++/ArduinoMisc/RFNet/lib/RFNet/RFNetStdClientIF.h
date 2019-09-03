#ifndef RFNET_STDCLIENT_AGENT_HEADER
#define RFNET_STDCLIENT_AGENT_HEADER

/*
    A client for RFNet for the RFAgent to callback on

*/
#include <SPI.h>
#include <RH_NRF24.h>
#include "RFFaults.h"

namespace RFNET
{
class StdClientIF
{
public:
    StdClientIF() = default;

    virtual void dataRecvd(uint8_t length, uint8_t data[]) = 0;
    virtual void fault(RFNET::Faults fault) = 0;
};
}

#endif