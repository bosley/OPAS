/*
    Includes
*/
#include <Settings.h>
#include <Arduino.h>
#include <Controller.h>
#include <Joystick.h>

#ifdef SETTINGS_USE_OLED
    #include <Oled.h>
#endif

/*
    Objects used in the controller
*/
RCATTController *controller;
RCATTJoystick   *joystick_left;
RCATTJoystick   *joystick_right;

#ifdef SETTINGS_USE_OLED
    Oled *screen;
#endif


void setup() 
{

#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.begin(9600);
#endif

    //
    //  Create the controller object
    //
    controller = new RCATTController();

#ifdef SETTINGS_USE_OLED
    //
    //  Create screen and register the oled
    //
    screen = new Oled();
    controller->registerOled(screen);
#endif

    //
    // Create left joystick and register the controller with it
    //
    joystick_left = new RCATTJoystick(0,             // Address
                                      0, 1, 2,       // X, Y, and SW pins
                                      0, 255, 95,    // X Range and rest
                                      0, 255, 100,   // Y Range and rest
                                      10,            // Tolerance    
                                      1              // SW Rest
                                    );
    joystick_left->registerController(controller);

    //
    // Create right joystick and register the controller with it
    //
    joystick_right = new RCATTJoystick(1,             // Address
                                       2, 3, 3,       // X, Y, and SW pins
                                       0, 255, 95,    // X Range and rest
                                       0, 255, 84,    // Y Range and rest
                                       10,            // Tolerance    
                                       1              // SW Rest
                                    );
    joystick_right->registerController(controller);

    //
    //  Register joysticks with controller
    //
    controller->registerJoystick(joystick_left);
    controller->registerJoystick(joystick_right);

    //
    //  Inform controller that setup is complete
    //
    controller->begin();
}

void loop() 
{
   // Tick the controller activity
   controller->tick();
   delay(SETTINGS_PROGRAM_DELAY_MS);
}