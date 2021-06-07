//
// Created by bosley on 3/30/20.
//

#include "Request.hpp"

namespace ORBIT
{
    Request::Request() : requestType(Type::INFO) {

    }

    Request::Request(Agent agent, Type requestType, json data) : agent(agent),
                                                                 requestType(requestType),
                                                                 dataField(data) {

    }


    Agent Request::getAgent() const {

        return agent;
    }

    Request::Type  Request::getType() const {

        return requestType;
    }

    std::string  Request::getTypeString() const {

        switch (requestType) {
            case Type::PUSH:     return(std::string("PUSH"));
            case Type::INFO:     return(std::string("INFO"));
            case Type::RETRIEVE: return(std::string("RETRIEVE"));
        }

        // Because the compiler =/
        return std::string("UNKNOWN REQUEST TYPE");
    }

    json Request::getData() const {

        return dataField;
    }
}