//
// Created by bosley on 3/27/20.
//

#include "Destination.hpp"

#include <iostream>

namespace APP
{
    Destination::Destination(std::string name) : name(name){

    }

    std::string Destination::getName() const {
        return name;
    }

    void Destination::recvMsg(SERV::ObjRemote from, json msg) {

            std::cout << std::endl << "---------------------------[RECEIVE]----------------------------" << std::endl;
            std::cout << std::endl << name << " got something from : "
                      << from.getName()
                      << "@(" << from.getAddress()
                      << ":"  << from.getPort()
                      << ") " << msg.dump(4) << std::endl;
            std::cout << "---------------------------[END RECV]---------------------------" << std::endl;
    }
}