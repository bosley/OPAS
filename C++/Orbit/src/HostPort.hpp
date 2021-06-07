//
// Created by bosley on 3/28/20.
//

#ifndef ORBIT_HOSTPORT_HPP
#define ORBIT_HOSTPORT_HPP

#include <string>

namespace ORBIT
{
    class HostPort {
    public:
        HostPort(std::string host, short port);

        std::string getHost() const;

        short getPort() const;

    private:
        std::string host;
        short       port;
    };
}

#endif //ORBIT_HOSTPORT_HPP
