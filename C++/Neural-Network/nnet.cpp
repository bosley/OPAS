#include "nnet.h"

NNet::NNet()
{
    debug = false;
}

void NNet::setDebug(const bool state)
{
    debug = state;
    n_Net.setDebug(state);
}

void NNet::loadTrainingFile(const std::string file)
{
    if(debug)
    {
        std::cout << "Loading file > " << file << std::endl;
    }

    std::string line;
    std::ifstream iFile(file);

    int lineCount = 0;
    bool first = true;
    unsigned in, hi, op;
    unsigned expected = 0;

    while(std::getline(iFile, line))
    {
        lineCount++;

        // Handle the first line differently than everything else
        if(first)
        {
            if(debug)
            {
                std::cout <<
                "Getting topology : " << line << std::endl << "Will add 1 bias node per-layer!"
                << std::endl;
            }

            std::vector<std::string> topology = getTokens(line);
            if(3 != topology.size())
            {
                std::cout << "Topology error : Must be 3 integers [Input nodes, hidden, output]" << std::endl;
            }

            // Convert to int the input, hidden, and output node counts
            std::istringstream(topology[0]) >> in;
            std::istringstream(topology[1]) >> hi;
            std::istringstream(topology[2]) >> op;

            // Create the topology
            std::vector<unsigned> top;
            top.push_back(in);
            top.push_back(hi);
            top.push_back(op);

            if(debug)
            {
                std::cout << std::endl << "Setting topology.. " << std::endl;
            }

            // Set the topology
            n_Net.setTopology(top);

            if(debug)
            {
                std::cout << "done." << std::endl << std::endl;
            }

            // Set the expected number of items in at this point to be i + o
            expected = in + op;
            first = false;

            // Flip the loop
            continue;
        }

        /*

                Every line that isn't the first. . .

        */

        std::vector<std::string> tokens = getTokens(line);

        if(expected != tokens.size())
        {
            std::cout << "Unexpected number of input tokens in input file on line " << lineCount << std::endl;
        }

        // Add the input values
        std::vector<double> inputValues;
        for(unsigned i = 0; i < in; i++)
        {
            if(debug)
            {
                std::cout << "Adding input\t: " << tokens[i] << std::endl;
            }
            double val;
            std::istringstream(tokens[i]) >> val;
            inputValues.push_back(val);
        }

        // Add target(s)
        std::vector<double> targetVals;
        for(unsigned i = in; i < expected; i++)
        {
            if(debug)
            {
                std::cout << "Adding target\t: " << tokens[i] << std::endl;
            }
            double val;
            std::istringstream(tokens[i]) >> val;
            targetVals.push_back(val);
        }

        if(debug)
        {
            std::cout << "Feeding the data forward.. ";
        }

        // Feed forward the input data
        n_Net.doFeedForward(inputValues);

        if(debug)
        {
            std::cout << "done." << std::endl
                      << "Performing back propagation.. ";
        }

        // Perform back propagation
        n_Net.doBackPropagation(targetVals);

        // Only get results if in debug for display.
        if(debug)
        {
            std::cout << "done." << std::endl << "Results : ";
            std::vector<double> resultVals;
            n_Net.getResults(resultVals);

            for(unsigned c = 0; c < resultVals.size(); ++c)
            {
                std::cout << "[" << resultVals[c] << "]"
                          << std::endl << std::endl;
            }
        }
    }
    iFile.close();
}

std::vector<std::string> NNet::getTokens(std::string &s)
{
    std::istringstream iss(s);
    std::vector<std::string> result;
    for (std::string token; std::getline(iss, token, ' '); )
        result.push_back(std::move(token));
    return result;
}


