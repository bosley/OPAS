#include "RFNetStdAgent.h"

#include <SPI.h>
#include <RH_NRF24.h>
#include "RFNetDefines.h"
#include "RFNetStdClientIF.h"

namespace RFNET
{

StdAgent::StdAgent(StdClientIF * client, unsigned ce_pin, unsigned csn_pin, unsigned channel,
        RH_NRF24::DataRate dataRate, RH_NRF24::TransmitPower power)
{
    if(!(client == nullptr))
        _client = client;

    _nrf24 = new RH_NRF24(ce_pin ,csn_pin);

    if(!_nrf24->init())
        _client->fault(Faults::NRF24_INIT);

    if(!_nrf24->setChannel(channel))
        _client->fault(Faults::NRF24_SET_CHAN);

    if(!_nrf24->setRF(dataRate, power))
        _client->fault(Faults::NRF24_SET_RF);
}

StdAgent::~StdAgent()
{
    delete _client;
    delete _nrf24;
}

void StdAgent::send(size_t dlen, uint8_t data[])
{
    _nrf24->send(data, dlen);

    _nrf24->waitPacketSent();

    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (_nrf24->waitAvailableTimeout(RFNET_TIMEOUT_MS))
    {
        if (_nrf24->recv(buf, &len))
        {
            /*
                Check here for an ack
            */
        }
        else
        {
            _client->fault(Faults::GENERAL_RECV);
        }
    }
    else
    {
        _client->fault(Faults::TIMEOUT_RECV);
    }
    delay(RFNET_DELAY_SEND);
}

void StdAgent::tick()
{
    if (_nrf24->available())
    {
        uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (_nrf24->recv(buf, &len))
        {
            _client->dataRecvd(len, buf);

            uint8_t data[] = "1234567890ABCDEF";
            _nrf24->send(data, sizeof(data));
            _nrf24->waitPacketSent();
        }
        else
        {
            _client->fault(Faults::GENERAL_RECV);
        }
    }
}

}