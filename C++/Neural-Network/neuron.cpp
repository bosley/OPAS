#include "neuron.h"


Neuron::Neuron(unsigned numOutputs, unsigned index)
{
    eta = 0.15;
    alpha = 0.5;
    for(unsigned c = 0; c < numOutputs; ++c)
    {
        outputWeights.push_back(netConn());
        outputWeights.back().weight = randomWeight();
    }
    currIndex = index;
}

void Neuron::setOutputVal(double val)
{
    outputVal = val;
}

double Neuron::getOutputVal() const
{
    return outputVal;
}

void Neuron::doFeedForward(const std::vector<Neuron> &prevLayer)
{
    // output = f(Sigma i sub-i * w sub-i)
    double sum = 0.0;

    // Sum previous layers output
    for(unsigned n = 0; n  < prevLayer.size(); ++n)
    {
        sum += prevLayer[n].getOutputVal() * prevLayer[n].outputWeights[currIndex].weight;
    }

    // Transfer function
    outputVal = Neuron::transferFunction(sum);
}

void Neuron::calcOutputGradients(double targetVal)
{
    double delta = targetVal - outputVal;
    gradient = delta * Neuron::transferDerivative(outputVal);
}

void Neuron::calcHiddenGradients(const std::vector<Neuron> &nextLayer)
{
    double dow = sumDOW(nextLayer);
    gradient = dow * Neuron::transferDerivative(outputVal);
}

void Neuron::updateInputWeights(std::vector<Neuron> &prevLayer)
{
    for(unsigned n = 0; n < prevLayer.size(); ++n)
    {
        Neuron &neuron = prevLayer[n];
        double oldDelta = neuron.outputWeights[currIndex].deltaWeight;
        double newDelta = eta * neuron.getOutputVal() * gradient + alpha * oldDelta;

        neuron.outputWeights[currIndex].deltaWeight = newDelta;
        neuron.outputWeights[currIndex].weight += newDelta;
    }
}

double Neuron::randomWeight()
{
    return rand() / double(RAND_MAX);
}

double Neuron::sumDOW(const std::vector<Neuron> &nextLayer)
{
    double sum = 0.0;
    for(unsigned n = 0; n < nextLayer.size()-1; ++n)
    {
        sum += outputWeights[n].weight * nextLayer[n].gradient;
    }
    return sum;
}

double Neuron::transferFunction(double x)
{
    // Could use step, ramp, sigmoid, exponential, or tanh
    // Here using tanh
    return tanh(x);
}

double Neuron::transferDerivative(double x)
{
    // Estimation of tanh derivative
    return (1.0 - x*x);
}
