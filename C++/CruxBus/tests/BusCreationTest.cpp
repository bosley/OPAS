#include <iostream>

#include "CppUTest/TestHarness.h"

#include "CBAddress.hpp"
#include "CBBus.hpp"
#include "CBMessage.hpp"

TEST_GROUP(BusCreationTest)
{
};

TEST(BusCreationTest, BusTestSimple)
{
    std::cout << "Test: BusCreationTest -> BusTestSimple" << std::endl;

    //
    //  Create a white list
    //
    CB::AddressList whiteList = { CB::Address(0x0A), 
                                  CB::Address(0x0B), 
                                  CB::Address(0x0C), 
                                  CB::Address(0x0D) };
    //
    //  Create a bus
    //
    CB::Bus cb(whiteList);

    //
    //  Check adding of addresses to bus
    //
    CHECK_EQUAL_TEXT( false, cb.addNewAddress(CB::Address(0x0A)), 
                      "Bus overwrote existing CB::Address");
    CHECK_EQUAL_TEXT( true, cb.addNewAddress(CB::Address(0xFF)), 
                      "Bus failed to addNewAddress()");
    CHECK_EQUAL_TEXT( true, cb.isExistingAddress(CB::Address(0xFF)),
                      "Bus indicated that new, existing item is not present");
    //
    // Check removing of address from bus
    //
    CHECK_EQUAL_TEXT( false, cb.removeExistingAddress(CB::Address(0xAA)),
                      "Bus indicated that non-existing item was removed");
    CHECK_EQUAL_TEXT( true, cb.removeExistingAddress(CB::Address(0xFF)),
                      "Bus indicated that existing item was unable to be removed");
    CHECK_EQUAL_TEXT( false, cb.isExistingAddress(CB::Address(0xFF)),
                      "Bus indicated that item exists post deletion");

    //
    // Check messages
    //
    CHECK_EQUAL_TEXT( false, cb.containsMessages(), "Bus wrongly indicated message existence");

    // Message information
    CB::Address testTo(0x0A);
    CB::Address testFrom(0xB1);
    CB::TYPES::bytes data = {'x', 'y', 'z'};
    
    // Set message
    CB::Message testMsg0(testFrom, CB::Message::PRIORITY::LOW, data);
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg0), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.containsMessages(), "Bus should contain a message");

    // Read message
    CB::Message recvMsg;
    CHECK_EQUAL_TEXT( true, cb.readMessage(testTo, recvMsg), "Unable to read message from bus"); 
    CHECK_EQUAL_TEXT( true, data == recvMsg.data, "Data from read message not correct");
    CHECK_EQUAL_TEXT( true, cb.containsMessages(), "Bus should contain a message");

    // Get message
    recvMsg = CB::Message();
    CHECK_EQUAL_TEXT( true, cb.getMessage(testTo, recvMsg), "Unable to read message from bus"); 
    CHECK_EQUAL_TEXT( true, data == recvMsg.data, "Data from read message not correct");
    CHECK_EQUAL_TEXT( false, cb.containsMessages(), "Bus wrongly indicated message existence");

    // =========================================

    // Change address
    testTo = CB::Address(0x0B);

    CB::TYPES::bytes d1 = {'e','f','g','h'};
    CB::TYPES::bytes d2 = {'i','j','k','l'};
    CB::TYPES::bytes d3 = {'m','n','o','p'};
    CB::TYPES::bytes d4 = {'q','r','s','t', 'u', 'v', 'w', 'x', 'y', 'z'};
    CB::TYPES::bytes d5 = {'a','b','c','d'};

    CB::Message testMsg1(testFrom, CB::Message::PRIORITY::NONE,     d1);
    CB::Message testMsg2(testFrom, CB::Message::PRIORITY::HIGH,     d2);
    CB::Message testMsg3(testFrom, CB::Message::PRIORITY::EXTREME,  d3);
    CB::Message testMsg4(testFrom, CB::Message::PRIORITY::MEDIUM,   d4);
    CB::Message testMsg5(testFrom, CB::Message::PRIORITY::LOW,      d5);

    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg1), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg2), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg3), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg4), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg5), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.containsMessages(), "Bus should contain a message");

    CB::Message recvMsg1;
    CB::Message recvMsg2;
    CB::Message recvMsg3;
    CB::Message recvMsg4;
    CB::Message recvMsg5;
    
    CHECK_EQUAL_TEXT( true, cb.getMessage(testTo, recvMsg1), "Unable to read message from bus"); 
    CHECK_EQUAL_TEXT( true, d1 == recvMsg1.data, "Data from read message not correct");
    CHECK_EQUAL_TEXT( true, cb.getMessage(testTo, recvMsg2), "Unable to read message from bus"); 
    CHECK_EQUAL_TEXT( true, d2 == recvMsg2.data, "Data from read message not correct");
    CHECK_EQUAL_TEXT( true, cb.getMessage(testTo, recvMsg3), "Unable to read message from bus"); 
    CHECK_EQUAL_TEXT( true, d3 == recvMsg3.data, "Data from read message not correct");
    CHECK_EQUAL_TEXT( true, cb.getMessage(testTo, recvMsg4), "Unable to read message from bus"); 
    CHECK_EQUAL_TEXT( true, d4 == recvMsg4.data, "Data from read message not correct");
    CHECK_EQUAL_TEXT( true, cb.getMessage(testTo, recvMsg5), "Unable to read message from bus"); 
    CHECK_EQUAL_TEXT( true, d5 == recvMsg5.data, "Data from read message not correct");

    CHECK_EQUAL_TEXT( false, cb.containsMessages(), "Bus wrongly indicated message existence");

    // =========================================

    CB::MessageList recvMsgList;
    CB::MessageList expected = {testMsg1, testMsg2, testMsg3, testMsg4, testMsg5 };

    // Insert messages
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg1), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg2), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg3), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg4), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.writeMessage(testTo, testMsg5), "Bus failed to write message");
    CHECK_EQUAL_TEXT( true, cb.containsMessages(), "Bus should contain a message");

    // Read messages
    CHECK_EQUAL_TEXT( true, cb.readAllMessages(testTo, recvMsgList), "Unable to read message from bus"); 

    // Check expectations
    for(const auto i : expected)
    {
        auto j = std::find(recvMsgList.begin(), recvMsgList.end(), i);
        if(j == recvMsgList.end())
        {
            FAIL("Unable to locate retrieved item from message list");
        }
    }

    // Ensure messages still there
    CHECK_EQUAL_TEXT( true, cb.containsMessages(), "Bus should contain a message");

    // Get all messages
    CB::MessageList recvMsgList1;
    CHECK_EQUAL_TEXT( true, cb.getAllMessages(testTo, recvMsgList1), "Unable to read message from bus"); 

    // Check expectations
    for(const auto i : expected)
    {
        auto j = std::find(recvMsgList.begin(), recvMsgList.end(), i);
        if(j == recvMsgList.end())
        {
            FAIL("Unable to locate retrieved item from message list");
        }
    }

    // Ensure messages are gone
    CHECK_EQUAL_TEXT( false, cb.containsMessages(), "Bus wrongly indicated message existence");
}
