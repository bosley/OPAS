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
 * File:   VideoSource.h
 * Author: josh
 *
 * Created on November 14, 2018, 9:06 PM
 */

#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include "ImageSourceIF.h"
#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>

namespace UTIL
{

class CameraSource : public ImageSourceIF
{
public:
    
    //!
    //! \class VideoSource  Defines a video source to pull images from
    //! \param cameraSource Camera number to pull video from
    //! \param imageFlag    CV defined flag for image reading function
    //!
    CameraSource(int    cameraSource);
    CameraSource(const  CameraSource& orig);
    virtual ~CameraSource();
    
    // From ImageSourceIF
    virtual bool open() override;
    virtual void close() override;
    virtual cv::Mat read() override;
private:
    int _cameraSource;
    cv::VideoCapture *_source;
};

}
#endif /* VIDEOSOURCE_H */

