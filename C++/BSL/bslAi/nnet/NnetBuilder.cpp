#include "NnetBuilder.hpp"

namespace NNET
{

    Builder::Builder(UTIL::Logger &logger) : logger(logger) {

    }
    
    // -------------------------------------------------
    // buildDeepFeedForward
    // -------------------------------------------------
    Network Builder::buildDeepFeedForward(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes, TransferFunction transferFunc) {
        
        return fullyConnected(NetworkType::DFF, inputNodes, innerLayers, outputNodes, transferFunc);
    }

    // -------------------------------------------------
    // buildRecurrent
    // -------------------------------------------------
    Network Builder::buildRecurrent(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes, TransferFunction transferFunc) {

        return fullyConnected(NetworkType::RECURRENT, inputNodes, innerLayers, outputNodes, transferFunc);
    }

    // -------------------------------------------------
    // buildLstm
    // -------------------------------------------------
    Network Builder::buildLstm(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes) {

        return fullyConnected(NetworkType::LSTM, inputNodes, innerLayers, outputNodes);
    }

    // -------------------------------------------------
    // buildGatedLstm
    // -------------------------------------------------
    Network Builder::buildGatedLstm(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes) {

        return fullyConnected(NetworkType::GATED_LSTM, inputNodes, innerLayers, outputNodes);
    }

    // -------------------------------------------------
    // buildRecurrent
    // -------------------------------------------------
    Network Builder::fullyConnected(NetworkType type, unsigned inputNodes, unsigned innerLayers, unsigned outputNodes, TransferFunction transferFunc) {
        
        assert(inputNodes  > 0);
        assert(innerLayers > 0);
        assert(outputNodes > 0);


        Network network(logger);

        unsigned totalLayers = innerLayers + 2;
        
        for(unsigned i = 0; i < totalLayers; i++) {
            network.newLayer();
        }

        std::vector<unsigned> inputIds;
        std::vector< std::vector<unsigned> > hiddenIds;

        std::vector<unsigned> outputIds;

        unsigned cellCount = 0;

        // Build input layer
        for(unsigned i = 0; i < inputNodes; i++) {
            inputIds.push_back(cellCount);
            network.newCell(0, cellCount++, Cell::Variant::INPUT, transferFunc);
        }

        // Build hidden layers
        unsigned hiddenNodesPerLayer = inputNodes + 1;

        for(unsigned layer = 1; layer < totalLayers -1 ; layer++ ) {

            std::vector<unsigned> layerIds;

            for(unsigned i = 0; i < hiddenNodesPerLayer; i++) {

                layerIds.push_back(cellCount);

                if(type == NetworkType::RECURRENT) {

                    network.newCell(layer, cellCount++, Cell::Variant::RECURRENT, transferFunc);

                } else if (type == NetworkType::LSTM) {

                    network.newCell(layer, cellCount++, Cell::Variant::MEMORY, transferFunc);

                } else if (type == NetworkType::GATED_LSTM) {

                    network.newCell(layer, cellCount++, Cell::Variant::GATED_MEMORY, transferFunc);

                } else {

                    network.newCell(layer, cellCount++, Cell::Variant::HIDDEN, transferFunc);
                }
            }

            hiddenIds.push_back(layerIds);
        }

        // Build output layer
        for(unsigned i = 0; i < outputNodes; i++) {
            outputIds.push_back(cellCount);
            network.newCell(totalLayers-1, cellCount++, Cell::Variant::OUTPUT, transferFunc);
        }

        // Connect cells input to hidden 
        for(auto & id : inputIds) {

            for(auto & hidden : hiddenIds[0]) {

                network.connectCells(id, hidden);
            }
        }

        // Conenct all hidden cells
        for(int i = 0; i < hiddenIds.size()-1; i++) {

            std::vector<unsigned> &currentLayer = hiddenIds[i];
            std::vector<unsigned> &nextLayer = hiddenIds[i+1];

            for(auto &currentLayerId : currentLayer) {

                for(auto &nextLayerId : nextLayer) {

                    network.connectCells(currentLayerId, nextLayerId);
                }
            }
        }

        // Connect last hidden layer to output nodes
        for(int i = 0; i < hiddenIds.back().size(); i++) {

            for(auto &outputId : outputIds) {

                network.connectCells(hiddenIds.back()[i], outputId);
            }
        }

        network.finalizeConstruction();

        return network;
    }
}