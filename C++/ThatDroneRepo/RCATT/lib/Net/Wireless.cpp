#include <Wireless.h>

#ifdef NRF24_ACTIVE
//
//  Radio Pins CE, CSN
//
RH_NRF24 RCATTdriver(NRF24_CE_PIN, NRF24_CSN_PIN);

//
//  Datagram manager
//
RHReliableDatagram RCATTmanager(RCATTdriver, ADDRESS_CONTROLLER);

#endif

namespace NET
{
    bool doInit()
    {
        return RCATTmanager.init();
    }

    bool writeBytes(uint8_t *data, size_t length, uint8_t to)
    {
        // Receiver buff
        uint8_t buff[RH_NRF24_MAX_MESSAGE_LEN];
        
        if (RCATTmanager.sendtoWait(data, sizeof(data), ADDRESS_RECEIVER))
        {
            uint8_t from;   
            uint8_t len = sizeof(buff);
            if (RCATTmanager.recvfromAckTimeout(buff, &len, 100, &from))
            {
                return true;
            }
        }
        return false;
    }

    

}