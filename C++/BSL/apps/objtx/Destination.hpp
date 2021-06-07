//
// Created by bosley on 3/27/20.
//

#ifndef BSL_APP_DESTINATION_HPP
#define BSL_APP_DESTINATION_HPP

#include <bslnet/ServObjRecvrIf.hpp>

#include <string>

namespace  APP
{
    // A 'destination' object
    class Destination : public SERV::JRecvIf {
    public:
        Destination(std::string destination_name);
        std::string getName() const;
        virtual void recvMsg(SERV::ObjRemote from, json msg) override;

    private:
        std::string name;
    };
}

#endif //BSL_APP_DESTINATION_HPP
