//  Basic tests of the BS0.hpp header
//  This is just to ensure that the basic encode and decode are working correctly
//
//

#include <iostream>
#include <math.h> 
#include "BS0.hpp"

bool float_match(float lhs, float rhs) {

    return !(fabs(lhs - rhs) > 0.1);
}

void test_device_list() {

    BS0::DeviceInfo dev_info;
    dev_info.node_id = 200;
    dev_info.num_devices = 27;
    for(int i = 0; i < 27; i++) {
        dev_info.device_list[i] = 100 + i;
    }

    BS0::Buffer buffer = BS0::encodeDeviceInfo(dev_info);

    BS0::DeviceInfo out_dev_info;

    if(!BS0::decodeDeviceInfo(buffer, out_dev_info)) {
        std::cout << "ERROR: Unable to decode DeviceInfo" << std::endl;
        exit(1);
    }
    
    if(dev_info.node_id != out_dev_info.node_id) {
        std::cout << "ERROR: Node id mismatch in decode DeviceInfo" << std::endl;
        exit(1);
    }

    if(dev_info.num_devices != out_dev_info.num_devices) {
        std::cout << "ERROR: Num devices mismatch in decode DeviceInfo" << std::endl;
        exit(1);
    }

    for(int i = 0; i < 27; i++) {
        if((100+i) != out_dev_info.device_list[i]) {
            std::cout << "ERROR: Device ID did not match expected value for number : " << i << std::endl;
            std::cout << "\tExpected " << 100+i << ", got " << out_dev_info.device_list[i] << std::endl;
            exit(1);
        }
    }

    std::cout << "PASS" << std::endl;
}

void test_temperature() {

    for(uint8_t node_id = 0; node_id < 4; node_id++) {

        BS0::Buffer buffer = BS0::encodeTemperature(node_id, static_cast<BS0::TemperatureType>(node_id), 101.1 + node_id, 3);
    
        uint8_t out_id = 200;
        BS0::TemperatureType out_temp = BS0::TemperatureType::UNKNOWN;
        float val = 0.00;
        uint8_t sub_id = 0;
    
        if(!BS0::decodeTemperature(buffer, out_id, out_temp, val, sub_id)) {
            std::cout << "ERROR: Failed to decode temperature" << std::endl;
            exit(1);
        }

        if(out_id != node_id) {
            std::cout << "ERROR: Node id mismatch" << std::endl;
            exit(1);
        }

        if(sub_id != 3) {
            std::cout << "ERROR: Sub id mismatch" << std::endl;
            exit(1);
        }

        if(!float_match((101.1 + node_id), val)) {
            std::cout << "ERROR: Temperature values do not match" << std::endl;
            std::cout << "Got " << val << ", expected " << 101.1 + node_id << std::endl;
            exit(1);
        }

        if(out_temp != static_cast<BS0::TemperatureType>(node_id)) {
            std::cout << "ERROR: Temperature types to not match" << std::endl;
            std::cout << "Got " << (int)out_temp << ", expected " << (int)node_id << std::endl;
            exit(1);
        }
    }

    std::cout << "PASS" << std::endl;
}


void test_humidity() {

    for(uint8_t node_id = 0; node_id < 4; node_id++) {

        BS0::Buffer buffer = BS0::encodeHumidity(node_id, 101.1 + node_id);
    
        uint8_t out_id = 200;
        float val = 0.00;
        uint8_t sub_id = 0;
    
        if(!BS0::decodeHumidity(buffer, out_id, val, sub_id)) {
            std::cout << "ERROR: Failed to decode humidity" << std::endl;
            exit(1);
        }

        if(out_id != node_id) {
            std::cout << "ERROR: Node id mismatch" << std::endl;
            exit(1);
        }

        if(!float_match((101.1 + node_id), val)) {
            std::cout << "ERROR: Temperature values do not match" << std::endl;
            std::cout << "Got " << val << ", expected " << 101.1 + node_id << std::endl;
            exit(1);
        }
    }

    std::cout << "PASS" << std::endl;
}

void test_movement() {

    for(uint8_t node_id = 0; node_id < 10; node_id++) {

        BS0::Buffer buffer = BS0::encodeMovement(node_id, (node_id % 2 == 0), 99);
    
        uint8_t out_id = 200;
        uint8_t sub_id = 200;
        bool val;
    
        if(!BS0::decodeMovement(buffer, out_id, val, sub_id)) {
            std::cout << "ERROR: Failed to decode movement" << std::endl;
            exit(1);
        }

        if(out_id != node_id) {
            std::cout << "ERROR: Node id mismatch" << std::endl;
            exit(1);
        }

        if(sub_id != 99) {
            std::cout << "ERROR: Sub id mismatch" << std::endl;
            exit(1);
        }

        if(val != (node_id % 2 == 0) ) {
            std::cout << "ERROR: Incorrect boolean : Expected " << (node_id % 2 == 0) << std::endl;
            exit(1);
        }
    }

    std::cout << "PASS" << std::endl;
}

void test_button() {

    for(uint8_t node_id = 0; node_id < 10; node_id++) {

        BS0::Buffer buffer = BS0::encodeButton(node_id, (node_id % 2 == 0), 99);
    
        uint8_t out_id = 200;
        uint8_t sub_id = 200;
        bool val;
    
        if(!BS0::decodeButton(buffer, out_id, val, sub_id)) {
            std::cout << "ERROR: Failed to decode button" << std::endl;
            exit(1);
        }

        if(out_id != node_id) {
            std::cout << "ERROR: Node id mismatch" << std::endl;
            exit(1);
        }

        if(sub_id != 99) {
            std::cout << "ERROR: Sub id mismatch" << std::endl;
            exit(1);
        }

        if(val != (node_id % 2 == 0) ) {
            std::cout << "ERROR: Incorrect boolean : Expected " << (node_id % 2 == 0) << std::endl;
            exit(1);
        }
    }

    std::cout << "PASS" << std::endl;
}


void test_light_sensor() {

    for(uint8_t node_id = 0; node_id < 10; node_id++) {

        BS0::Buffer buffer = BS0::encodeLightSensor(node_id, 200+node_id, 99);
    
        uint8_t out_id = 200;
        uint8_t sub_id = 200;
        uint16_t val;
    
        if(!BS0::decodeLightSensor(buffer, out_id, val, sub_id)) {
            std::cout << "ERROR: Failed to decode light sensor" << std::endl;
            exit(1);
        }

        if(out_id != node_id) {
            std::cout << "ERROR: Node id mismatch" << std::endl;
            exit(1);
        }

        if(sub_id != 99) {
            std::cout << "ERROR: Sub id mismatch" << std::endl;
            exit(1);
        }

        if(val != 200+node_id ) {
            std::cout << "ERROR: Light Sensor values mismatch " << std::endl;
            std::cout << "Expected " << 200+node_id << ", got " << val << std::endl; 
            exit(1);
        }
    }

    std::cout << "PASS" << std::endl;
}
int main() {

    std::cout << "test_device_list  : ";
    test_device_list();

    std::cout << "test_temperature  : ";
    test_temperature();

    std::cout << "test_humidity     : ";
    test_humidity();

    std::cout << "test_movement     : ";
    test_movement();

    std::cout << "test_button       : ";
    test_button();

    std::cout << "test_light_sensor : ";
    test_light_sensor();
}