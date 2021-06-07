#include "bslcv/ImageProcessor.hpp"
#include "bslcv/ImageStream.hpp"
#include "bslcv/ImageViewer.hpp"

#include <iostream>
#include <string>


#define CAMERA_SOURCE "0" // String because most things in real life will prol be a path to a far-away system

int main(int argc, char** argv)
{
    // Setup producer and stream
    IMAGE::Producer* prod = IMAGE::Producer::Create(IMAGE::ProducerType::PT_VideoCameraLocal, CAMERA_SOURCE);

    IMAGE::Stream stream(*prod);

    // Create a receiver to view the raw images from the producer
    IMAGE::Viewer * streamView = new IMAGE::Viewer(std::string("Raw stream view"), 120);

    // Create an image processor
    IMAGE::Processor * basicProcessor = new IMAGE::Processor(std::string("ViewerDemo"), 0);

    stream.setFps(60);
    stream.registerRawFrameReceiver(*streamView);
    stream.registerRawFrameReceiver(*basicProcessor);

    while(1) {
        stream.tick();

        char c = static_cast<char>(cv::waitKey(25));
        if (c == 27)
            break;
    }

    delete basicProcessor;
    delete streamView;
    delete prod;
    return 0;
}