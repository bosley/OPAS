#include "network.h"

Network::Network()
{
    debug = false;
}

void Network::setDebug(const bool state)
{
    debug = state;
}

void Network::setTopology(const std::vector<unsigned> topology)
{
    unsigned numLayers = topology.size();
    for(unsigned i = 0; i < numLayers; ++i)
    {
        // Add a new layer
        n_layers.push_back(Layer());

        // If output layer, set 0 : else, set
        unsigned numOutputs = (i == topology.size()-1) ?
                    0 : topology[i+1];

        // Add neurons as-well-as bias neuron to layer
        for(unsigned n = 0; n <= topology[i]; ++n)
        {
            n_layers.back().push_back(Neuron(numOutputs, n));

            if(debug)
                std::cout << "Neuron Created.." << std::endl;
        }

        // Force bias node output
        n_layers.back().back().setOutputVal(1.0);
    }
}

void Network::doFeedForward(const std::vector<double> &inputVals)
{
    // Make sure we aren't adding something silly
    assert(inputVals.size() == n_layers[0].size()-1);

    // Latch input values to input neurons
    for(unsigned i = 0; i < inputVals.size(); i++)
    {
        n_layers[0][i].setOutputVal(inputVals[i]);
    }

    // Propogate forward
    for(unsigned i = 1; i < n_layers.size(); ++i)
    {
        // Ref to last layer
        Layer &prevLayer = n_layers[i -1];
        for(unsigned n = 0; n < n_layers[i].size()-1; ++n)
        {
            // Feed last layer [Calls Neuron's doFeedForward]
            n_layers[i][n].doFeedForward(prevLayer);
        }
    }
}

void Network::doBackPropagation(const std::vector<double> &targetVals)
{
    // Calculate overall net err
    Layer &outputLayer = n_layers.back();
    n_err = 0.0;

    for(unsigned n = 0; n < outputLayer.size()-1; ++n)
    {
        double delta = targetVals[n] - outputLayer[n].getOutputVal();
        n_err += delta * delta;
    }
    n_err /= outputLayer.size()-1;
    n_err = sqrt(n_err);

    // Calculate out layer gradients
    for(unsigned n=0; n < outputLayer.size()-1; ++n)
    {
        outputLayer[n].calcOutputGradients(targetVals[n]);
    }

    // Calculate gradients on hidden layers
    for(unsigned layer = n_layers.size()-2; layer > 0; --layer)
    {
        Layer &hidden = n_layers[layer];
        Layer &nLayer = n_layers[layer+1];

        for(unsigned n = 0; n < hidden.size(); ++n)
        {
            hidden[n].calcHiddenGradients(nLayer);
        }
    }

    // For all layers from out to first update conn weights
    for(unsigned layer = n_layers.size()-1; layer > 0; --layer)
    {
        Layer &lay = n_layers[layer];
        Layer &pLay = n_layers[layer-1];

        for(unsigned n = 0; n < lay.size()-1; ++n)
        {
            lay[n].updateInputWeights(pLay);
        }
    }

}

void Network::getResults(std::vector<double> &resultVals) const
{
    resultVals.clear();
    for(unsigned n = 0; n < n_layers.back().size()-1; ++n)
    {
        resultVals.push_back(n_layers.back()[n].getOutputVal());
    }
}
