#include <iostream>
#include "NnetTrainingData.hpp"
#include "bslcore/UtilLogger.hpp"
#include <nlohmann/json.hpp>
#include "CppUTest/TestHarness.h"


TEST_GROUP(NnetTrainingDataTest)
{
};

TEST(NnetTrainingDataTest, NnetTrainTest)
{
    UTIL::Logger logger("TopologyTestLogger");
    logger.setLevel(LL_DEBUG);

    nlohmann::json config = R"(
        [
            { "input" : [1, 0], "output" : [1] },
            { "input" : [1, 1], "output" : [0] },
            { "input" : [0, 1], "output" : [1] },
            { "input" : [0, 0], "output" : [0] },
            { "input" : [1, 0], "output" : [1] }
        ]
    )"_json;

    NNET::TrainingData data(logger, config);

    CHECK_EQUAL_TEXT(true, data.isReady(), "Data parse error?");

    CHECK_EQUAL_TEXT(5, data.getData().size(), "Not all of the data was loaded!");
}