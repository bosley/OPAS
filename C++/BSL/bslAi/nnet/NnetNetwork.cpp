#include "NnetNetwork.hpp"
#include <algorithm>
#include <exception>

namespace NNET 
{
    namespace {

        // Generate a random weight
        inline double generateRandomWeight() {
            return rand() / double(RAND_MAX);
        }
    }

    // -------------------------------------------------
    // Network
    // -------------------------------------------------
    Network::Network(UTIL::Logger &logger, double bias) : logger(logger), 
                                             constructionFinalized(false),
                                             bias(bias) {
        networkJson["bias"] = bias;
    }

    // -------------------------------------------------
    // Network
    // -------------------------------------------------
    Network::Network(UTIL::Logger &logger, json config) : logger(logger),
                                                          constructionFinalized(false) {

        std::vector< std::vector< Cell > > tempLayers ;

        config.at("bias").get_to(bias);

        config.at("layers").get_to(layers);

        networkJson["bias"] = bias;

        std::vector<CellToCell> tempC2c;
        config.at("cellConnections").get_to(tempC2c);

        for(auto &cc : tempC2c) {

            Cell* cellFrom = findCell(cc.idFrom);
            Cell* cellTo   = findCell(cc.idTo); 

            // Connect cellFrom to cellTo
            cellFrom->addOutboundCell(cellTo, cellFrom->getOuputWeightByCellId(cellTo->id));

            cellTo->addInboundCell(cellFrom);

            connectedCells.push_back({ cc.idFrom, cc.idTo });
        }

        finalizeConstruction();

        LOG_DEBUG(logger) << "Network built from json configuration\n";
    }

    // -------------------------------------------------
    // ~Network
    // -------------------------------------------------
    Network::~Network() {

        layers.clear();
    }

    // -------------------------------------------------
    // newLayer
    // -------------------------------------------------
    void Network::newLayer() {

        if(constructionFinalized) {
            LOG_WARN(logger) << "Construction was finalized - Not creating new layer\n";
            return;
        }

        layers.push_back(
            std::vector<Cell>()
        );

        LOG_DEBUG(logger) << "New layer created\n";
    }

    // -------------------------------------------------
    // newCell
    // -------------------------------------------------
    void Network::newCell(unsigned layer, unsigned cellId, Cell::Variant variant, TransferFunction transferFunc) {

        if(constructionFinalized) {
            LOG_WARN(logger) << "Construction was finalized - Not creating new cell\n";
            return;
        }

        if( variant == Cell::Variant::NONE ) {

            std::__throw_runtime_error("Cell variant must not be NONE");
        } 

        // Ensure input nodes only go into layer 0
        if( (variant == Cell::Variant::INPUT && layer != 0 || 
            (layer == 0 && variant != Cell::Variant::INPUT))) {

            std::__throw_runtime_error("Only INPUT nodes can be in layer 0");
        } 

        // Ensure output nodes only go into last layer
        if( (variant == Cell::Variant::OUTPUT && layer != layers.size()-1)) {

            std::__throw_runtime_error("OUTPUT nodes must be in the last error");
        }

        LOG_DEBUG(logger) << "Creating new cell with id : " << cellId << "\n";

        // Ensure layer exists
        if(layer > layers.size()) {
            std::string s = "Given layer out of bounds for cell creation : " + std::to_string(layer);
            std::__throw_runtime_error(s.c_str());
        }

        // Create a new cell 
        layers[layer].push_back(Cell(cellId, layer, variant, transferFunc));
    }

    // -------------------------------------------------
    // connectCells
    // -------------------------------------------------
    void Network::connectCells(unsigned cellIdFrom, unsigned cellIdTo) {

        if(constructionFinalized) {
            LOG_WARN(logger) << "Construction was finalized - Not creating new connection\n";
            return;
        }

        LOG_DEBUG(logger) << "Connecting cell [" << cellIdFrom << "] to cell [" << cellIdTo << "]\n";

        Cell* cellFrom = findCell(cellIdFrom);
        Cell* cellTo   = findCell(cellIdTo); 

        // Connect cellFrom to cellTo
        cellFrom->addOutboundCell(cellTo, {generateRandomWeight(), bias});

        cellTo->addInboundCell(cellFrom);

        connectedCells.push_back({ cellIdFrom, cellIdTo });
    }

