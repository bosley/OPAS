#include <iostream>
#include"nnet.h"

using namespace std;

int main()
{
    /*
            TOPOLOGY EXAMPLE : 2 4 1

            2 Input nodes
            4 Nodes in hidden layer
            1 Ouput node

              X
            X X
            X X X
              X

            First line [0] of input file MUST be topology

            Line [1..n] must contain training data
            Each line must have (Input nodes + output nodes) number of doubles
                to represent input values, and the expected result from those values
    */

    NNet nnet;
    nnet.setDebug(true);
    nnet.loadTrainingFile("C:\\Users\\Bosley\\Documents\\ANNet\\trainingData.txt");

    return 0;
}
