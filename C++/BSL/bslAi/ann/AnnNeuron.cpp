#include "AnnNeuron.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>  

namespace {

    inline double sigmoid(double x) {
        return (1.0 / (1.0 + std::pow(ANN::Neuron::EULER, -x)));
    }
}

namespace ANN
{
    // -------------------------------------------------
    // Neuron
    // -------------------------------------------------
    Neuron::Neuron(int numOutputs, int index,
                   TransferFunction transfer,
                   double eta, double alpha) : 
                                                selectedTransferFunc(transfer),
                                                eta(eta),
                                                alpha(alpha) {
        srand (time(NULL));
        for(unsigned c = 0; c < numOutputs; ++c)
        {
            outputWeights.push_back(Connection());
            outputWeights.back().weight = randomWeight();
        }
        currIndex = index;
    }

    // -------------------------------------------------
    // Neuron
    // -------------------------------------------------
    Neuron::Neuron(json neuronData) {

        from_json(neuronData);
    }

    // -------------------------------------------------
    // setOutputVal
    // -------------------------------------------------
    void Neuron::setOutputVal(double val) {

        outputVal = val;
    }

    // -------------------------------------------------
    // getOutputVal
    // -------------------------------------------------
    double Neuron::getOutputVal() const {

        return outputVal;
    }

    // -------------------------------------------------
    // doFeedForward
    // -------------------------------------------------
    void Neuron::doFeedForward(const std::vector<Neuron> &prevLayer) {

        // output = f(Sigma i sub-i * w sub-i)
        double sum = 0.0;

        // Sum previous layers output
        for(unsigned n = 0; n  < prevLayer.size(); ++n)
        {
            sum += prevLayer[n].getOutputVal() * prevLayer[n].outputWeights[currIndex].weight;
        }

        // Transfer function
        outputVal = Neuron::transferFunction(sum, selectedTransferFunc);
    }

    // -------------------------------------------------
    // calcOutputGradients
    // -------------------------------------------------
    void Neuron::calcOutputGradients(double targetVal) {

        double delta = targetVal - outputVal;
        gradient = delta * Neuron::transferDerivative(outputVal, selectedTransferFunc);
    }

    // -------------------------------------------------
    // calcHiddenGradients
    // -------------------------------------------------
    void Neuron::calcHiddenGradients(const std::vector<Neuron> &nextLayer) {

        double dow = sumDOW(nextLayer);
        gradient = dow * Neuron::transferDerivative(outputVal, selectedTransferFunc);
    }

    // -------------------------------------------------
    // updateInputWeights
    // -------------------------------------------------
    void Neuron::updateInputWeights(std::vector<Neuron> &prevLayer) {

        for(unsigned n = 0; n < prevLayer.size(); ++n)
        {
            Neuron &neuron = prevLayer[n];
            double oldDelta = neuron.outputWeights[currIndex].deltaWeight;
            double newDelta = eta * neuron.getOutputVal() * gradient + alpha * oldDelta;

            neuron.outputWeights[currIndex].deltaWeight = newDelta;
            neuron.outputWeights[currIndex].weight += newDelta;
        }
    }

    // -------------------------------------------------
    // randomWeight
    // -------------------------------------------------
    double Neuron::randomWeight() {

        return rand() / double(RAND_MAX);
    }

    // -------------------------------------------------
    // sumDOW
    // -------------------------------------------------
    double Neuron::sumDOW(const std::vector<Neuron> &nextLayer) {

        double sum = 0.0;
        for(unsigned n = 0; n < nextLayer.size()-1; ++n)
        {
            sum += outputWeights[n].weight * nextLayer[n].gradient;
        }
        return sum;
    }

    // -------------------------------------------------
    // transferFunction
    // -------------------------------------------------
    double Neuron::transferFunction(double x, TransferFunction func) {

        switch(func) {
            case TransferFunction::TANH:    return tanh(x);
            case TransferFunction::SIGMOID: return sin(x);
        };

        // Satisfy the compiler
        return tanh(x);
    }

    // -------------------------------------------------
    // transferDerivative
    // -------------------------------------------------
    double Neuron::transferDerivative(double x, TransferFunction func) {

        switch(func) {
            case TransferFunction::TANH:    return (1.0 - x*x);
            case TransferFunction::SIGMOID: return sigmoid(x)*(1.0-sigmoid(x));
        };

        // Satisfy the compiler
        return (1.0 - x*x);
    }

    json Neuron::to_json() const {

        json jsonNeuron;
        jsonNeuron["eta"]       = eta;
        jsonNeuron["alpha"]     = alpha;
        jsonNeuron["outputVal"] = outputVal;
        jsonNeuron["gradient"]  = gradient;
        jsonNeuron["index"]     = currIndex;
        jsonNeuron["transferFunc"] = static_cast<int>(selectedTransferFunc);
        jsonNeuron["outputWeights"] = outputWeights;
        return jsonNeuron;
    }

    void Neuron::from_json(const json& j) {
        
        int stf = 0;
        j.at("eta").get_to(this->eta);
        j.at("alpha").get_to(this->alpha);
        j.at("outputVal").get_to(this->outputVal);
        j.at("gradient").get_to(this->gradient);
        j.at("index").get_to(this->currIndex);
        j.at("transferFunc").get_to(stf);
        j.at("outputWeights").get_to(this->outputWeights);

        this->selectedTransferFunc = static_cast<TransferFunction>(stf);
    }
}