#include <Settings.h>

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#ifndef WIRELESS_HEADER_GUARD
#define WIRELESS_HEADER_GUARD

namespace NET
{
    bool doInit();

    bool writeBytes(uint8_t *data, size_t length, uint8_t to);
}
#endif