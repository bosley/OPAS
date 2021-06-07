
#include "AnnTopology.hpp"
#include "AnnNeuron.hpp"
#include "AnnNetwork.hpp"
#include "AnnTrainingData.hpp"
#include "AnnTypes.hpp"

#include "bslcore/UtilLogger.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include "CppUTest/TestHarness.h"

json getTestTopology();
json getTestTrainingData();

namespace {

    constexpr int testCasesNum = 4;
    constexpr int trainingItrs = 100000;

    struct TestCase {
        std::vector<double> input;
        bool expected;
    };

    TestCase tcs[testCasesNum] = { 
        { {0.0, 0.0}, 0 },
        { {1.0, 0.0}, 1 },
        { {0.0, 1.0}, 1 },
        { {1.0, 1.0}, 0 }
    };
}

TEST_GROUP(AnnNetworkBuildTest)
{
};

TEST(AnnNetworkBuildTest, networkBuildTestUsingTanh)
{
    UTIL::Logger logger("NBTLogger");
    logger.setLevel(LL_TRACE);

    json config = getTestTopology();
    json trainData = getTestTrainingData();

    ANN::Topology topology(logger, config);

    CHECK_EQUAL_TEXT(true, topology.isReady(), "Topology is not ready.");

    ANN::TrainingData training(logger, trainData);

    CHECK_EQUAL_TEXT(true, training.isReady(), "Training data is not ready.");

    ANN::Network network(logger, topology, ANN::TransferFunction::TANH);

    network.train(training, trainingItrs);

    int incorrect = 0;
    for(auto &tc : tcs) {

        network.input(tc.input);

        std::vector<double> results = network.getResults();

        bool result = (results[0] < 0.5) ? false : true;

        if(tc.expected != result) {
            incorrect++;
        }
    }

    logger.setLevel(LL_DEBUG);
    LOG_DEBUG(logger) << incorrect << " of " << testCasesNum 
                        << " xor inputs failed for tanh network after " 
                        << trainingItrs << " training iterations\n";

    CHECK_EQUAL_TEXT(true, (incorrect != testCasesNum), "Network failed ALL test inputs");
}

TEST(AnnNetworkBuildTest, networkBuildTestUsingSigmoid)
{
    UTIL::Logger logger("NBTLogger");
    logger.setLevel(LL_TRACE);

    json config = getTestTopology();
    json trainData = getTestTrainingData();

    ANN::Topology topology(logger, config);

    CHECK_EQUAL_TEXT(true, topology.isReady(), "Topology is not ready.");

    ANN::TrainingData training(logger, trainData);

    CHECK_EQUAL_TEXT(true, training.isReady(), "Training data is not ready.");

    ANN::Network network(logger, topology, ANN::TransferFunction::SIGMOID);

    network.train(training, trainingItrs);

    int incorrect = 0;

    for(auto &tc : tcs) {

        network.input(tc.input);

        std::vector<double> results = network.getResults();

        bool result = (results[0] < 0.5) ? false : true;

        if(tc.expected != result) {
            incorrect++;
        }
    }

    logger.setLevel(LL_DEBUG);
    LOG_DEBUG(logger) << incorrect << " of " << testCasesNum 
                        << " xor inputs failed for sigmoid network after " 
                        << trainingItrs << " training iterations\n";

    CHECK_EQUAL_TEXT(true, (incorrect != testCasesNum), "Network failed ALL test inputs");
}


json getTestTopology() {

    std::ifstream ifs("testTopology.json");
    
    if(!ifs.is_open()) {
        FAIL("Unable to find testTopology.json");
    }

    nlohmann::json topology;
    ifs >> topology;
    return topology;
}

json getTestTrainingData() {

    std::ifstream ifs("testTrain.json");

    if(!ifs.is_open()) {
        FAIL("Unable to find testTrain.json");
    }

    nlohmann::json trainingData;
    ifs >> trainingData;
    return trainingData;
}