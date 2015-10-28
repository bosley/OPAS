#include "bdgraph.h"

#include <iostream>

using namespace bosley;

int main()
{
    // Build graph from a file

    BDGraph bdg1;

    if(!bdg1.buildFromFile((char*)"C:\\Users\\jbosley2\\Documents\\build-BDGraph-Desktop_Qt_5_5_0_MinGW_32bit-Debug\\debug\\graph1.txt"))
        return -1;

    bdg1.printMatrix();
    bdg1.calculateAllPaths();
    bdg1.printPaths();

    // Manually build new graph
    BDGraph bdg;

    bdg.setVerticesAndEdges(4, 6);

    bdg.addNode(0, 1, 10);  // Start node
    bdg.addNode(0, 2, 5);
    bdg.addNode(1, 2, 2);   // Intermediate nodes
    bdg.addNode(1, 3, 5);
    bdg.addNode(2, 1, 1);
    bdg.addNode(2, 3, 10);
    bdg.addNode(3, -1, -1); // Sink node (-1 to indicate no connections)

    bdg.printMatrix();
    bdg.calculateAllPaths();
    bdg.printPaths();

    return 0;
}

