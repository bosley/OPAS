#include <BS0.hpp>

#include <DHT.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Serial settings
#define SERIAL_OUT 1

// NODE Information
#define NODE_ID 0
#define TEMPERATURE_DEVICE_ID 1
#define HUMIDITY_DEVICE_ID    2
#define MOVEMENT_DEVICE_ID    3
#define LIGHT_DEVICE_ID       4

// RF settings
RF24 radio(9, 8);  // CE, CSN  (Hard wired on constructed nodes following the Base_Node diagram)

const byte receiver_address[6] = "00001";

// DHT settings
#define DHTPIN 7          // (Connection 13)
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

// Pir pin and value
#define PIR_INPUT 2       // (Connection 4)
int pirValue = LOW;

// Light sensor value for detecting change
int light_value = -1;

// Timer
unsigned long time_ms = 0;

// Transmit a data buffer
void transmitBuffer(BS0::Buffer &buffer) {

    radio.write(&buffer.data, sizeof(buffer.data));
}

void encode_and_send_device_info() {
    BS0::DeviceInfo dev;
    
    dev.node_id = NODE_ID;  // Node ID
    dev.num_devices = 4;    // Number of devices
    dev.device_list[0] = TEMPERATURE_DEVICE_ID; // Temp
    dev.device_list[1] = HUMIDITY_DEVICE_ID;    // Humidity
    dev.device_list[2] = MOVEMENT_DEVICE_ID;    // Movement
    dev.device_list[3] = LIGHT_DEVICE_ID;       // Light Sensor

    BS0::Buffer buffer = BS0::encodeDeviceInfo(dev);

    transmitBuffer(buffer);
}

void encode_and_send_light_sensor() {
  
   BS0::Buffer buffer = BS0::encodeLightSensor(NODE_ID,
                                               light_value);
   transmitBuffer(buffer);
}

void encode_and_send_temperature(float temp) {
  
   BS0::Buffer buffer = BS0::encodeTemperature(NODE_ID, 
                                               BS0::TemperatureType::FAHRENHEIT, 
                                               temp);
   transmitBuffer(buffer);
}

void encode_and_send_humidity(float humidity) {

   BS0::Buffer buffer = BS0::encodeHumidity(NODE_ID, humidity);
   transmitBuffer(buffer);
}

void encode_and_send_motion(bool active) {

   BS0::Buffer buffer = BS0::encodeMovement(NODE_ID, active);
   transmitBuffer(buffer);
}

void run_light_sensor() {
  
  int value = analogRead(A0);   // (Connection 23)

  if(SERIAL_OUT) {
      Serial.print("Analog reading = ");
      Serial.print(value);
    
      if (value < 10) {
        Serial.println(" - Dark");
      } else if (value < 200) {
        Serial.println(" - Dim");
      } else if (value < 500) {
        Serial.println(" - Light");
      } else if (value < 800) {
        Serial.println(" - Bright");
      } else {
        Serial.println(" - Very bright");
      }
   }

   if( value != light_value ) {

       light_value = value;

       encode_and_send_light_sensor();
   }
}

void run_dht11() {

    float hum  = dht.readHumidity();
    float temp = dht.convertCtoF(dht.readTemperature());
  
   if(SERIAL_OUT) {
      //Print temp and humidity values to serial monitor
      Serial.print("Humidity: ");
      Serial.print(hum);
      Serial.print(" %, Temp: ");
      Serial.print(temp);
      Serial.println(" F");
   }

   encode_and_send_temperature(temp);
   encode_and_send_humidity(hum);
}

void run_pir() {

  int val = digitalRead(PIR_INPUT);
  
  if (val == HIGH) {
    if (pirValue == LOW) {      
      if(SERIAL_OUT) {
        Serial.println("Motion detected!");
      }

      pirValue = HIGH;

      encode_and_send_motion(true);
    }
    
  } else {

    if (pirValue == HIGH){
      if(SERIAL_OUT) {
          Serial.println("Motion ended!");
      }

      pirValue = LOW;
      
      encode_and_send_motion(false);
    }
  }
}


void setup() {
  // put your setup code here, to run once:

  if(SERIAL_OUT) {
    Serial.begin(9600);
  }
  
  dht.begin();

  pinMode(PIR_INPUT, INPUT); 

  time_ms = millis();

  // start the radio
  radio.begin();
  
  // set the address
  radio.openWritingPipe(receiver_address);
  
  // set module as transmitter
  radio.stopListening();

  encode_and_send_device_info();
}

void loop() {

  // Run every 30 seconds
  if( ( millis() - time_ms) > 30000 ) {
    
      time_ms = millis();

      run_dht11();
  }

  run_light_sensor();
  
  run_pir();

  delay(100);
}