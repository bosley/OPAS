#include <Settings.h>
#include <Arduino.h>
#include <Controller.h>
#include <Joystick.h>
#include <EnDecode.h>

#include <Wireless.h>

#ifdef SETTINGS_USE_OLED
    #include <Oled.h>
#endif

namespace
{
    void led_all_off()
    {
        digitalWrite(INDICATE_IDLE, LOW);
        digitalWrite(INDICATE_GOOD, LOW);
        digitalWrite(INDICATE_BAD,  LOW);
    }

    void led_all_on()
    {
        digitalWrite(INDICATE_IDLE, HIGH);
        digitalWrite(INDICATE_GOOD, HIGH);
        digitalWrite(INDICATE_BAD,  HIGH);
    }

    void indicate_bad()
    {
        digitalWrite(INDICATE_IDLE, LOW);
        digitalWrite(INDICATE_GOOD, LOW);
        digitalWrite(INDICATE_BAD,  HIGH);
    }

    void indicate_good()
    {
        digitalWrite(INDICATE_IDLE, LOW);
        digitalWrite(INDICATE_GOOD, HIGH);
        digitalWrite(INDICATE_BAD,  LOW);
    }

    void indicate_idle()
    {
        digitalWrite(INDICATE_IDLE, HIGH);
        digitalWrite(INDICATE_GOOD, LOW);
        digitalWrite(INDICATE_BAD,  LOW);
    }
} 


RCATTController::RCATTController() : time_index(millis()),
                                     controllerStatus(Status::LOADING)
{
    led_all_on();

#ifdef NRF24_ACTIVE
  if (!NET::doInit())
  {
    indicate_bad();
    controllerStatus = Status::ERROR;
    #ifdef SETTINGS_SERIAL_OUTPUT
        Serial.println("init failed");
    #endif
  }
#endif

#ifdef SETTINGS_USE_OLED
    updateScreen = true;
#endif

    indicate_idle();
}

void RCATTController::begin()
{
    controllerStatus = Status::IDLE;
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
    controllerStatus = Status::IDLE;
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

    if( (millis() - time_index) > 1000 )
    { 
        switch(controllerStatus)
        {
            case Status::WARNING:
                indicate_bad();
            break;
            case Status::ERROR:
                led_all_on();
            break;
            case Status::OPERATIONAL:
                indicate_good();
            break;
            case Status::NO_RESP:
                indicate_bad();
            break;
            case Status::LOADING:
            case Status::IDLE:
                indicate_idle();
            break;
        }
        time_index = millis();
    }
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
    // Encode the data
    NET::wBytes encoded = NET::encode({joystick, control, value});

    // Data to send
    uint8_t data[] = {encoded.b0, encoded.b1};

    if(NET::writeBytes(data, sizeof(data), ADDRESS_RECEIVER))
    {
        controllerStatus = Status::OPERATIONAL;
    }
    else
    {
        controllerStatus = Status::NO_RESP;
    }
    //delay(NRF24_SEND_DELAY_MS);
}
#endif