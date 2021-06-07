# NNET

Neural Network

Construct neural networks!

## Constructing a NNET
```cpp

    UTIL::Logger logger("DFFExample");
    logger.setLevel(LL_DEBUG);

    NNET::Builder networkBuilder(logger);

    NNET::Network dffNetwork = networkBuilder.buildDeepFeedForward(2, 3, 1, NNET::TransferFunction::TANH);
```

## Example training file for a trainer object
```json
[
    { "input" : [1, 0], "output" : [1] },
    { "input" : [1, 1], "output" : [0] },
    { "input" : [0, 1], "output" : [1] },
    { "input" : [0, 0], "output" : [0] }
]
```

## Training the network
```cpp

    int minutes         = 5;
    int TRAINING_ITR    = 100;
    int TRAINING_EPOCHS = 100000;
    double errorDelta   = 0.01;

    NNET::Trainer networkTrainer(logger, minutes * 60); // Make a network trainer that will last up-to 5 minutes max

    // Train for a certain number of iterations, or time expires
    networkTrainer.trainByItr(dffNetwork, getJsonFile("testTrain.json"), TRAINING_ITR, TRAINING_EPOCHS);

    // Train until errors are less than a given delta, or time expires
    networkTrainer.trainToComplete(dffNetwork, getJsonFile("testTrain.json"), errorDelta, TRAINING_EPOCHS); 

    // One or both above could be used
```

## Using the trained network

```cpp
    // Inputs should be the size() of the input dictated by the topology object
    std::vector<double> inputs = {1.0, 1.0};

    // Input the data into the network
    dffNetwork.input(inputs);

    // Results will be the output size dictated by the topology object
    std::vector<double> results = dffNetwork.getResults();
```

## Saving the network
```cpp

    // Get a json representation of the trained network
    json networkJson = dffNetwork.to_json();

    // The nlohmann json used here can be dumped into an ostream (file, etc)
```

## Loading a saved network
```cpp

    // Load a json object from the file stream
    json networkJson;

    ifs >> networkJson;

    // Construct the network with the json obkect
    ANN::Network network(logger, networkJson);

    // The network is now ready
```

## Manual network creation

Instead of using the NNET Builder, a network can be built by hand.

```cpp

    UTIL::Logger logger("logger");
    logger.setLevel(LL_TRACE);

    NNET::Network network(logger);

    /*
        Standard network. 2 inputs 3 hidden, 1 output. Fully connected
    */
    network.newLayer(); // Layer 0
    network.newLayer(); // Layer 1
    network.newLayer(); // Layer 2
    network.newLayer(); // Layer 3
    network.newLayer(); // Layer 4

    try{
        //          layer, cell, variant
        network.newCell(0,  0,   NNET::Cell::Variant::INPUT, NNET::TransferFunction::SIGMOID);
        network.newCell(0,  1,   NNET::Cell::Variant::INPUT, NNET::TransferFunction::TANH);
        
        network.newCell(1,  2,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::SIGMOID);
        network.newCell(1,  3,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH);
        network.newCell(1,  4,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::SIGMOID);

        network.newCell(2,  5,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH);
        network.newCell(2,  6,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::SIGMOID);
        network.newCell(2,  7,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH);

        network.newCell(3,  8,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH);
        network.newCell(3,  9,   NNET::Cell::Variant::HIDDEN, NNET::TransferFunction::TANH);

        network.newCell(4,  10,   NNET::Cell::Variant::OUTPUT);

    } catch (std::exception &e) {

        // Could fail if a layer is not found
        std::cout << e.what() << std::endl;
        exit(1);
    }

    try{
        
        // Input to hidden
        network.connectCells(0, 2);
        network.connectCells(0, 3);
        network.connectCells(0, 4);

        network.connectCells(1, 2);
        network.connectCells(1, 3);
        network.connectCells(1, 4);

        network.connectCells(2, 5);
        network.connectCells(3, 5);
        network.connectCells(4, 5);
        
        network.connectCells(2, 6);
        network.connectCells(3, 6);
        network.connectCells(4, 6);

        network.connectCells(2, 7);
        network.connectCells(3, 7);
        network.connectCells(4, 7);

        network.connectCells(5, 8);
        network.connectCells(6, 8);
        network.connectCells(7, 8);

        network.connectCells(5, 9);
        network.connectCells(6, 9);
        network.connectCells(7, 9);

        // Hidden to output
        network.connectCells(8, 10);
        network.connectCells(9, 10);

    } catch (std::exception &e) {

        // Could fail if one of the cell ids isnt correct
        std::cout << e.what() << std::endl;
        exit(1);
    }
    // Indicate construction as finalized
    network.finalizeConstruction();

    // At this point the network can be trained and saved

```