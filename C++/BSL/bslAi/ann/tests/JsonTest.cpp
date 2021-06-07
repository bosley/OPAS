#include <iostream>
#include "AnnNeuron.hpp"
#include "AnnTopology.hpp"
#include "AnnNetwork.hpp"
#include "bslcore/UtilLogger.hpp"
#include "CppUTest/TestHarness.h"

#define TOLERANCE       0.001

namespace  {
    struct NeuronTestCase {
        int numOutputs;
        int index;
        ANN::TransferFunction transferFunc;
        double eta;
        double alpha;
    };

    NeuronTestCase neuronTestCases[8] = {
        { 4,  0,  ANN::TransferFunction::SIGMOID, ANN::Neuron::DEFAULT_ETA, ANN::Neuron::DEFAULT_ALPHA },
        { 9,  1,  ANN::TransferFunction::TANH,    0.99,                     ANN::Neuron::DEFAULT_ALPHA },
        { 44, 2,  ANN::TransferFunction::TANH,    ANN::Neuron::DEFAULT_ETA, ANN::Neuron::DEFAULT_ALPHA },
        { 2,  43, ANN::TransferFunction::SIGMOID, 1.0,                      0.99                       },
        { 99, 4,  ANN::TransferFunction::TANH,    ANN::Neuron::DEFAULT_ETA, 0.01                       },
        { 1,  54, ANN::TransferFunction::SIGMOID, ANN::Neuron::DEFAULT_ETA, 0.10                       },
        { 81, 6,  ANN::TransferFunction::TANH,    0.44,                     ANN::Neuron::DEFAULT_ALPHA },
        { 0,  7,  ANN::TransferFunction::SIGMOID, ANN::Neuron::DEFAULT_ETA, ANN::Neuron::DEFAULT_ALPHA },
    };

    struct TopologyTestcase {
        json config;
    };

    TopologyTestcase topologyTestCases[4] = {
        {
            R"(
                {
                    "input"  : 2,
                    "output" : 1,
                    "hidden" : [3,6,9]
                }
            )"_json
        },
        {
            R"(
                {
                    "input"  : 6,
                    "output" : 1,
                    "hidden" : [4,4,4]
                }
            )"_json
        },
        {
            R"(
                {
                    "input"  : 7,
                    "output" : 2,
                    "hidden" : [5,4,4]
                }
            )"_json
        },
        {
            R"(
                {
                    "input"  : 2,
                    "output" : 1,
                    "hidden" : [9,9,9,10,44]
                }
            )"_json
        }
    };
}

json jsonTestGetTopology();
json jsonTestGetTrainingData();

TEST_GROUP(AnnJsonTest)
{   
};

TEST(AnnJsonTest, Neuron)
{
    for(auto &tc : neuronTestCases) {

        ANN::Neuron neuron(tc.numOutputs, tc.index, tc.transferFunc, tc.eta, tc.alpha);

        json jsonNeuron = neuron.to_json();

        ANN::Neuron compNeuron(jsonNeuron);

        CHECK_EQUAL_TEXT(true, (neuron == compNeuron), "Neurons did not survive json-ing");
    }
}

TEST(AnnJsonTest, Topology)
{
    UTIL::Logger logger("Json-Topology");
    logger.setLevel(LL_TRACE);

    for(auto &tc : topologyTestCases) {

        ANN::Topology topology(logger, tc.config);

        json builtTopology = topology.to_json();

        ANN::Topology compTopology(logger, builtTopology);

        CHECK_EQUAL_TEXT(true, (topology == compTopology), "Topology did not survive json-ing");
    }
}

TEST(AnnJsonTest, NetworkBasic)
{
    UTIL::Logger logger("Json-Network");
    logger.setLevel(LL_TRACE);

    double bias = 0.0;
    for(auto &tc : neuronTestCases) {

        ANN::Topology topology(logger, topologyTestCases[0].config);

        ANN::Network network(logger, topology, tc.transferFunc, bias);

        json networkJson = network.to_json();

        ANN::Network compNetwork(logger, networkJson);

        CHECK_EQUAL_TEXT(true, (network == compNetwork), "Network did not survive json-ing");
    }
}

TEST(AnnJsonTest, NetworkTrained)
{
    UTIL::Logger logger("Json-TrainedNetwork");
    logger.setLevel(LL_TRACE);

    double bias = 0.0;
    for(int i = 0; i < 10; i++) {

        // ---------------------------------------- Build and train a newtwork
        json config = jsonTestGetTopology();
        json trainData = jsonTestGetTrainingData();

        ANN::Topology topology(logger, config);

        CHECK_EQUAL_TEXT(true, topology.isReady(), "Topology is not ready.");

        ANN::TrainingData training(logger, trainData);

        CHECK_EQUAL_TEXT(true, training.isReady(), "Training data is not ready.");

        ANN::Network network(logger, topology, ANN::TransferFunction::SIGMOID, bias);

        network.train(training, 1);

        // ---------------------------------------- Dump to json
        json networkJson = network.to_json();

        // ---------------------------------------- Build from json and check for equality
        ANN::Network compNetwork(logger, networkJson);

        CHECK_EQUAL_TEXT(true, (network == compNetwork), "Network did not survive json-ing");
  
        // We wont test inputs here, just ensuring data survives json-ing

        bias += 0.01;
    }
}

json jsonTestGetTopology() {

    std::ifstream ifs("testTopology.json");
    
    if(!ifs.is_open()) {
        FAIL("Unable to find testTopology.json");
    }

    json topology;
    ifs >> topology;
    return topology;
}

json jsonTestGetTrainingData() {

    std::ifstream ifs("testTrain.json");

    if(!ifs.is_open()) {
        FAIL("Unable to find testTrain.json");
    }

    json trainingData;
    ifs >> trainingData;
    return trainingData;
}