#include "ImageStream.hpp"

#include <iostream>

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Stream
    // --------------------------------------------------------------------
    Stream::Stream(IMAGE::Producer &producer): producer(producer),
                                               fps(DEFAULT_FPS),
                                               started(false)
                                               {

    }

    // --------------------------------------------------------------------
    //  setFps
    // --------------------------------------------------------------------
    void Stream::setFps(unsigned fps) {
        this->fps = fps;
    }

    // --------------------------------------------------------------------
    //  tick
    // --------------------------------------------------------------------
    void Stream::tick() {
        if(!started)  {
            producer.open();

            if (producer.isReady()) {
                start = std::chrono::high_resolution_clock::now();
                interval = 1000/fps;
                started = true;
            }
            return;
        }

        end = std::chrono::high_resolution_clock::now();

        long long passed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        if (passed >= interval) {
            start = std::chrono::high_resolution_clock::now();
            processOne();
        }
    }

    void Stream::registerRawFrameReceiver(FrameRecvIf &receiver)
    {
        rawFrameCallbacks.push_back(&receiver);
    }

    // --------------------------------------------------------------------
    //  processOne
    // --------------------------------------------------------------------
    void Stream::processOne() {

        IMAGE::Frame imgFrame;

        // Get the frame
        producer.getFrame(imgFrame.getRawMatrixRef());
        imgFrame.setTimeStamp();

        // Send frame to any frame receivers
        for(auto i = rawFrameCallbacks.begin(); i != rawFrameCallbacks.end(); ++i) {
            (*i)->recvImageFrame(imgFrame);
        }
    }
}
