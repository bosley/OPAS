#include "NnetTrainer.hpp"

namespace NNET
{
    // -------------------------------------------------
    // Trainer
    // -------------------------------------------------
    Trainer::Trainer(UTIL::Logger &logger, double trainTime) : logger(logger) {

        timer = new UTIL::Timer(trainTime);
    }

    // -------------------------------------------------
    // ~Trainer
    // -------------------------------------------------
    Trainer::~Trainer() {
        delete timer;
    }

    // -------------------------------------------------
    // trainByItr
    // -------------------------------------------------
    void Trainer::trainByItr(Network &network, json trainingData, uint64_t itr, uint64_t epochs) {

        data = new TrainingData(logger, trainingData);

        uint64_t currentIteration = 1;

        timer->reset();

        while(currentIteration <= itr) {

            LOG_DEBUG(logger) << "\n<---------------- [" << currentIteration << "/" << itr << "] ---------------->\n";

            network.train((*data), epochs, NNET::Network::CorrectionMethod::BACK_PROPAGATION);
             
            std::vector<TrainingData::Data> testData = data->getData();

            for(auto & v : testData) {

                json easyOutput;

                easyOutput["input"]    = v.input;
                easyOutput["expected"] = v.expected;

                network.input(v.input);

                std::vector<double> results = network.getResults();

                easyOutput["results"] = results;

                LOG_DEBUG(logger) << easyOutput.dump(4) << "\n";
            }

            currentIteration++;

            if(timer->isExpired()) {

                LOG_DEBUG(logger) << "Training timer has expired. Aborting training sequence\n";
                delete data;
                return;
            }
        }

        delete data;
    }

    // -------------------------------------------------
    // trainToComplete
    // -------------------------------------------------
    void Trainer::trainToComplete(Network &network, json trainingData, double delta, unsigned epoch) {

        data = new TrainingData(logger, trainingData);

        uint64_t currentIteration = 1;

        bool doTrain = true;

        timer->reset();

        while(doTrain) {

            if(timer->isExpired()) {

                LOG_DEBUG(logger) << "Training timer has expired. Aborting training sequence\n";
                delete data;
                return;
            }

            LOG_DEBUG(logger) << "\n<---------------- [" << currentIteration << "/ ? ] ---------------->\n";

            network.train((*data), epoch, NNET::Network::CorrectionMethod::BACK_PROPAGATION);
             
            std::vector<TrainingData::Data> testData = data->getData();

            std::vector<double> errors;
            for(auto & v : testData) {

                json easyOutput;

                easyOutput["input"]    = v.input;
                easyOutput["expected"] = v.expected;

                network.input(v.input);

                std::vector<double> results = network.getResults();

                easyOutput["results"] = results;

                for(int i = 0; i < v.expected.size(); i++) {

                    double err = std::abs(v.expected[i] - results[i]);

                    if(err > delta) {

                        errors.push_back(err);
                    }
                }

                if(errors.size() == 0) {
                    doTrain = false;
                }

                easyOutput["deltas"] = errors;

                LOG_DEBUG(logger) << easyOutput.dump(4) << "\n";
            }

            currentIteration++;
        }

        delete data;
    }
}