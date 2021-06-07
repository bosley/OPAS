//
// Created by bosley on 3/30/20.
//

#include "Agent.hpp"

#include "version.hpp"

namespace ORBIT
{
    Agent::Agent() : name("Anonymous"),
                     version(ORBIT_VERSION) {

    }

    Agent::Agent(std::string agentName) : name(agentName),
                                            version(ORBIT_VERSION){

    }

    std::string Agent::getName() const {

        return name;
    }

    std::string Agent::getVersion() const {

        return version;
    }
}