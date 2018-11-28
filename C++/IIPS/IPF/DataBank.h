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
 * File:   DataBank.h
 * Author: josh
 *
 * Created on November 15, 2018, 5:31 PM
 */

#ifndef DATABANK_H
#define DATABANK_H

#include <map>
#include <string>
#include <vector>

#include "DataStore.h"

#include <opencv2/core/core.hpp>

namespace IPF
{
    struct DataBank
    {
        DataStore< cv::Mat > imageMap;                  // Image storage
        DataStore< std::vector< cv::Point> > poiMap;    // Points of interest
        DataStore< std::vector< float > > valuesMap;    // Values calculated
    };
}


#endif /* DATABANK_H */

