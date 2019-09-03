#ifndef RFNET_STDAGENT_HEADER
#define RFNET_STDAGENT_HEADER

#include <SPI.h>
#include <RH_NRF24.h>
#include "RFNetStdClientIF.h"

/*
    The standard agent is a very basic class
    The user must decide which is the server and which is the client
    node non-addressed. Clients (Primary senders) must noo tick the
    agent, instead their primary function is to send. The send function
    will check to ensure that the message was received on the other end.
    The tick function will be used by the 'server' that is constantly 
    listening for data to act on or store (decided by stdClientIf impl)
*/

namespace RFNET
{
class StdAgent
{
public:
    /*
        \brief Agent class for interfacing with the wirless module
        \param client   The StdClinetIf object to perform callbacks on
        \param ce_pin   SPI CE Pin
        \param csn_pin  SPI CSN Pin
        \param channel  NRF24 RF Channel
        \param dataRate Data transfer rate
        \param power    Transmission power
    */
    StdAgent(StdClientIF * client, unsigned ce_pin, unsigned csn_pin, unsigned channel,
          RH_NRF24::DataRate dataRate, RH_NRF24::TransmitPower power);
    ~StdAgent();


    /*
        \param data     Data to send out over RF
                        - Max length is RH_NRF24_MAX_MESSAGE_LEN
    */
    void send(size_t len, uint8_t data[]);

    /*
        \brief Tick the agent to perform current loop operations
    */
    void tick();

private:
    RH_NRF24    *_nrf24;
    StdClientIF *_client;
};
}

#endif