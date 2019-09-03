#include <Settings.h>
#include <Arduino.h>
#include <Controller.h>
#include <Joystick.h>

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#ifdef SETTINGS_USE_OLED
    #include <Oled.h>
#endif


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

RCATTController::RCATTController() : controllerStatus(Status::LOADING)
{
    
#ifdef NRF24_ACTIVE
  if (!RCATTmanager.init())
  {
    controllerStatus = Status::ERROR;
    #ifdef SETTINGS_SERIAL_OUTPUT
        Serial.println("init failed");
    #endif
  }
#endif

#ifdef SETTINGS_USE_OLED
    updateScreen = true;
#endif
}

void RCATTController::begin()
{
    // Do system checks to ensure ready
    controllerStatus = Status::OPERATIONAL;
}

void RCATTController::registerJoystick(RCATTJoystick *joystick)
{
    if(joystick == nullptr)
        return;

    joysticks.pushBack(joystick);
}

#ifdef SETTINGS_USE_OLED
void RCATTController::registerOled(Oled *oled)
{
    if(oled == nullptr)
        return;

    loled = oled;
}
#endif

void RCATTController::tick()
{
    // Poll devices

    // - Joysticks
    for(int i =0; i < joysticks.size; i++)
    {
        joysticks.retrieveDataByIndex(i)->pollJoystick();
    }

#ifdef SETTINGS_USE_OLED
    if(updateScreen)
    {
        switch(controllerStatus)
        {
            case Status::LOADING:
                loled->flip("Loading", "", "");
            break;
            case Status::WARNING:
                loled->flip("WARNING", "", "");
            break;
            case Status::ERROR:
                loled->flip("_ERROR_", "", "");
            break;
            case Status::OPERATIONAL:
                loled->flip("Operational", "", "");
            break;
            case Status::NO_RESP
                loled->flip("NO RESP", "", "");
            break;
        }
        updateScreen = false;
    }
#endif  
}

////// Callbacks //////

// - Joystick callback
void RCATTController::joystick_x(uint8_t address, uint8_t value)
{
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Joystick: ");
    Serial.print(address);
    Serial.print(". X: ");
    Serial.print(value);
    Serial.print("\n");
#endif

#ifdef NRF24_ACTIVE
    transmit(address, 0, value);
#endif
}

// - Joystick callback
void RCATTController::joystick_y(uint8_t address, uint8_t value)
{
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Joystick: ");
    Serial.print(address);
    Serial.print(". Y: ");
    Serial.print(value);
    Serial.print("\n");
#endif

#ifdef NRF24_ACTIVE
    transmit(address, 1, value);
#endif
}

// - Joystick callback
void RCATTController::joystick_sw(uint8_t address)
{
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.print("Joystick: ");
    Serial.print(address);
    Serial.print(". Switch Triggered. ");
    Serial.print("\n");
#endif

#ifdef NRF24_ACTIVE
    transmit(address, 2, 1);
#endif
}


#ifdef NRF24_ACTIVE
void RCATTController::transmit(uint8_t joystick, uint8_t control, uint8_t value)
{
    // Receiver buff
    uint8_t buff[RH_NRF24_MAX_MESSAGE_LEN];

    // Data to send
    uint8_t data[] = {joystick, control, value};

#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.println("Sending joystick value to receiver");
#endif

    if (RCATTmanager.sendtoWait(data, sizeof(data), ADDRESS_RECEIVER))
    {
        uint8_t from;   
        uint8_t len = sizeof(buff);
        if (RCATTmanager.recvfromAckTimeout(buff, &len, 2000, &from))
        {
#ifdef SETTINGS_SERIAL_OUTPUT
            Serial.print("Reply from : 0x");
            Serial.print(from, HEX);
            Serial.print(": ");
            Serial.println((char*)buff);
#endif
            controllerStatus = Status::OPERATIONAL;
        }   
        else
        {
#ifdef SETTINGS_SERIAL_OUTPUT
            Serial.println("No reply, is the receiver running?");
#endif
            controllerStatus = Status::NO_RESP;
        }
    }
    else
    {
#ifdef SETTINGS_SERIAL_OUTPUT
            Serial.println("sendtoWait failed");
#endif
            controllerStatus = Status::WARNING;
    }
    delay(NRF24_SEND_DELAY_MS);
}
#endif