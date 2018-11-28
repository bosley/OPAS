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
 * File:   Sobel.cpp
 * Author: josh
 * 
 * Created on November 14, 2018, 10:25 PM
 */

#include "Sobel.h"

#include <iostream>

#include "../LOG/aixlog.hpp"

namespace ALGO
{
    
Sobel::Sobel() : _thresh(SOBEL_DEFAULT_THRESH)
{
}

Sobel::Sobel(const Sobel& orig) : _stepperCallback(nullptr)
{
}

Sobel::~Sobel() {
}

void Sobel::defineDb(std::shared_ptr< IPF::DataBank > db)
{
    _db = db;
}

void Sobel::defineSeriesCallback(IPF::StepperIF *stepper)
{
    if(_stepperCallback == nullptr)
        _stepperCallback = stepper;
}

void Sobel::setThresh(int thresh)
{
    _thresh = thresh;
}

void Sobel::process()
{
    cv::Mat gray;
    if(!_db->imageMap.getItem(std::string("frameGrayBlur"), gray))
    {
        LOG(WARNING) << "Unable to grab frame from imageMap" << std::endl;
        return;
    }
    if(gray.empty())
    {
        LOG(WARNING) << "Empty frame pulled from imageMap" << std::endl;
        return;
    }
    runSobel(gray);
}

void Sobel::runSobel(cv::Mat gray)
{
    std::vector < float > angles;
    std::vector < cv::Point > contours;
    for(int x = 1; x < gray.rows-2; x++)
    {
        for(int y = 1; y < gray.cols-2; y++)
        {
            // Bottom
            short b0 = (short)gray.at<uchar>(x-1,y-1);
            short b1 = (short)gray.at<uchar>(x,y-1);
            short b2 = (short)gray.at<uchar>(x+1,y-1);

            // Middle
            short m0 = (short)gray.at<uchar>(x-1,y);
            // Middle value (x,y) always multd by 0
            short m2 = (short)gray.at<uchar>(x+1,y);

            // Top
            short t0 = (short)gray.at<uchar>(x-1,y+1);
            short t1 = (short)gray.at<uchar>(x,y+1);
            short t2 = (short)gray.at<uchar>(x+1,y+1);
            
            /*
                GX             GY
                   1, 0,-1,        1, 2, 1,
                   2, 0,-2,        0, 0, 0,
                   1, 0,-1        -1,-2,-1
            */

            // Reduced from matrix operations
            int px = b0 + (-b2) + (2 * m0) + (-2 * m2) + t0 + (-t2);
            int py = b0 + (2 * b1) + b2 + (-t0) + (-2 * t1) + (-t2);

            int value = static_cast<int>(std::ceil(std::sqrt((px*px) + (py*py))));
            if(value > 255)
                value = 255;

            if(value >= _thresh)
            {
                contours.push_back(cv::Point(x,y));
                if(px != 0)
                    angles.push_back( std::atan(py/px) );
                else
                    angles.push_back(0.0);
            }
        } // End Y
    }// End X
    
    _db->poiMap.setItem(std::string("sobelContours"), contours);
    _db->valuesMap.setItem(std::string("sobelAngles"),  angles);
}

}   //ALGO