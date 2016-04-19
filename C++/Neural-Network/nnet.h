#ifndef NNET_H
#define NNET_H

#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <fstream>
#include <iostream>

#include "network.h"

class NNet
{
public:
    NNet();
    void setDebug(const bool state);
    void loadTrainingFile(const std::string file);

private:

    // The neural network
    Network n_Net;

    bool debug;
    std::vector<std::string> getTokens(std::string &s);

};

#endif // NNET_H
