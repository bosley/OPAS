#ifndef BSLAI_NEURON_HPP
#define BSLAI_NEURON_HPP

#include "AnnTypes.hpp"

#include<cmath>
#include<vector>
#include<cstdlib>
#include<iostream>

namespace ANN
{

    //!
    //! \class Neuron
    //! \brief Create a neuron
    //!
    class Neuron {

    public:
        static constexpr double DEFAULT_ETA   = 0.15; //! Default ETA value
        static constexpr double DEFAULT_ALPHA = 0.50; //! Default ALPHA value
        static constexpr double EULER         = 2.71828182845904523536; //! Eulers number (e)

        //!
        //! \brief Create a neuron
        //! \param numOutputs Number of neuron outouts
        //! \param index Index of the neuron
        //! \param transfer Transfer function to use
        //! \param eta Overall net training weighr (Defaults to DEFAULT_ETA)
        //! \param alpha Multiplier for last weight change i.e Momentum (Defaults to DEFAULT_ALPHA)
        //!
        Neuron(int numOutputs, int index, TransferFunction transfer, double eta=DEFAULT_ETA, double alpha=DEFAULT_ALPHA);

        //!
        //! \brief Construct a neuron from a json representation
        //! \param neuronData The neuron data
        //! \post  The neuron will be setup as-dictated by the json input
        //!
        Neuron(json neuronData);

        //!
        //! \brief Set the output value of the neuron
        //! \param val The value to set for the neuron output
        //!
        void setOutputVal(double val);

        //!
        //! \brief Get neuron output value
        //! \returns value of the neuron
        //!
        double getOutputVal() const;

        //!
        //! \brief Perform the feed-forward operation
        //! \param prevLayer Previous layer of the network
        //!
        void doFeedForward(const std::vector<Neuron> &prevLayer);

        //!
        //! \brief Calculate the output gradients
        //! \param targetVal Target value
        //!
        void calcOutputGradients(double targetVal);

        //!
        //! \brief Calculate hidden gradients
        //! \param nextLayer The next layer to the network
        //!
        void calcHiddenGradients(const std::vector<Neuron> &nextLayer);

        //!
        //! \brief Update neuron input weights
        //! \param prevLayer Previous layer of the network
        //!
        void updateInputWeights(std::vector<Neuron> &prevLayer);

        //!
        //! \brief Convert Neuron to json
        //! \retval Json representation of neuron
        //!
        json to_json() const;

        //!
        //! \brief Build neuron from json
        //! \param j The json to build neuron from
        //!
        void from_json(const json& j);

        friend bool     operator == (const Neuron &lhs, const Neuron &rhs);
        friend std::ostream& operator << (std::ostream& os, const Neuron& neuron);

    private:
        TransferFunction selectedTransferFunc;
        double eta;
        double alpha;
        double outputVal;
        double gradient;
        unsigned currIndex;
        std::vector<Connection> outputWeights;

        // Get random [0 -> 1]
        double randomWeight();

        // Weight calculation
        double transferFunction(double x,  TransferFunction func);
        double transferDerivative(double x,TransferFunction func);
        double sumDOW(const std::vector<Neuron> &nextLayer);
    };

    inline bool operator== (const Neuron &lhs, const Neuron &rhs) {
        return (
            lhs.eta == rhs.eta &&
            lhs.alpha == rhs.alpha &&
            lhs.outputVal == rhs.outputVal &&
            lhs.gradient == rhs.gradient &&
            lhs.currIndex == rhs.currIndex &&
            lhs.outputWeights == rhs.outputWeights
        );
    }

    inline std::ostream& operator<<(std::ostream& os, const Neuron& neuron) {

        json jsonNeuron = neuron.to_json();
        std::string strNeuron = jsonNeuron.dump();
        os << strNeuron;
        return os;
    }
}

#endif