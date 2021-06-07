# ANN

Artificial Neural Network

This is a basic neural network builder that used feed forward and back propagation. There isn't anything to support LSTM, RNN, or anything else.

## Constructing an ANN
```cpp

    UTIL::Logger logger("ExampleLogger");
    logger.setLevel(LL_DEBUG);

    ANN::Topology topology(logger, jsonConfig);

    ANN::Network network(logger, topology, ANN::TransferFunction::TANH);
```

## Training the network
```cpp

    // Need to create a training data object 
    ANN::TrainingData td(logger, jsonData);

    int iterations = 20000;

    network.train(td, iterations);
```

## Using the trained network

```cpp
    // Inputs should be the size() of the input dictated by the topology object
    std::vector<double> inputs = {1.0, 1.0};

    // Input the data into the network
    network.input(inputs);

    // Results will be the output size dictated by the topology object
    std::vector<double> results = network.getResults();
```

## Saving the network
```cpp

    // Get a json representation of the trained network
    json networkJson = network.to_json();

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

## Example Topology configuration file
```json
    {
        "input"  : 2,
        "output" : 1,
        "hidden" : [4, 2, 3, 4, 3]
    }
```

## Example training file for a TrainingData object
```json
[
    { "input" : [1, 0], "output" : [1] },
    { "input" : [1, 1], "output" : [0] },
    { "input" : [0, 1], "output" : [1] },
    { "input" : [0, 0], "output" : [0] }
]
```