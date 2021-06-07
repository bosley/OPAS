//
// Created by bosley on 3/28/20.
//

#include "HostPort.hpp"

namespace ORBIT
{
    HostPort::HostPort(std::string host, short port) : host(host), port(port) {

    }

    std::string HostPort::getHost() const {

        return host;
    }

    short HostPort::getPort() const {

        return port;
    }
}