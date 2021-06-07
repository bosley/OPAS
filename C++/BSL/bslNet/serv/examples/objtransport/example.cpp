#include <iostream>

#include <bslcore/UtilTimer.hpp>
#include "ServObjTransport.hpp"
#include "ServObjRemote.hpp"
#include "ServObjRecvrIf.hpp"
#include <random>
#include <thread>
#include <nlohmann/json.hpp>

/*
 *
 *      Transporter_1   ----> Remote ------> Transporter_2 -----> Destination_2
 *      Transporter_2   ----> Remote ------> Transporter_1 -----> Destination_1
 *
 * */

typedef nlohmann::json json;

namespace {

    // Destination 1 (server1)
    class Destination1 : public SERV::JRecvIf {
    public:
        virtual void recvMsg(SERV::ObjRemote from, json msg) override {

            std::cout << std::endl << "Destination1 got something from : "
                      << from.getName()
                      << "@(" << from.getAddress()
                      << ":"  << from.getPort()
                      << ") " << msg.dump(4) << std::endl;
        }
    };

    // Destination 2 (server2)
    class Destination2 : public SERV::JRecvIf {
    public:
        virtual void recvMsg(SERV::ObjRemote from, json msg) override {

            std::cout << std::endl << "Destination2 got something from : "
                    << from.getName()
                    << "@(" << from.getAddress()
                    << ":"  << from.getPort()
                    << ") " << msg.dump(4) << std::endl;
        }
    };

    // Borrowed from stack overflow
    std::string random_string(std::string::size_type length)
    {
        static auto& chrs = "0123456789"
                            "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        thread_local static std::mt19937 rg{std::random_device{}()};
        thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

        std::string s;

        s.reserve(length);

        while(length--)
            s += chrs[pick(rg)];

        return s;
    }
}

void runServer(SERV::ObjTransport & transporter) {

    UTIL::Timer timer(5*60);    // Run for 5 minutes
    UTIL::Timer sleeper(0.05);  // Sleep for some time between ticks. Just.. because. Not required.

    while(!timer.isExpired()) {

        transporter.tick();

        sleeper.reset();
        while(!sleeper.isExpired()){}
    }
}


int main()
{
    // Create 2 servers
    SERV::ObjTransport transporter1("transporter_1", "127.0.0.1", 4096);

    SERV::ObjTransport transporter2("transporter_2", "127.0.0.1", 4098);

    // Create objects that will receive data from the server
    Destination1 d1;
    Destination2 d2;

    // Assign a recvif to a string-name that MUST correspond to a remote name
    transporter1.addLocalReceiver("destination1", d1);      // Destination string isn't encoded into objTransport because
    transporter2.addLocalReceiver("destination2", d2);      // on the other end of things they won't have it setup

    // Create 'remotes' that point to a 'destination'
    SERV::ObjRemote remoteToDestination1("127.0.0.1", 4096, "destination1");
    SERV::ObjRemote remoteToDestination2("127.0.0.1", 4098, "destination2");

    // Connect server with their remotes
    transporter1.addRemote(remoteToDestination2);    // Connects server 1 to destination2
    transporter2.addRemote(remoteToDestination1);    // Connects server 2 to destination 1

    std::cout << "Starting threads.." << std::endl;

    // Start server threads to launch and tick the things
    std::thread transporter2Thread(runServer, std::ref(transporter2));
    std::thread transporter1Thread(runServer, std::ref(transporter1));

    std::cout << "Sending data.." << std::endl;

    // Run the example for 5 minutes
    UTIL::Timer timer(5*60);

    // Send data every 5 seconds
    UTIL::Timer sendTimer(5);

    int count = 1;
    while(!timer.isExpired()) {

        // If its time to send data
        if(sendTimer.isExpired())
        {
            // For output clairity
            std::cout << "-------------------------------------------------------" << std::endl;

            // Create some data
            json obj;
            obj["data_num"]    = count++;
            obj["rand_string"] = random_string(4096);

            if(count % 2 == 0)
            {
                // If even, send from transporter 1 to destination 2
                transporter1.send("destination2", obj);
            }
            else
            {
                // If odd, send from transporter 2 to destination 1
                transporter2.send("destination1", obj);
            }

            sendTimer.reset();
        }
    }

    // Join the server threads
    transporter1Thread.join();
    transporter2Thread.join();

    return 0;
}