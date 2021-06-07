//
// Created by bosley on 3/30/20.
//

#ifndef ORBIT_AGENT_HPP
#define ORBIT_AGENT_HPP

#include <string>
#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace ORBIT
{
    class Agent {
    public:

        Agent();
        Agent(std::string agentName);

        std::string getName() const;
        std::string getVersion() const;

        friend void to_json(json &j, const   Agent &s);
        friend void from_json(const json &j, Agent &r);

    private:
        std::string name;
        std::string version;
    };

    //json serialization
    inline void to_json(json &j, const Agent &a)
    {
        j["name"]     = a.name;
        j["version"]  = a.version;
    }

    inline void from_json(const json &j, Agent &a)
    {
        a.name = j.at("name").get<std::string>();
        a.version = j.at("name").get<std::string>();
    }
}


#endif //ORBIT_AGENT_HPP
