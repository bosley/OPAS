#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>

#include <vector>
#include <cassert>

#include "neuron.h"

// Layer vector structure
typedef std::vector<Neuron> Layer;

class Network
{
public:
    Network();
    void setDebug(const bool state);
    void setTopology(const std::vector<unsigned> topology);
    void doFeedForward(const std::vector<double> &inputVals);
    void doBackPropagation(const std::vector<double> &targetVals);
    void getResults( std::vector<double> &resultVals) const;

private:
    bool debug;
    double n_err;
    std::vector<Layer> n_layers; // [Layer][neuron]
};

#endif // NETWORK_H
