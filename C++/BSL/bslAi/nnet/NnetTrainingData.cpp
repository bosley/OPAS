#include "NnetTrainingData.hpp"

namespace NNET
{
    // -------------------------------------------------
    // TrainingData
    // -------------------------------------------------
    TrainingData::TrainingData(UTIL::Logger &logger, nlohmann::json data) : 
                                                                            logger(logger),
                                                                            ready(false) {
        
        for(auto &item : data) {

            if(!item.contains("input")  ||
               !item.contains("output")) {
                   LOG_WARN(logger) << "Invalid training data invalid check your json!\n";
                   return;
               }

            Data segment;
            try {
                
                segment.input     = item["input"].get<std::vector<double>>();
                segment.expected  = item["output"].get<std::vector<double>>();

            } catch (nlohmann::json::exception& e) {

                LOG_WARN(logger) << "Exception parsing json : " << e.what()
                                << "\nID: " << e.id << "\n";
                return;
            }

            trainingData.push_back(segment);
        }

        ready = true;


        LOG_DEBUG(logger) << "Training data size : " << trainingData.size() << "\n";
    }

    // -------------------------------------------------
    // getData
    // -------------------------------------------------
    std::vector<NNET::TrainingData::Data> TrainingData::getData() const {

        return trainingData;
    }

    // -------------------------------------------------
    // isReady
    // -------------------------------------------------
    bool TrainingData::isReady() const {

        return ready;
    }

}