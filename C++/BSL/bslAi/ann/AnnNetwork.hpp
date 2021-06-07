#ifndef BSLAI_NETWORK_HPP
#define BSLAI_NETWORK_HPP

#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <fstream>
#include <iostream>

#include "AnnTopology.hpp"
#include "AnnTrainingData.hpp"
#include "AnnNeuron.hpp"
#include "bslcore/UtilLogger.hpp"

namespace ANN
{
    //!
    //! \class Network
    //! \brief Create a neural network
    //!
    class Network {

    public:
        //!
        //! \brief Construct a neural network
        //! \param logger Logger to use in the network
        //! \param topology Topology to use for the network
        //! \param transfer Transfer function to use (Defaults to TransferFunction::TANH)
        //! \param forcedBias Bias to force on the neurons upon construction
        //!
        Network(UTIL::Logger &logger, Topology &topology, TransferFunction transferFunction=TransferFunction::TANH, double forcedBias=0.01);

        //!
        //! \brief Construct a neural network from a json config
        //! \param logger Logger to use in the network
        //! \param jsonNetwork The json values for the network
        //! \note  Only data generated from a hand-build network should be stuffed here
        //!
        Network(UTIL::Logger &logger, json jsonNetwork);

        //!
        //! \brief Destruct the network
        //!
        ~Network();

        //!
        //! \brief Input data
        //! \param inputVals Input values being fed into network
        //!
        void input(const std::vector<double> &inputVals);

        //!
        //! \brief Get results
        //! \returns Resulting values
        //!
        std::vector<double> getResults() const;

        //!
        //! \brief Train the network with some data
        //! \param data The data to train the network with
        //! \param iterations How many times to run the data
        //! \note  If the training data doesn't match topology, the behavior is undefined
        //!
        void train(TrainingData data, int iterations);

        //!
        //! \brief Build json representation of the network
        //! \retval json information representing the network
        //!
        json to_json();

        friend bool     operator == (const Network &lhs, const Network &rhs);

    private:
        UTIL::Logger &logger;
        Topology     *topology;
        TransferFunction selectedTransferFunc;
        double       error;
        double       bias;
        std::vector<std::vector<Neuron>> layers; 
        bool builtFromConfig;

        void doFeedForward(const std::vector<double> &inputVals);
        void doBackPropagation(const std::vector<double> &targetVals);
        void buildTopology();
    };

    inline bool operator== (const Network &lhs, const Network &rhs) {
        return (
            lhs.topology != nullptr &&
            rhs.topology != nullptr &&
            *lhs.topology == *rhs.topology &&
            lhs.selectedTransferFunc == rhs.selectedTransferFunc &&
            lhs.layers == rhs.layers
        );
    }
}

#endif // BSLAI_NETWORK_HPP
