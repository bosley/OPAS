//
//  Stream - Brings in images from a producer at a specified rate (frames per second) and
//           stores them as an IMAGE::Frame with the timestamp that the image was acquired
//

#ifndef IMAGE_STREAM_HPP
#define IMAGE_STREAM_HPP

#include "ImageFrame.hpp"
#include "ImageProducer.hpp"
#include "ImageFrameReceiverIf.hpp"
#include "ImageProcessor.hpp"

#include "bslcore/UtilTickableIf.hpp"

#include <chrono>
#include <ctime>
#include <functional>
#include <vector>


namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Stream
    // --------------------------------------------------------------------
    class Stream : public UTIL::Tickable {
    public:
        static constexpr unsigned DEFAULT_FPS       = 15;

        // Create an image stream given an image producer
        Stream(IMAGE::Producer &producer);

        // Set the frames per second to process. Must be done prior to the first call
        // to tick
        void setFps(unsigned fps);

        // Tick the stream along to begin processing images
        void tick() override;

        // Register receiver to get raw image from producer
        void registerRawFrameReceiver(FrameRecvIf &receiver);

    protected:
        IMAGE::Producer &producer;
        unsigned fps;
        bool started;
        long long interval;

        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;

        std::vector< FrameRecvIf* > rawFrameCallbacks;

        void processOne();
    };
}
#endif // IMAGE_STREAM_HPP
