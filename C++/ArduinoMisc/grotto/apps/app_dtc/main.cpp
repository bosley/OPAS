#include "Arduino.h"
#include <DtcServer.hpp>

DTC::Server * server = nullptr;

void setup() {
    
	Serial.begin(9600);

}

void loop() {

	if( nullptr == server) {
		server = new DTC::Server();
	}

	Serial.println("DTC On");
    delay(1000);
}