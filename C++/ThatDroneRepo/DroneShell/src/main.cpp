#include <Arduino.h>
#include <Settings.h>
#include <Wireless.h>
#include <VehicleIf.h>

// #include <GVTank/Tank.h>  // For tank vehicle implementation
#include <AVQuad/QuadCopter.h> // For quad copter vehicle implementation


//
//  The vehicle implementation
//
VEHICLE::VehicleIf *vehicle;

//
//  A wireless receiver communication module
//
#ifdef NRF24_ACTIVE
  COMM::WirelessTXRX * comm;  // Wireless communication module
#endif

void setup() 
{
  //
  // Define Which Vehicle implementation to use
  //
  //  To change implementation #include the implementation 
  //  and set the vehicle variable to that implementation.
  //    - Each implementation is responsible for setting up
  //      the board in the constructor
  //
  // ------------------------------------------------------------
  
  //vehicle = new VEHICLE::Tank();  // A tank 
  vehicle = new VEHICLE::QuadCopter();  // A copter 
  
  // ------------------------------------------------------------

  // Setup serial
#ifdef SETTINGS_SERIAL_OUTPUT
    Serial.begin(9600);
#endif

  // Create the wireless module
#ifdef NRF24_ACTIVE
  comm = new COMM::WirelessTXRX(vehicle);
#endif
}

void loop() 
{
#ifdef NRF24_ACTIVE
  comm->tick();   // If anything comes off the wire, will populate in command list
#endif

  // Step the vehicle execution 
  vehicle->tick();

  delay(1000);
}