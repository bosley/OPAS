#ifndef BSLAI_ANN_TOPOLOGY
#define BSLAI_ANN_TOPOLOGY

#include <iostream>
#include <vector>
#include "AnnTypes.hpp"

#include "bslcore/UtilLogger.hpp"

namespace ANN 
{
    //!
    //! \class Topology
    //! \brief Create a Topology
    //!
    /// {
    ///     "input"  : 2,
    ///     "output" : 1,
    ///     "hidden" : [4,7,4,4,3]
    /// }
    class Topology {

    public: 
        //!
        //! \brief Construct a topology
        //! \param logger The logger to use for the topology class
        //! \param config Json config for topolgy
        //! \post  Topology will be marked ready and values will be populated
        //!
        Topology(UTIL::Logger &logger, json &config);

        //!
        //! \brief Check if topology data was built correctly
        //! \returns true if ready, false otherwise
        //!
        bool isReady() const;

        //!
        //! \brief Get the number of input nodes
        //! \returns Number of input nodes for the networks 
        //!
        int getInput() const;

        //!
        //! \brief Get the number of output nodes
        //! \returns Number of output nodes for the networks 
        //!
        int getOutput() const;

        //!
        //! \brief Get the numbers for each hidden layer
        //! \returns Vector containing size for each hidden layer. Vector.size() is number of hidden layers
        //!
        std::vector<int> getHidden() const;

        //!
        //! \brief Generate the json representation of this topology
        //!
        json to_json();

        friend bool operator == (const Topology &lhs, const Topology &rhs);

    private:
        UTIL::Logger &logger;
        bool ready;
        int  input;
        int  output;
        std::vector<int> hidden;
        void buildFromJson(nlohmann::json &config);
    };

    inline bool operator== (const Topology &lhs, const Topology &rhs) {
        return (
            lhs.input == rhs.input && 
            lhs.output == rhs.output &&
            lhs.hidden == rhs.hidden);
    }

    static std::ostream & operator<<(std::ostream &os, const ANN::Topology& t) {

        std::vector<int> hidden = t.getHidden();
        int  input  = t.getInput();
        int  output = t.getOutput();

        std::string hiddenString = "[ ";

        for(auto &item : hidden) {
            hiddenString += std::to_string(item);
            hiddenString += " ";
        }
        hiddenString += "]";

        return os << "Input nodes: " << input << " | Output nodes: " << output << " | Hidden nodes: " << hiddenString;
    }

}

#endif