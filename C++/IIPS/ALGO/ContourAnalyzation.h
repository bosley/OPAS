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
 * File:   ContourAnalyzation.h
 * Author: josh
 *
 * Created on November 15, 2018, 7:53 PM
 */

#ifndef CONTOURANALYZATION_H
#define CONTOURANALYZATION_H

#include <vector>
#include <opencv2/core/core.hpp>

namespace ALGO
{
namespace CONTOURS
{
    //!
    //! \brief From a list of points representing contours, 
    //!        find continuous contours
    //!
    //! \param[in]  contours        Vector of points representing contour points
    //! \param[out] continuous      Vector of continuous 
    //!                             contours represented in vector format 
    //! \param[in]  lengthThreshold Minimum length of continuous contour
    //! \param[in]  maxX            Maximum X coordinate  
    //! \param[in]  maxY            Maximum Y coordinate  
    extern void Continuous(std::vector< cv::Point > contours, 
                                   std::vector< std::vector < cv::Point > > &continuous,
                                   unsigned lengthThreshold,
                                   unsigned maxX,
                                   unsigned maxY);
}
}
#endif /* CONTOURANALYZATION_H */

