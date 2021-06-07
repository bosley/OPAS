//
// Created by bosley on 3/30/20.
//

#include "Client.hpp"

namespace ORBIT
{

    Client::Client() : hostPort("127.0.0.1", 4051) {

    }

    Client::Client(std::string userAgent, short port) : userAgent(userAgent), hostPort("127.0.0.1", port){

    }

    Client::Client(std::string userAgent) : userAgent(userAgent), hostPort("127.0.0.1", 4051) {

    }

    Client::Client(std::string userAgent, HostPort hostPort) : userAgent(userAgent), hostPort(hostPort) {

    }
/*
    json Client::retrieve(HostPort remote, std::string path) {

    }

    json Client::info(HostPort remote) {

    }

    json Client::push(HostPort remote, std::string path, json data) {

    }
    */
}