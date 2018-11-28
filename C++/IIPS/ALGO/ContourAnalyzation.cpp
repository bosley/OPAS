/*
 * The MIT License
 *
 * Copyright 2018 josh.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   ContourAnalyzation.cpp
 * Author: josh
 * 
 * Created on November 15, 2018, 7:53 PM
 */

#include "ContourAnalyzation.h"

#include <vector>
#include <algorithm>
#include <opencv2/core/core.hpp>

#include <iostream>

namespace ALGO
{
namespace CONTOURS
{
    void Continuous(std::vector< cv::Point > contours, 
                                   std::vector< std::vector < cv::Point > > &continuous,
                                   unsigned lengthThreshold,
                                   unsigned maxX,
                                   unsigned maxY)
    {
        
        // Create a boolean representation of the image
        std::vector< std::vector<int> > matrix;
        matrix.reserve(maxY);
        for(unsigned y = 0; y <= maxY; y++)
        {
            std::vector< int > row;
            for(unsigned x = 0; x <= maxX; x++)
            {
                row.push_back(-1);
            }
            matrix.push_back(row);
        }
        
        // Place contour points
        for(auto c = contours.begin(); c != contours.end(); ++c)
        {
            matrix[ (*c).y ][ (*c).x ] = 0;
        }
        
        //
        //  Pass 1
        //
        // Step over each point
        int id = 1;
        for(unsigned y = 0; y < maxY; y++)
        {
            for(unsigned x = 0; x < maxX; x++)
            {
                // If a contour point
                if(matrix[y][x] != -1)
                {
                    // Get neighborhood boundary
                    int sy = y-1; int ey = y+1;
                    int sx = x-1; int ex = x+1;
                    
                    // Scan neighborhood
                    bool found = false;
                    int lowestInNeighborhood = id;
                    for(int yy = sy; yy <= ey; yy++)
                    {
                        for(int xx = sx; xx <= ex; xx++)
                        {
                            // Ensure the spot is within the matrix
                            if(xx < maxX && yy < maxY)
                            {
                                // If point in neighborhood previously marked,
                                // and lower than 
                                if(matrix[yy][xx] < lowestInNeighborhood && matrix[yy][xx] > 0)
                                {
                                    lowestInNeighborhood = matrix[yy][xx];
                                    found = true;
                                }
                            }
                        }
                    }
                    
                    // If brand new, no marked neighborhood, mark with default id value
                    // If something is found, take on that value
                    matrix[y][x] = lowestInNeighborhood;
                    
                    // If Brand new node, increase id
                    if(!found)
                    {
                        matrix[y][x] = id++;
                    }
                }
            }
        }
        
        // PASS 2
        for(unsigned y = maxY; y > 0; y--)
        {
            for(unsigned x = maxX; x > 0; x--)
            {
                if(matrix[y][x] != -1)
                {
                    // Get neighborhood boundary
                    int sy = y-1; int ey = y+1;
                    int sx = x-1; int ex = x+1;

                    for(int yy = sy; yy <= ey; yy++)
                    {
                        for(int xx = sx; xx <= ex; xx++)
                        {
                            // Ensure the spot is within the matrix
                            if(xx < maxX && yy < maxY)
                            {
                                // If point in neighborhood > current
                                // bring them down to current
                                if(matrix[yy][xx] > matrix[y][x])
                                {
                                    matrix[yy][xx] = matrix[y][x];
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Collect all continuous contours
        for(int i = 1; i < id; i++)
        {
            std::vector< cv::Point > points;
            for(unsigned y = 0; y < maxY; y++)
            {
                for(unsigned x = 0; x < maxX; x++)
                {
                    if(matrix[y][x] == i)
                    {
                        points.push_back(cv::Point(x,y));
                    }
                }
            }
            if(!points.empty() && points.size() > lengthThreshold)
            {
                continuous.push_back(points);
            }
        }
        
        /*
        // TEST OUTPUT
        for(unsigned y = 0; y < maxY; y++)
        {
            for(unsigned x = 0; x < maxX; x++)
            {
                // If a contour point
                
                if(matrix[y][x] == -1)
                    std::cout << ".\t";
                else
                    std::cout << matrix[y][x] << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << "==============" << std::endl;
        */
        
    }
}
}