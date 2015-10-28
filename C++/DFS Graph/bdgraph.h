#ifndef BDGRAPH_H
#define BDGRAPH_H

#include <vector>
#include <fstream>
#include <iostream>

namespace bosley
{

typedef std::vector<int> ILST;
typedef ILST::iterator ILSTITR;
typedef std::vector<ILST> VILST;
typedef VILST::iterator VILSTITR;

class BDGraph
{
public:
    BDGraph();

    // Building from file erases old matrix
    // adding a node only appends to matrix
    bool buildFromFile(char * file);

    // Need to set vertices before adding nodes
    // Setting vertices and edges resets paths/adjMatrix
    void setVerticesAndEdges(int v, int e);
    bool addNode(int node, int adjNode, int weight);

    // Outputs all paths in paths, and matrix
    // a calculate function must be called before this
    // is made available
    void printPaths();
    void printMatrix();

    // Each method will erase paths, and place
    // their generated paths within the paths VILST
    void calculateAllPaths();
    void calculateSpecificPath(int s, int g);

    // Clean all lists to load new graph
    void cleanLists();

    // Returned the shortest path within paths
    ILST shortestPath();

private:
    // Graph paths, matrix, and corresponding information
    VILST paths;
    VILST adjMatrix;
    int vertices, edges;

    // Depth-First Search method
    void dfs(int s, int g, bool closed[], int path[], int &pi);

};

}
#endif // BDGRAPH_H
