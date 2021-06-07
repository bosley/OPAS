//
// Created by bosley on 3/30/20.
//

#ifndef ORBIT_CLIENT_HPP
#define ORBIT_CLIENT_HPP

#include <string>

#include <nlohmann/json.hpp>

typedef nlohmann::json json;

#include "Agent.hpp"
#include "HostPort.hpp"

#warning Client needs to actually be written

namespace ORBIT
{
    class Client {
    public:

        Client();
        Client(std::string userAgent);
        Client(std::string userAgent, short port);
        Client(std::string userAgent, HostPort hostPort);

        /*
        json retrieve(HostPort remote, std::string path);

        json info(HostPort remote);

        json push(HostPort remote, std::string path, json data);
        */
    private:

        Agent userAgent;
        HostPort hostPort;
    };
}


#endif //ORBIT_CLIENT_HPP
