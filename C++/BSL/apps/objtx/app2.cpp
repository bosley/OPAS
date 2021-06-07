//
// Created by bosley on 3/27/20.
//

#include <bslcore/UtilTimer.hpp>
#include <bslnet/ServObjTransport.hpp>
#include <bslnet/ServObjRemote.hpp>
#include <bslnet/ServObjRecvrIf.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <thread>

#include "Helpers.hpp"
#include "Destination.hpp"

namespace {
    // Run a server in a thread
    void runServer(SERV::ObjTransport & transporter) {

        UTIL::Timer timer(5*60);    // Run for 5 minutes
        UTIL::Timer sleeper(0.05);  // Sleep for some time between ticks. Just.. because. Not required.

        while(!timer.isExpired()) {

            transporter.tick();

            sleeper.reset();
            while(!sleeper.isExpired()){}
        }
    }
}

int main() {

    SERV::ObjTransport application("application_2", "127.0.0.1", 8092);

    // Create some objects that can receive information from the object server
    APP::Destination destination1("app_2_destination_1");
    APP::Destination destination2("app_2_destination_2");

    // 'Mount' them
    application.addLocalReceiver(destination1.getName(), destination1);
    application.addLocalReceiver(destination2.getName(), destination2);

    // Add some remotes that tell application1 about places that it can reach
    application.addRemote(
            SERV::ObjRemote(
                    "127.0.0.1",
                    4096,
                    "app_1_destination_1"));


    application.addRemote(
            SERV::ObjRemote(
                    "127.0.0.1",
                    4096,
                    "app_1_destination_2"));

    // Start-up a thread that runs the server
    std::thread transporterThread(runServer, std::ref(application));

    // Run the app for 5 minutes
    UTIL::Timer timer(5*60);

    // Send data every 5 seconds
    UTIL::Timer sendTimer(5);

    int count = 1;
    while(!timer.isExpired()) {

        if (sendTimer.isExpired()) {

            std::cout << std::endl << "-----------------------------[SEND]-----------------------------" << std::endl;

            // Create some data
            json obj;
            obj["data_num"] = count++;
            obj["rand_string"] = APP::HELPERS::generate_random_string(25);

            if (count % 2 == 0) {
                std::cout << "Application2 --> Application1.destination1" << std::endl;

                // If even, send to app 2 destination 1
                application.send("app_1_destination_1", obj);

            } else  {
                std::cout << "Application2 --> Application2.destination2" << std::endl;

                // If odd, send to app 2 destination 2
                application.send("app_1_destination_2", obj);
            }

            std::cout << "---------------------------[END SEND]---------------------------" << std::endl;

            sendTimer.reset();
        }
    }

    transporterThread.join();
    return 0;
}