#include "AnnNetwork.hpp"

namespace ANN 
{
    // -------------------------------------------------
    // Network
    // -------------------------------------------------
    Network::Network(UTIL::Logger &logger, Topology &topology, TransferFunction transferFunc, double forcedBias) : 
                                                                                                logger(logger), 
                                                                                                topology(&topology),
                                                                                                selectedTransferFunc(transferFunc),
                                                                                                bias(forcedBias),
                                                                                                builtFromConfig(false) {
        buildTopology();
    }

    // -------------------------------------------------
    // Network
    // -------------------------------------------------
    Network::Network(UTIL::Logger &logger, json config) : logger(logger),
                                                          topology(nullptr) {

        topology = new Topology(logger, config["topology"]);
        
        builtFromConfig = true;

        if(!topology->isReady()) {
            LOG_WARN(logger) << "Configuration for topology given to network failed to produce working topology object!\n";
            exit(1);
        }

        selectedTransferFunc = static_cast<ANN::TransferFunction>(config["transferFunc"]);

        bias = config["bias"];

        // Construct the layers
        for(auto &outer : config["layers"]) {

            std::vector<Neuron> layer;
            for(auto &inner: outer) {

                layer.push_back(Neuron(inner));
            }

            layers.push_back(layer);
        }
    }

    // -------------------------------------------------
    // ~Network
    // -------------------------------------------------
    Network::~Network() {

        // Only if the network is config built will it own the space for topology
        if(builtFromConfig) {
            delete topology;
        }
    }

    // -------------------------------------------------
    // input
    // -------------------------------------------------
    void Network::input(const std::vector<double> &inputVals) {

        doFeedForward(inputVals);
    }

    // -------------------------------------------------
    // buildTopology
    // -------------------------------------------------
    void Network::buildTopology() {

        LOG_DEBUG(logger) << "Building topology\n";

        if(topology == nullptr) {
            LOG_WARN(logger) << "Somehow topology is a nullptr.. this shouldn't have happened.\n";
            return;
        }

        int inputLayersCnt  = topology->getInput();
        int outputLayersCnt = topology->getOutput();
        std::vector<int> hiddenLayers = topology->getHidden();
        int hiddenLayersCnt = hiddenLayers.size();

        // Figure out the number of layers
        int numLayers = inputLayersCnt + outputLayersCnt + hiddenLayersCnt;

        // --------------------- Input Layer ----------------------

        // Create input layer
        layers.push_back(std::vector<Neuron>());

        // Number of neurons in first hidden layer
        int firstHiddenLayerCount = hiddenLayers[0];

        // Populate input layer with neurons
        for(int i = 0; i < inputLayersCnt; i++) {

            layers.back().push_back(
                Neuron(firstHiddenLayerCount, i, selectedTransferFunc)
                );

            // Force bias node output
            layers.back().back().setOutputVal(bias);
        }

        // -------------------- Hidden Layers --------------------
        
        for(int i = 0; i < hiddenLayersCnt; i++){

            // Create the layer
            layers.push_back(std::vector<Neuron>());

            // Get the next layer's number of nodes
            // If its the last hidden layer, use the output layer, otherwise,
            // use the number of the next hidden layer
            int numOutputs = (i == hiddenLayersCnt-1) ?  outputLayersCnt : hiddenLayers[i+1];

            // For each neuron in the current layer
            for(int j = 0; j <= hiddenLayers[i]; j++) {

                layers.back().push_back(
                    Neuron(numOutputs, j, selectedTransferFunc)
                    );
            }

            // Force bias node output
            layers.back().back().setOutputVal(bias);
        }

        // -------------------- Output Layer --------------------

        layers.push_back(std::vector<Neuron>());

        // Populate output layer with neurons
        for(int i = 0; i < outputLayersCnt; i++) {

            layers.back().push_back(
                Neuron(0, i, selectedTransferFunc)
                );

            // Force bias node output
            layers.back().back().setOutputVal(bias);
        }

    }

