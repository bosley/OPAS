
#include <map>
#include <iostream>

#include "CppUTest/TestHarness.h" 

#include "TSGPBusImpl.hpp"
#include "CBAddress.hpp"
#include "CBMessage.hpp"

namespace 
{
    struct TestVal
    {
        CB::Address to;
        CB::Address from;
        CB::Message::PRIORITY priority;
        CB::TYPES::bytes data;
    };

    CB::AddressList const initialWhiteList = {  CB::Address(0x01), 
                                                CB::Address(0x02), 
                                                CB::Address(0x03),
                                                CB::Address(0x04),
                                                CB::Address(0x05),
                                                CB::Address(0x06) };
}

TEST_GROUP(TSGPImplTest)
{
};

TEST(TSGPImplTest, TSGPImplTestSimple)
{
    std::cout << "Test: TSGPImplTest -> TSGPImplTestSimple" << std::endl;

    TSGP::BusImpl bus ( initialWhiteList );

    // Write messages
    for(TestVal const &val : { 
              TestVal { CB::Address(0x01), CB::Address(0x02), 
                        CB::Message::PRIORITY::LOW, {'x', 'y', 'i'} },
              TestVal { CB::Address(0x02), CB::Address(0x02), 
                        CB::Message::PRIORITY::HIGH, {'a', 'y', 'z'} },
              TestVal { CB::Address(0x03), CB::Address(0x02), 
                        CB::Message::PRIORITY::LOW, {'b', 'c', 'z'} },
              TestVal { CB::Address(0x01), CB::Address(0x02), 
                        CB::Message::PRIORITY::MEDIUM, {'h', 'd', 'e'} },
              TestVal { CB::Address(0x06), CB::Address(0x02), 
                        CB::Message::PRIORITY::EXTREME, {'g', 'f', 'z'} }
                             })
    {
        CHECK_EQUAL_TEXT(true, bus.writeBusMessage(val.to, 
                               CB::Message(val.from, val.priority, val.data) ), 
                               "Unable to write message to bus");
    }
}
