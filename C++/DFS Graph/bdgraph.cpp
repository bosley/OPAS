#include "bdgraph.h"

namespace bosley
{

BDGraph::BDGraph()
{
    cleanLists();
}

bool BDGraph::buildFromFile(char *file)
{
    cleanLists();

    std::cout << "\t This program assumes there is a start node [0],\n\t and a finish node[n], as-well-as at least one path from s to f."
              << std::endl;

    // Read in file
    std::ifstream inFile(file, std::ios::in);

    // Make sure the thing opened
    if(inFile.fail())
    {
        std::cout << "Unable to find: " << file << std::endl;
        exit(0);
    }

    // Get V, and E count
    inFile >> vertices >> edges;

    // Read in remaining values, and place into adjMatrix
    int v = 0, adj = v, cost = v, current = -1;

    // Initialize list
    for(int i = 0; i < vertices; i++)
    {
        ILST temp;
            for(int j = 0; j < vertices; j++)
                    temp.push_back(-1);
        adjMatrix.push_back(temp);
    }

    // For ensuring good data in from file
    bool f = 0;

    // Populate adjacency list
    while(inFile >> v >> adj >> cost)
    {
           // Grab start node
           if(!f)
           {
                   f = 1;
                   if(0 != v)
                   {
                           // Make sure that the start node is listed as 0
                           std::cout << std::endl << "Start node MUST be 0" << std::endl;
                           exit(0);
                   }
           }

           // If new vertex is found
           if(current != v)
                   current = v;

           // Indicate that the vertex is adjacent to the item
           // Save the cost by setting that as the label
           adjMatrix[v][adj] = cost;
    }

    inFile.close();

    return true;
}

void BDGraph::setVerticesAndEdges(int v, int e)
{
    cleanLists();

    edges = e;
    vertices = v;

    // Initialize list
    for(int i = 0; i < vertices; i++)
    {
        ILST temp;
            for(int j = 0; j < vertices; j++)
                    temp.push_back(-1);
        adjMatrix.push_back(temp);
    }
}

bool BDGraph::addNode(int node, int adjNode, int weight)
{
    adjMatrix[node][adjNode] = weight;

    return (adjMatrix[node][adjNode] == weight);
}

void BDGraph::printPaths()
{
    std::cout << std::endl << "Paths:" << std::endl;
    for(VILSTITR i = paths.begin(); i != paths.end(); i++)
    {
        std::cout << std::endl;
        for(ILSTITR j = (*i).begin(); j != (*i).end(); j++)
        {
            std::cout << (*j) << " ";
        }
    }
    std::cout << std::endl;
}

void BDGraph::printMatrix()
{
    // Print X axis labels
    std::cout << std::endl << "Graph Matrix : " << std::endl;
    for(int i = 0; i < vertices; i++)
        std::cout << "\t " << i;
    std::cout << std::endl << std::endl;

    int k = 0;
    for(VILSTITR i = adjMatrix.begin(); i != adjMatrix.end(); i++)
    {
        // Show y
        std::cout << k++ << "\t ";
        for(ILSTITR j = (*i).begin(); j != (*i).end(); j++)
        {
            if((*j) == -1)
                    std::cout << "--\t";
            else
                    std::cout << (*j) << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}

void BDGraph::calculateAllPaths()
{
    paths.clear();
    int pathIndex = 0;
    int *path = new int[vertices];
    bool *closed = new bool[vertices];

    for(int i = 0; i < vertices; i++)
            closed[i] = false;

    dfs(0, (vertices-1), closed, path, pathIndex);
}

void BDGraph::calculateSpecificPath(int s, int g)
{
    paths.clear();
    int pathIndex = 0;
    int *path = new int[vertices];
    bool *closed = new bool[vertices];

    for(int i = 0; i < vertices; i++)
            closed[i] = false;

    dfs(s, g, closed, path, pathIndex);
}

ILST BDGraph::shortestPath()
{
    ILST temp;
    std::cout << "Not yet complete" << std::endl;
    return temp;
}

void BDGraph::dfs(int c, int g, bool closed[], int path[], int &pi)
{
    path[pi++] = c;
    closed[c] = true;

    if(g == c)
    {
        // If goal node is current node, add the path that got us here
        ILST temp;
            for(int i = 0; i < pi; i++)
                temp.push_back(path[i]);
        paths.push_back(temp);
    }
    else
    {
        for(int i = 0; i < vertices; i++)
        {
            // Find open adjacent node, and search it
            if(adjMatrix[c][i] != -1 && !closed[i])
                    dfs(i, g, closed, path, pi);
        }
    }

    closed[c] = 0;
    pi--;
}

void BDGraph::cleanLists()
{
    edges = -1;
    vertices = -1;

    paths.clear();
    adjMatrix.clear();

}

}
