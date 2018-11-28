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
 * File:   Sobel.h
 * Author: josh
 *
 * Created on November 14, 2018, 10:25 PM
 */

#ifndef SOBEL_H
#define SOBEL_H

#include <opencv2/core/core.hpp>

#include <memory>
#include <string>

#include "../IPF/DataBank.h"
#include "../IPF/ProcessableIF.h"
#include "../IPF/StepperIF.h"

#define SOBEL_DEFAULT_THRESH 15

namespace ALGO
{

class Sobel : public IPF::ProcessableIF
{
public:
    Sobel();
    Sobel(const Sobel& orig);
    virtual ~Sobel();
    
    void defineDb(std::shared_ptr< IPF::DataBank > db);
    
    void setThresh(int thresh);
    
    // From ProcessableIF
    virtual void defineSeriesCallback(IPF::StepperIF *) override;
    virtual void process() override;
private:
    IPF::StepperIF * _stepperCallback;
    std::shared_ptr< IPF::DataBank > _db;
    int _thresh;
    
    void runSobel(cv::Mat gray);
};

}
#endif /* SOBEL_H */

