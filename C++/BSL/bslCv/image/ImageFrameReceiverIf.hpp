//
//  FrameRecvIf - An interface for classes who want to receive an image frame from
//                a stream or from a process result.
//
#ifndef IMAGE_FRAMERECVIF_H
#define IMAGE_FRAMERECVIF_H

#include "ImageFrame.hpp"

namespace IMAGE
{
    class FrameRecvIf {
    public:
        virtual ~FrameRecvIf() {}
        virtual void recvImageFrame(IMAGE::Frame frame) = 0;
    };
}

#endif // IMAGEFRAMERECVIF_H
