#ifndef DRONE_SETTINGS_HEADER_GUARD
#define DRONE_SETTINGS_HEADER_GUARD

#define SETTINGS_PROGRAM_DELAY_MS 1 // How long to delay at the end of main loop

#define ADDRESS_CONTROLLER 1        // The Datagram address for the Controller
#define ADDRESS_RECEIVER   2        // Thing that the controller is controlling

/*
    Arduino Nano
*/
#define NRF24_CE_PIN        9       // CE Pin for the module
#define NRF24_CSN_PIN       10      // CSE Pin for the module

/*
    Arduino Mega

#define NRF24_CE_PIN        9       // CE Pin for the module
#define NRF24_CSN_PIN       53      // CSE Pin for the module
*/
#define NRF24_SEND_DELAY_MS 5       // Cooldown time for transmission
#define NRF24_ACTIVE        1       // Enable disable communications

#define SETTINGS_SERIAL_OUTPUT 1   // Spits information out via serial

#define LINKED_LIST_MAX     50      // Maximum linked list size

#endif