#ifndef NEURON_H
#define NEURON_H

#include<cmath>
#include<vector>
#include<cstdlib>
#include<iostream>

// Weights of connections, and how much they will change
struct connection
{
    double weight;
    double deltaWeight;
};
typedef struct connection netConn;

class Neuron
{
public:
    Neuron(unsigned numOutputs, unsigned index);
    void setOutputVal(double val){
        outputVal = val;
    }
    double getOutputVal(void) const {
        return outputVal;
    }
    void doFeedForward(const std::vector<Neuron> &prevLayer);

    void calcOutputGradients(double targetVal);
    void calcHiddenGradients(const std::vector<Neuron> &nextLayer);
    void updateInputWeights(std::vector<Neuron> &prevLayer);

private:

    double eta; // [0.0 ... 1.0] - Overall net train rate
    double alpha; // [0.0 .. n]  - Multiplier of last weight change(momentum)


    double outputVal;
    unsigned currIndex;
    double gradient;
    std::vector<netConn> outputWeights;

    // Get random [0 -> 1]
    static double randomWeight(void){
        return rand() / double(RAND_MAX);
    }

    double sumDOW(const std::vector<Neuron> &nextLayer);

    static double transferFunction(double x);
    static double transferDerivative(double x);

};

#endif // NEURON_H