    // -------------------------------------------------
    // doFeedForward
    // -------------------------------------------------
    void Network::doFeedForward(const std::vector<double> &inputVals) {

        // Make sure we aren't adding something silly
        assert(inputVals.size() == layers[0].size()-1);

        // Latch input values to input neurons
        for(unsigned i = 0; i < inputVals.size(); i++) {

            layers[0][i].setOutputVal(inputVals[i]);
        }

        // Propogate forward
        for(unsigned i = 1; i < layers.size(); i++) {

            // Ref to last layer
            std::vector<Neuron> &prevLayer = layers[i -1];
            for(unsigned n = 0; n < layers[i].size()-1; n++) {

                // Feed last layer [Calls Neuron's doFeedForward]
                layers[i][n].doFeedForward(prevLayer);
            }
        }
    }

    // -------------------------------------------------
    // doBackPropagation
    // -------------------------------------------------
    void Network::doBackPropagation(const std::vector<double> &targetVals) {

        // Calculate overall net err
        std::vector<Neuron> &outputLayer = layers.back();
        error = 0.0;

        for(int n = 0; n < outputLayer.size()-1; ++n) {

            double delta = targetVals[n] - outputLayer[n].getOutputVal();
            error += delta * delta;
        }

        error /= outputLayer.size()-1;
        error = sqrt(error);

        // Calculate out layer gradients
        for(int n=0; n < outputLayer.size()-1; ++n) {

            outputLayer[n].calcOutputGradients(targetVals[n]);
        }

        // Calculate gradients on hidden layers
        for(int layer = layers.size()-2; layer > 0; --layer) {

            std::vector<Neuron> &hidden = layers[layer];
            std::vector<Neuron> &nLayer = layers[layer+1];

            for(int n = 0; n < hidden.size(); ++n) {

                hidden[n].calcHiddenGradients(nLayer);
            }
        }

        // For all layers from out to first update conn weights
        for(int layer = layers.size()-1; layer > 0; --layer) {

            std::vector<Neuron> &lay = layers[layer];
            std::vector<Neuron> &pLay = layers[layer-1];

            for(int n = 0; n < lay.size()-1; ++n) {

                lay[n].updateInputWeights(pLay);
            }
        }
    }

    // -------------------------------------------------
    // getResults
    // -------------------------------------------------
    std::vector<double> Network::getResults() const {

        std::vector<double> resultVals;
        for(int n = 0; n < layers.back().size()-1; ++n) {

            resultVals.push_back(layers.back()[n].getOutputVal());
        }
        return resultVals;
    }

    // -------------------------------------------------
    // train
    // -------------------------------------------------
    void Network::train(TrainingData data, int iterations) {

        std::vector<TrainingData::Data> tdata = data.getData();

        for(int i = 0; i < iterations; i++) {

            for(auto &segment : tdata) {

                doFeedForward(segment.input);
                doBackPropagation(segment.expected);

                LOG_DEBUG(logger) << segment << "\n";

                for(auto &result : getResults()) {

                    LOG_DEBUG(logger) << "Result: [" << result << "]\n";
                }
            }
        }
    }

    // -------------------------------------------------
    // to json
    // -------------------------------------------------
    json Network::to_json() {
        json build;

        if(topology == nullptr) {
            LOG_WARN(logger) << "Somehow topology managed to be a nullptr.\n";
            return build;
        }

        build["topology"] = topology->to_json();
        build["transferFunc"] = static_cast<int>(selectedTransferFunc);
        build["bias"] = bias;

        std::vector<std::vector<json>> jsonLayers;

        for(auto &outer : layers) {

            std::vector<json> jsonInner;
            for(auto &inner: outer) {

                jsonInner.push_back(inner.to_json());
            }
            jsonLayers.push_back(jsonInner);
        }

        build["layers"] = jsonLayers;

        return build;
    }
}