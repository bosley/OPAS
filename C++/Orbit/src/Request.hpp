//
// Created by bosley on 3/30/20.
//

#ifndef ORBIT_REQUEST_HPP
#define ORBIT_REQUEST_HPP

#include "Agent.hpp"

#include <stdint.h>

#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace ORBIT
{
    class Request {
    public:
        enum class Type
        {
            INFO      = 0x00,
            PUSH      = 0x01,
            RETRIEVE  = 0x02
        };

        Request();
        Request(Agent agent, Type requestType, json data = json());

        Agent getAgent() const;
        Request::Type  getType() const;
        std::string getTypeString() const;
        json getData() const;

        friend void to_json(json &j, const Request &s);
        friend void from_json(const json &j, Request &r);

    private:
        Agent           agent;
        Request::Type   requestType;
        json            dataField;
    };

    //json serialization
    inline void to_json(json &j, const Request &r)
    {
        j["agent"] = r.agent;
        j["type"]  = static_cast<uint8_t>(r.requestType);
        j["data"]  = r.dataField;
    }

    inline void from_json(const json &j, Request &r)
    {
        r.agent = j.at("agent").get<Agent>();
        r.requestType = static_cast<Request::Type>(j.at("type").get<uint8_t>());
        r.dataField = j.at("data");
    }

}

#endif //ORBIT_REQUEST_HPP
