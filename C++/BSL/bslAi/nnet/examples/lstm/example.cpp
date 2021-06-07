#include "NnetBuilder.hpp"
#include "NnetTrainer.hpp"
#include "NnetTrainingData.hpp"
#include "bslcore/UtilLogger.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace {

    constexpr int TEST_CASE_NUM   = 4;
    constexpr int TRAINING_ITR    = 100;
    constexpr int TRAINING_EPOCHS = 100000;

    struct ExampleCase {
        std::vector<double> input;
        bool expected;
    };

    ExampleCase tcs[TEST_CASE_NUM] = { 
        { {0.0, 0.0}, 0 },
        { {1.0, 0.0}, 1 },
        { {0.0, 1.0}, 1 },
        { {1.0, 1.0}, 0 }
    };
}

json getJsonFile(std::string fileName) {

    std::ifstream ifs(fileName);

    if(!ifs.is_open()) {
        std::cerr << "Unable to find : " << fileName << std::endl;
        exit(1);
    }

    json file;
    ifs >> file;
    return file;
}

int main() {

    double errorDelta = 0.0001;
    double minutes    = 10;

    UTIL::Logger logger("LSTMExample");
    logger.setLevel(LL_DEBUG);

    NNET::Builder networkBuilder(logger);

    NNET::Network network = networkBuilder.buildLstm(2, 4, 1);

    // Allow 10 minutes of training as an upper bound
    NNET::Trainer networkTrainer(logger, minutes * 60);

   // networkTrainer.trainByItr(network, getJsonFile("testTrain.json"), TRAINING_ITR, TRAINING_EPOCHS);

    networkTrainer.trainToComplete(network, getJsonFile("testTrain.json"), errorDelta, TRAINING_EPOCHS);

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
    LOG_DEBUG(logger) << incorrect << " of " << TEST_CASE_NUM 
                        << " xor inputs failed for examples lstm network after "
                        << TRAINING_ITR 
                        << " training iterations @ " 
                        << TRAINING_EPOCHS 
                        << " epochs per itr\n";
    return 0;
}