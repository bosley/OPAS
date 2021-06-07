#include "AnnTopology.hpp"

namespace ANN
{
    // -------------------------------------------------
    // Topology
    // -------------------------------------------------
    Topology::Topology(UTIL::Logger &logger, json &config) : 
                                                            logger(logger),
                                                            ready(false) {

        buildFromJson(config);
    }

    // -------------------------------------------------
    // isReady
    // -------------------------------------------------
    bool Topology::isReady() const {
        return ready;
    }

    // -------------------------------------------------
    // getInput
    // -------------------------------------------------
    int Topology::getInput() const {

        return input;
    }

    // -------------------------------------------------
    // getOutput
    // -------------------------------------------------
    int Topology::getOutput() const {

        return input;
    }

    // -------------------------------------------------
    // getHidden
    // -------------------------------------------------
    std::vector<int> Topology::getHidden() const {

        return hidden;
    }

    // -------------------------------------------------
    // buildFromJson
    // -------------------------------------------------
    void Topology::buildFromJson(nlohmann::json &config) {

        if(!config.contains("input")  || 
           !config.contains("output") || 
           !config.contains("hidden") ) {

               LOG_WARN(logger) << "Invalid json configuration for topology\n";
               ready = false;
           }

        try {
            
            input  = config["input"].get<int>();
            output = config["output"].get<int>();
            hidden = config["hidden"].get<std::vector<int>>();

        } catch (nlohmann::json::exception& e) {

            LOG_WARN(logger) << "Exception parsing json : " << e.what()
                             << "\nID: " << e.id << "\n";
            return;
        }

        LOG_DEBUG(logger) << *this << "\n";

        ready = true;
    }

    // -------------------------------------------------
    // to json
    // -------------------------------------------------
    json Topology::to_json() {
        json jsonTopology;

        jsonTopology["input"]  = input;
        jsonTopology["output"] = output;
        jsonTopology["hidden"] = hidden;

        return jsonTopology;
    }
}