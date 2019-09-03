#include <Arduino.h>
/*
    Radio Frequency Network

    A helper class that wraps radiohead for integration into other projects without
    too much heartache.

    The main file here will serve as a test.
*/
#include <SPI.h>
#include <RH_NRF24.h>
#include <RFNetStdClientIF.h>
#include <RFNetStdAgent.h>

//
//  Setup params
//

#define CE_PIN    9
#define CSN_PIN   10


// 0  =  'SERVER'  1  =  'CLIENT'
const int PROGRAM_ROLE = 1;


// Test client
class ExamapleStdClient : public RFNET::StdClientIF
{
public:
  ExamapleStdClient(){}

  void dataRecvd(uint8_t length, uint8_t data[])
  {
      /*
          Only called back from a 'tick' in the agent. 
          This is primarily for the 'server' node (The thing that will process the cmd)
      */
      Serial.print("Got data [");
      Serial.print(length);
      Serial.print("]: ");
      Serial.print((char*)data);
  }

  void fault(RFNET::Faults fault)
  {
    switch(fault)
    {
      case RFNET::Faults::NRF24_INIT:
        Serial.println("FAULT :: INIT");
      break;
      case RFNET::Faults::NRF24_SET_CHAN:
        Serial.println("FAULT :: SET CHANNEL");
      break;
      case RFNET::Faults::NRF24_SET_RF:
        Serial.println("FAULT :: SET RF");
      break;
      case RFNET::Faults::NRF24_UNAVAILABLE:
        Serial.println("FAULT :: SET UNAVAILABLE");
      break;
      case RFNET::Faults::TIMEOUT_SEND:
        Serial.println("FAULT :: SEND TIMEOUT");
      break;
      case RFNET::Faults::TIMEOUT_RECV:
        Serial.println("FAULT :: RECV TIMEOUT - No response back from a send");
      break;
      default:
        Serial.println("FAULT :: UNKNOWN");
      break;
    }
  }
};

ExamapleStdClient  * stdClient;
RFNET::StdAgent    * stdAgent;

void setup()
{
  Serial.begin(9600);

  stdClient = new ExamapleStdClient();
  stdAgent  = new RFNET::StdAgent(stdClient,
                                  CE_PIN, CSN_PIN, 1, 
                                  RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm);

    if(0 == PROGRAM_ROLE)
    {
      Serial.println("Program Role: Server");
    }
    else
    {
      Serial.println("Program Role: Client");
    }
}

void loop() 
{
    // Server role ticks
    if(0 == PROGRAM_ROLE)
    {
        stdAgent->tick();
    }
    else
    {
    // Client role sends
        uint8_t testData[] = "1234567890ABCDEFGHIJKLMNOP";
        stdAgent->send(sizeof(testData), testData);
    }
}