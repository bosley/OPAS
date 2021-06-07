#include <iostream>
#include <fstream>
#include "NnetCell.hpp"
#include "NnetNetwork.hpp"
#include "bslcore/UtilLogger.hpp"
#include "CppUTest/TestHarness.h"

#define TOLERANCE       0.001

namespace  {
    struct CellTestCase {
        unsigned id;
        unsigned layer;
        NNET::Cell::Variant variant;
        NNET::TransferFunction tf;
        double eta;
        double alpha;
    };

    CellTestCase cellTestCases[8] = {
        { 4,  0,  NNET::Cell::Variant::INPUT,  NNET::TransferFunction::SIGMOID, NNET::Cell::DEFAULT_ETA,  NNET::Cell::DEFAULT_ALPHA },
        { 9,  10, NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH,    0.99,                     NNET::Cell::DEFAULT_ALPHA },
        { 44, 0,  NNET::Cell::Variant::OUTPUT, NNET::TransferFunction::TANH,    NNET::Cell::DEFAULT_ETA,  NNET::Cell::DEFAULT_ALPHA },
        { 2,  6,  NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::SIGMOID, 1.0,                      0.99                      },
        { 99, 5,  NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH,    NNET::Cell::DEFAULT_ETA,  0.01                      },
        { 1,  4,  NNET::Cell::Variant::INPUT,  NNET::TransferFunction::SIGMOID, NNET::Cell::DEFAULT_ETA,  0.10                      },
        { 81, 2,  NNET::Cell::Variant::OUTPUT, NNET::TransferFunction::TANH,    0.44,                     NNET::Cell::DEFAULT_ALPHA },
        { 0,  1,  NNET::Cell::Variant::INPUT,  NNET::TransferFunction::SIGMOID, NNET::Cell::DEFAULT_ETA,  NNET::Cell::DEFAULT_ALPHA },
    };

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

json jsonTestGetFile(std::string file);

TEST_GROUP(NnetJsonTest)
{   
};

TEST(NnetJsonTest, Cell)
{
    for(auto &tc : cellTestCases) {

        NNET::Cell cell(tc.id, tc.layer, tc.variant, tc.tf, tc.eta, tc.alpha);

        json jsonCell;

        NNET::to_json(jsonCell, cell);

        NNET::Cell compCell(jsonCell);

        // Cannot rely on equality operator for cell, it is used for the internal workings of the cell operations
        // and needs to check only for the id which is not relyable for here
        CHECK_EQUAL_TEXT(true, NNET::doCellsMatch(cell, compCell), "Cells did not survive json-ing");
    }
}

TEST(NnetJsonTest, NetworkBasic)
{
    UTIL::Logger logger("Json-Network");
    logger.setLevel(LL_TRACE);

    NNET::Network network(logger);
    network.newLayer(); // Layer 0
    network.newLayer(); // Layer 1
    network.newLayer(); // Layer 2

    network.newCell(0,  0,   NNET::Cell::Variant::INPUT, NNET::TransferFunction::SIGMOID);
    network.newCell(0,  1,   NNET::Cell::Variant::INPUT, NNET::TransferFunction::SIGMOID);
    network.newCell(1,  2,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH);
    network.newCell(1,  3,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH);
    network.newCell(1,  4,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::SIGMOID);
    network.newCell(2,  5,   NNET::Cell::Variant::OUTPUT);

    try{
        network.connectCells(0, 2);
        network.connectCells(0, 3);
        network.connectCells(0, 4);
        network.connectCells(1, 2);
        network.connectCells(1, 3);
        network.connectCells(1, 4);
        network.connectCells(2, 5);
        network.connectCells(3, 5);
        network.connectCells(4, 5);

    } catch (std::exception &e) {

        // Could fail if one of the cell ids isnt correct
        FAIL(e.what());
    }

    // Indicate construction as finalized
    network.finalizeConstruction();

    // Load training data
    NNET::TrainingData data(logger, jsonTestGetFile("testTrain.json"));

    network.train(data, TRAINING_EPOCHS);

    // Record results
    std::vector< std::vector<double> > builtRecordedResults;
    for(auto &tc : tcs) {

        network.input(tc.input);
        builtRecordedResults.push_back(network.getResults());
    }

    // Get trained network json config
    json trainedNetwork = network.get_json();

    // Load a network from the json
    NNET::Network loadedNet(logger, trainedNetwork);

    // Get the loaded json
    json loadedNetwork = loadedNet.get_json();

    // Check that the networks are the same as-well-as their resulting json strings
    std::string strTrained = trainedNetwork.dump(4);
    std::string strLoaded  = loadedNetwork.dump(4);

    //std::cout << strTrained << std::endl << "------------------------" << std::endl << strLoaded << std::endl;

    // This might be okay if cell weights dont transfer correctly, as cell equality operation checks id, not weight values
    CHECK_EQUAL_TEXT(true, (network == loadedNet), "Network did not survive jsoning");

    // So to ensure equality, we check the raw strings
    CHECK_EQUAL_TEXT(strTrained, strLoaded, "Network json strings differ");

    // Get config-loaded json results from same examples input
    std::vector< std::vector<double> > configdRecordedResults;
    for(auto &tc : tcs) {

        loadedNet.input(tc.input);
        configdRecordedResults.push_back(loadedNet.getResults());
    }

    // Ensure that the same results (good or bad) come from the built network and the configured network
    CHECK_EQUAL_TEXT(true, (builtRecordedResults == configdRecordedResults), "Different results yeilded from saved network");

}

json jsonTestGetFile(std::string fileName) {

    std::ifstream ifs(fileName);

    if(!ifs.is_open()) {
        FAIL("Unable to find .json file");
    }

    json file;
    ifs >> file;
    return file;
}