    // -------------------------------------------------
    // finalizeConstruction
    // -------------------------------------------------
    void Network::finalizeConstruction() {

        LOG_DEBUG(logger) << "Marking network as finalized\n";
        constructionFinalized = true;

        networkJson["cellConnections"] = connectedCells;
    }

    // -------------------------------------------------
    // train
    // -------------------------------------------------
    void Network::train(TrainingData data, unsigned epochs, CorrectionMethod correctionMethod) {

        if(!constructionFinalized) {
            LOG_WARN(logger) << "Construction of network not finalized, aborting training sequence\n";
            return;
        }

        std::vector<TrainingData::Data> tdata = data.getData();
 
        for(unsigned i = 0; i < epochs; i++) {

            for(auto &segment : tdata) {

                feedForward(segment.input);

                switch(correctionMethod) {
                    case CorrectionMethod::BACK_PROPAGATION:  backPropagation(segment.expected); break;
                    default: 
                        backPropagation(segment.expected); 
                        break;
                }
            }
        }

    }

    // -------------------------------------------------
    // input
    // -------------------------------------------------
    void Network::input(const std::vector<double> &inputVals) {

        if(!constructionFinalized) {
            LOG_WARN(logger) << "Construction of network not finalized, aborting input sequence\n";
            return;
        }

        feedForward(inputVals);
    }

    // -------------------------------------------------
    // getResults
    // -------------------------------------------------
    std::vector<double> Network::getResults() const {

        std::vector<double> results;

        if(!constructionFinalized) {
            LOG_WARN(logger) << "Construction of network not finalized, can not get results\n";
            return results;
        }

        for(int n = 0; n < layers.back().size(); n++) {

           results.push_back(layers.back()[n].getOutputValue());
        }
        return results;
    }

    // -------------------------------------------------
    // feedForward
    // -------------------------------------------------
    void Network::feedForward(const std::vector<double> &input) {

        if(input.size() != layers[0].size()) {

            std::__throw_runtime_error("Input size is incorrect for given input nodes for the network");
            return;
        }

        for(unsigned i = 0; i < input.size(); i++) {

            layers[0][i].setOutputValue(input[i]);
        }

        // Propogate forward
        for(unsigned i = 1; i < layers.size(); i++) {

            for(unsigned n = 0; n < layers[i].size(); n++) {

                layers[i][n].feedForward();
            }
        }
    }

    // -------------------------------------------------
    // backpropagation
    // -------------------------------------------------
    void Network::backPropagation(const std::vector<double> &expected) {

        if(expected.size() != layers.back().size()) {

            std::__throw_runtime_error("Expected target size is incorrect for given output nodes for the network");
            return;
        }

        std::vector<Cell> &outputLayer = layers.back();

        for(int n = 0; n < outputLayer.size(); n++) {

            outputLayer[n].calculateOutputGradients(expected[n]);
        }

        for(int layer = layers.size()-2; layer > 0; --layer) {

            std::vector<Cell> &hidden = layers[layer];

            for(int n = 0; n < hidden.size(); ++n) {

                hidden[n].calculateHiddenGradients();
            }
        }

        for(int layer = layers.size()-1; layer > 0; --layer) {

            std::vector<Cell> &lay = layers[layer];

            for(int n = 0; n < lay.size(); ++n) {

                lay[n].updateInputWeights();
            }
        }
    }

    // -------------------------------------------------
    // findCell
    // -------------------------------------------------
    Cell * Network::findCell(unsigned id) {

        Cell * cell = nullptr;

        for(auto &layer : layers ) {

            // Find 'from' cell
            if(cell == nullptr) {

                for(auto &inner : layer) {
                    if(inner.id == id) {
                        cell = &inner;
                    }
                }
            }
        }

        if(cell == nullptr) {
            std::string s("Could not locate cell : " + std::to_string(id));
            std::__throw_runtime_error(s.c_str());
        }

        return cell;
    }

    // -------------------------------------------------
    // get_json
    // -------------------------------------------------
    json Network::get_json() {

        if(!this->constructionFinalized) {
            std::__throw_runtime_error("Network not finalized, can not generate JSON");
            return json();
        }

        networkJson["layers"] = layers;

        return networkJson;
    }

}