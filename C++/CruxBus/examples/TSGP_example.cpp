#include <iostream>
#include <thread>        
#include <chrono>

#include "TSGPBusImpl.hpp"
#include "TSGPReceiver.hpp"

#include "CBAddress.hpp"
#include "CBMessage.hpp"

#define EXAMPLE_IS_THREADED 1 // Change example from threaded to no threaded
#define MAIN_THREAD_SLEEP 2   // Sleep post message send, pre join
#define NUM_MSGS_TO_SEND 5    // Each ping will trigger pong, expect this * 2 total messages

// Ping and pong data
CB::TYPES::bytes PING  = {'p', 'i', 'n', 'g'};
CB::TYPES::bytes PING0 = {'p', 'i', 'n', 'g', '0'};
CB::TYPES::bytes PONG  = {'p', 'o', 'n', 'g'};

// Example object to show receiver and send functionality
class ExampleObject: public TSGP::Receiver
{
public:
    //
    //  Create an example object, giving a copy of busImpl to write to / etc
    //
    ExampleObject(CB::Address addr, TSGP::BusImpl *busImpl) : localAddr(addr),
                                                              busImpl(busImpl)
    {
    }

    ~ExampleObject(){}

    // 
    //  From Receiver - Receives message from bus when they're available
    //
    void msgsAvailable(CB::MessageList list) override
    {
        if(busImpl == nullptr)
        {
            return;
        }

        std::cout << "Example Object (" << localAddr.getAddress() << ") has [" << list.size() << "] message(s) available." << std::endl;
    
        //
        //  Write a message back to each incomming message
        //
        for(CB::Message &msg : list)
        {
            std::cout << "From: " << msg.from.getAddress()  
                      << ". Data: ";

            for(const auto &d : msg.data)
            {
                std::cout << d << " ";
            }
            std::cout << std::endl;

            if(msg.data != PONG)
            {
                std::cout << " >>>> " << msg.from.getAddress() << " is sending back pong!" << std::endl;
                busImpl->writeBusMessage(msg.from, 
                                        CB::Message(localAddr.getAddress(),
                                        CB::Message::PRIORITY::HIGH, 
                                        PONG)
                                        );
            }

            std::cout << "-------------------------------" << std::endl;
        }
    }

    // Object address
    CB::Address localAddr;

    private:
        TSGP::BusImpl *busImpl;
};

// Start the bus in a thread from main
void threadedExample(TSGP::BusImpl *bus)
{
    std::cout << "Threaded Example" << std::endl;

    bus->start();
}

// Call the bus from main similarly to the threaded example... but without its own thread :( 
void nonThreadedExample(TSGP::BusImpl *bus)
{
    std::cout << "Non-Threaded Example" << std::endl;

    bus->start();
}

// Send the initial pings from each object to the other
void sendMessages(TSGP::BusImpl *bus, ExampleObject &eObj, ExampleObject &eObj0)
{
    /*
        Send messages from one object to another
    */
    std::cout << "Main loop : writing messages from [" 
                << eObj.localAddr.getAddress() << "] to [" 
                << eObj0.localAddr.getAddress() << "]"
                << std::endl;

    for(int i = 0; i < NUM_MSGS_TO_SEND; i++)
    {
        bus->writeBusMessage(eObj.localAddr, 
                             CB::Message(eObj0.localAddr,
                             CB::Message::PRIORITY::HIGH,
                             PING)
                             );

    }

    /*
        Send messages from the other object to the first
    */
    std::cout << "Main loop : writing messages from [" 
                << eObj0.localAddr.getAddress() << "] to [" 
                << eObj.localAddr.getAddress() << "]"
                << std::endl;

    for(int i = 0; i < NUM_MSGS_TO_SEND; i++)
    {
        bus->writeBusMessage(eObj0.localAddr, 
                             CB::Message(eObj.localAddr,
                             CB::Message::PRIORITY::HIGH,
                             PING0)
                             );

    }
}

// Example entry
int main(int ac, char **av)
{
    // Create the bus
    TSGP::BusImpl *cruxBus;
    cruxBus = new TSGP::BusImpl (CB::AddressList{ 0x0A, 0x0B, 0x0C, 0x0D });

    // Create user objects
    ExampleObject eObj  (CB::Address(0x0A), cruxBus);    
    ExampleObject eObj0 (CB::Address(0x0B), cruxBus);    

    // Register the msgsAvailable callback
    cruxBus->registerRecvCallback(eObj.localAddr, &eObj);
    cruxBus->registerRecvCallback(eObj0.localAddr, &eObj0);

    // Start in thread
    if(EXAMPLE_IS_THREADED)
    {
        std::thread theThread (threadedExample,cruxBus);

        sendMessages(cruxBus, eObj, eObj0);

        std::this_thread::sleep_for (std::chrono::seconds(MAIN_THREAD_SLEEP));

        //
        //  STOP THE BUS!!!
        //
        cruxBus->stop();

        // Join
        theThread.join();
    }

    // Don't start in thread
    else
    {
        std::cout << "Non Threaded Example - Use CTRL+C to stop" << std::endl;
        std::cout << "Starting delay = 5 seconds.. " << std::endl;

        std::this_thread::sleep_for (std::chrono::seconds(5));

        sendMessages(cruxBus, eObj, eObj0);
        
        nonThreadedExample(cruxBus);

        cruxBus->stop(); // This won't be reached, but placed to be consistent
    }
    
    // Only thread end will trigger this final cout and cleanup
    std::cout << "Done" << std::endl;

    // Cleanup
    delete cruxBus;
    return 0;
}
