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
 * File:   VideoSource.cpp
 * Author: josh
 * 
 * Created on November 14, 2018, 9:06 PM
 */

#include "CameraSource.h"
#include "../LOG/aixlog.hpp"

namespace UTIL
{

CameraSource::CameraSource(int cameraSource) : 
                        _cameraSource(cameraSource),
                        _source(nullptr)
{   
}

CameraSource::CameraSource(const CameraSource& orig) 
{
}

CameraSource::~CameraSource() 
{
    delete _source;
}

bool CameraSource::open()
{
    if(_source != nullptr)
    {
        LOG(INFO) << "Video source already defined!";
        return false;
    }
    _source = new cv::VideoCapture();
    _source->open(_cameraSource);
    return false;
}

void CameraSource::close()
{
    _source->release();
    delete _source;
    _source = nullptr;
}

cv::Mat CameraSource::read()
{
    cv::Mat frame;
    if(_source == nullptr)
    {
        LOG(WARNING) << "Video source not yet defined. open() not yet called!";
        return frame;
    }
    
    if (!_source->read(frame))
    {
        LOG(WARNING) << "Unable to read frame from camera: " << _cameraSource;
        return cv::Mat();
    }
    
    return frame;
}

} // UTIL