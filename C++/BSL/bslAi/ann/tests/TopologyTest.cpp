#include <iostream>
#include "AnnTopology.hpp"
#include "bslcore/UtilLogger.hpp"
#include <nlohmann/json.hpp>
#include "CppUTest/TestHarness.h"


TEST_GROUP(AnnTopologyTest)
{
};

TEST(AnnTopologyTest, AnnJsonTest)
{
    UTIL::Logger logger("TopologyTestLogger");
    logger.setLevel(LL_DEBUG);

    nlohmann::json config = R"(
        {
            "input"  : 2,
            "output" : 1,
            "hidden" : [1,2,3]
        }
    )"_json;

    ANN::Topology topology(logger, config);

    CHECK_EQUAL_TEXT(true, topology.isReady(), "Topology is not ready.");
}