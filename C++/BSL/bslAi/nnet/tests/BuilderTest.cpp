
#include <chrono>
#include <iostream>
#include <fstream>

#include "bslcore/UtilLogger.hpp"

#include "NnetCell.hpp"
#include "NnetNetwork.hpp"
#include "NnetBuilder.hpp"

#include "CppUTest/TestHarness.h"

typedef nlohmann::json json;

namespace {

    constexpr int TRAIN_ATTEMPTS = 10;
    constexpr int TEST_CASE_NUM = 4;
    constexpr int TRAINING_EPOCHS = 100000;

    struct TestCase {
        std::vector<double> input;
        bool expected;
    };

    TestCase tcs[TEST_CASE_NUM] = { 
        { {0.0, 0.0}, 0 },
        { {1.0, 0.0}, 1 },
        { {0.0, 1.0}, 1 },
        { {1.0, 1.0}, 0 }
    };
}

json builderTestGetFile(std::string fileName);

TEST_GROUP(NetBuilderTest)
{
};

TEST(NetBuilderTest, builderTestDFF)
{
    UTIL::Logger logger("builderTestDFF");
    logger.setLevel(LL_TRACE);

    NNET::Builder networkBuilder(logger);

    NNET::Network dffNetwork = networkBuilder.buildDeepFeedForward(2, 3, 1, NNET::TransferFunction::TANH);

    // Load training data
    NNET::TrainingData data(logger, builderTestGetFile("testTrain.json"));

    dffNetwork.train(data, TRAINING_EPOCHS);

    int incorrect = 0;
    for(auto &tc : tcs) {

        dffNetwork.input(tc.input);

        std::vector<double> results = dffNetwork.getResults();

        bool result = (results[0] < 0.5) ? false : true;

        if(tc.expected != result) {
            incorrect++;
        }
    }

    logger.setLevel(LL_DEBUG);
    LOG_DEBUG(logger) << incorrect << " of " << TEST_CASE_NUM 
                        << " xor inputs failed for dff network after " 
                        << TRAINING_EPOCHS << " training iterations\n";
}

TEST(NetBuilderTest, builderTestRecurrent)
{
    UTIL::Logger logger("builderTestRecurrent");
    logger.setLevel(LL_TRACE);

    NNET::Builder networkBuilder(logger);

    NNET::Network dffNetwork = networkBuilder.buildRecurrent(2, 3, 1, NNET::TransferFunction::SIGMOID);

    // Load training data
    NNET::TrainingData data(logger, builderTestGetFile("testTrain.json"));

    dffNetwork.train(data, TRAINING_EPOCHS);

    int incorrect = 0;
    for(auto &tc : tcs) {

        dffNetwork.input(tc.input);

        std::vector<double> results = dffNetwork.getResults();

        bool result = (results[0] < 0.5) ? false : true;

        if(tc.expected != result) {
            incorrect++;
        }
    }

    logger.setLevel(LL_DEBUG);
    LOG_DEBUG(logger) << incorrect << " of " << TEST_CASE_NUM 
                        << " xor inputs failed for recurrent network after " 
                        << TRAINING_EPOCHS << " training iterations\n";
}

TEST(NetBuilderTest, builderTestLSTM)
{
    UTIL::Logger logger("builderTestLSTM");
    logger.setLevel(LL_TRACE);

    NNET::Builder networkBuilder(logger);

    NNET::Network lstmNetwork = networkBuilder.buildLstm(2, 3, 1);

    // Load training data
    NNET::TrainingData data(logger, builderTestGetFile("testTrain.json"));

    lstmNetwork.train(data, TRAINING_EPOCHS);

    int incorrect = 0;
    for(auto &tc : tcs) {

        lstmNetwork.input(tc.input);

        std::vector<double> results = lstmNetwork.getResults();

        bool result = (results[0] < 0.5) ? false : true;

        if(tc.expected != result) {
            incorrect++;
        }
    }

    logger.setLevel(LL_DEBUG);
    LOG_DEBUG(logger) << incorrect << " of " << TEST_CASE_NUM 
                        << " xor inputs failed for LSTM network after " 
                        << TRAINING_EPOCHS << " training iterations\n";
}

TEST(NetBuilderTest, builderTestGatedLSTM)
{
    UTIL::Logger logger("builderTestGatedLSTM");
    logger.setLevel(LL_TRACE);

    NNET::Builder networkBuilder(logger);

    NNET::Network gatedLstmNetwork = networkBuilder.buildLstm(2, 3, 1);

    // Load training data
    NNET::TrainingData data(logger, builderTestGetFile("testTrain.json"));

    gatedLstmNetwork.train(data, TRAINING_EPOCHS);

    int incorrect = 0;
    for(auto &tc : tcs) {

        gatedLstmNetwork.input(tc.input);

        std::vector<double> results = gatedLstmNetwork.getResults();

        bool result = (results[0] < 0.5) ? false : true;

        if(tc.expected != result) {
            incorrect++;
        }
    }

    logger.setLevel(LL_DEBUG);
    LOG_DEBUG(logger) << incorrect << " of " << TEST_CASE_NUM 
                        << " xor inputs failed for LSTM network after " 
                        << TRAINING_EPOCHS << " training iterations\n";
}

json builderTestGetFile(std::string fileName) {

    std::ifstream ifs(fileName);

    if(!ifs.is_open()) {
        FAIL("Unable to find .json file");
    }

    json file;
    ifs >> file;
    return file;
}