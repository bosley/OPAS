//
// Created by bosley on 3/23/20.
//

#include "ServObjRemote.hpp"

namespace SERV
{
    // ---------------------------------------------------------
    // ObjRemote
    // ---------------------------------------------------------

    ObjRemote::ObjRemote() : address("127.0.0.1"), name("default"), port(4096) {

    }

    // ---------------------------------------------------------
    // ~ObjRemote
    // ---------------------------------------------------------

    ObjRemote::ObjRemote(std::string address, uint16_t port, std::string name) :
                                                            address(address),   // Transporter Address
                                                            name(name),         // Destination name
                                                            port(static_cast<int>(port)){ // Transporter port

    }

    // ---------------------------------------------------------
    // getAddress
    // ---------------------------------------------------------

    std::string ObjRemote::getAddress() const {

        return address;
    };

    // ---------------------------------------------------------
    // getName
    // ---------------------------------------------------------

    std::string ObjRemote::getName() const {

        return name;
    };

    // ---------------------------------------------------------
    // getPort
    // ---------------------------------------------------------

    int ObjRemote::getPort() const {

        return port;
    };

    // ---------------------------------------------------------
    // asJson
    // ---------------------------------------------------------

    json ObjRemote::asJson() {
        json obj;
        obj["name"]     = name;
        obj["address"]  = address;
        obj["port"]     = port;
        return obj;
    }

}