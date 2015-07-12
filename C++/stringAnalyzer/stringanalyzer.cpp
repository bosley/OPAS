#include "stringanalyzer.h"

sanalyzer::stringAnalyzer::stringAnalyzer(){}

bool sanalyzer::stringAnalyzer::loadDataSets( int count, _dataSet data[] )
{
    numberOfDataSets = 0;

    for( int i = 0; i < count; i++)
    {
        allDataSets[i] = data[i];
        numberOfDataSets++;
    }
    return true;
}

bool sanalyzer::stringAnalyzer::compair(std::string x, std::string y)
{
    return scanner(x, y);
}

std::string sanalyzer::stringAnalyzer::ofDataSet(std::string x)
{
    std::map<int,std::string>::iterator itr;

    for( int i = 0; i < numberOfDataSets; i++ )
    {
        for( itr = allDataSets[i].dataSet.begin(); itr != allDataSets[i].dataSet.end(); ++itr)
        {
            if( scanner(x, itr->second) )
            {
                return allDataSets[i].dataLabel;
            }
        }
    }
    return " Data not in sets ";
}

// Uses Levenshtein Distance
bool sanalyzer::stringAnalyzer::scanner(std::string x, std::string y)
{
    const size_t xSize( x.size() );
    const size_t ySize( y.size() );
    const double largest  = ( xSize > ySize ? xSize : ySize );
    typedef std::string::const_iterator s_itr;

    if( xSize == 0 || ySize == 0 )
    {
        return false;
    }

    size_t indexA = 0, *cost = new size_t[xSize + 1];

    for( size_t s = 0; s <= xSize; s++)
    {
        cost[s] = s;
    }

    for( s_itr A = x.begin(); A != x.end(); ++A, ++indexA)
    {
        cost[0] = indexA+1;
        size_t corner = indexA, indexB = 0;

        for( s_itr B = y.begin(); B != y.end(); ++B, ++indexB)
        {
            size_t upper = cost[indexB+1];

            if( *A == *B )
            {
                cost[indexB+1] = corner;
            }
            else
            {
                size_t temp ( upper < corner ? upper : corner );
                cost[indexB+1] = ( cost[indexB] < temp ? cost[indexB] : temp )+1;
            }
            corner = upper;
        }
    }
    delete [] cost;

    /*
        Once difference is found, get
        a score and test against threshold
    */

    double score = (double)cost[xSize] / largest ;

    if( score <= DIFFERENCE_THRESHOLD )
    {
        return true;
    }
    else
    {
        return false;
    }
}


