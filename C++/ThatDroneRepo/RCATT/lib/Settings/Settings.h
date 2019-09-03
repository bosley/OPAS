/*
    Defines used to change construction of controller, and some runtime settings
*/

// Required

#define SETTINGS_PROGRAM_DELAY_MS 1 // How long to delay at the end of main loop

#define ADDRESS_CONTROLLER 1        // The Datagram address for the Controller
#define ADDRESS_RECEIVER   2        // Thing that the controller is controlling

#define NRF24_CE_PIN        9       // CE Pin for the module
#define NRF24_CSN_PIN       10      // CSE Pin for the module
#define NRF24_SEND_DELAY_MS 5       // Cooldown time for transmission
#define NRF24_ACTIVE        1       // Enable disable communications

#define INDICATE_IDLE       8       // Digital Out 8 - Indicate idle
#define INDICATE_GOOD       7       // Digital Out 7 - Indicate good tx
#define INDICATE_BAD        6       // Digital Out 6 - Indicate bad tx

// Optional

//#define SETTINGS_USE_OLED    1     // Using OLED - High memory usage
#define SETTINGS_SERIAL_OUTPUT 1   // Spits information out via serial
