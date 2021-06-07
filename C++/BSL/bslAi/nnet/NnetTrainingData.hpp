#ifndef BSLAI_NNET_TRAINING_DATA_HPP
#define BSLAI_NNET_TRAINING_DATA_HPP

#include "bslcore/UtilLogger.hpp"

#include <vector>
#include <nlohmann/json.hpp>

namespace NNET
{
    //!
    //! \class TrainingData
    //! \brief Training data for neural network
    //!
    class TrainingData {
    
    public:

        //!
        //! \brief The parsed training data
        //!
        struct Data {
            std::vector<double> input;
            std::vector<double> expected;
        };

        //!
        //! \brief Constructor
        //!
        TrainingData(UTIL::Logger &logger, nlohmann::json data);

        //!
        //! \brief Get the training data
        //! \returns vector of Data
        //!
        std::vector<Data> getData() const;

        //!
        //! \brief Determine if parsing worked
        //! \retval true, data is valid and ready, otherwise false
        //!
        bool isReady() const;

    private:
        UTIL::Logger &logger;
        bool ready;
        std::vector<Data> trainingData;
    };
}

static std::ostream & operator<<(std::ostream &os, const NNET::TrainingData::Data& t) {

    std::string output = "Input [ ";
    for(auto &item : t.input) {
        output += std::to_string(item);
        output += " ";
    }
    output += "] | Epected [ ";

    for(auto &item : t.expected) {
        output += std::to_string(item);
        output += " ";
    }
    output += "]";

    return os << output;
}

#endif