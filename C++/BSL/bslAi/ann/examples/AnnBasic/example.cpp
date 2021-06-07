#include "AnnTopology.hpp"
#include "AnnNeuron.hpp"
#include "AnnNetwork.hpp"
#include "AnnTrainingData.hpp"
#include "AnnTypes.hpp"

#include "bslcore/UtilLogger.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace {

    struct ExampleCase {
        std::vector<double> input;
        bool expected;
    };

    ExampleCase tcs[8] = { 
        { {0.0, 0.0}, 0 },
        { {1.0, 0.0}, 1 },
        { {0.0, 1.0}, 1 },
        { {1.0, 1.0}, 0 },
        { {0.0, 0.0}, 0 },
        { {1.0, 0.0}, 1 },
        { {0.0, 1.0}, 1 },
        { {1.0, 1.0}, 0 }
    };
}

nlohmann::json getTestTopology() {

    std::ifstream ifs("testTopology.json");
    
    if(!ifs.is_open()) {
        std::cout << "Unable to find testTopology.json" << std::endl;
        exit(1);
    }

    nlohmann::json topology;
    ifs >> topology;
    return topology;
}

nlohmann::json getTestTrainingData() {

    std::ifstream ifs("testTrain.json");

    if(!ifs.is_open()) {
        std::cout << "Unable to find testTrain.json" << std::endl;
        exit(1);
    }

    nlohmann::json trainingData;
    ifs >> trainingData;
    return trainingData;
}

int main() {

    UTIL::Logger logger("AnnExampleLogger");
    logger.setLevel(LL_DEBUG);

    nlohmann::json config = getTestTopology();
    nlohmann::json trainData = getTestTrainingData();

    ANN::Topology topology(logger, config);
    ANN::TrainingData training(logger, trainData);

    if(!topology.isReady() || !training.isReady()) {
        std::cout << "There was an error with json data. Can not continue." << std::endl;
        exit(1);
    }

    ANN::Network network(logger, topology, ANN::TransferFunction::TANH);

    network.train(training, 2);

    for(auto &tc : tcs) {

        network.input(tc.input);

        std::vector<double> results = network.getResults();

        bool result = (results[0] < 0.5) ? false : true;

        std::cout << "Input : " 
                  << tc.input[0] 
                  << " xor " 
                  << tc.input[1] 
                  << " | Expected result: " 
                  << tc.expected 
                  << " | Actual: " 
                  << result 
                  << std::endl;
    }
    return 0;
}