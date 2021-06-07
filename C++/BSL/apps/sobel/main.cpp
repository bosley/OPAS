#include "bslcv/AlgoSobel.hpp"
#include "bslcv/ImageProcessIf.hpp"
#include "bslcv/ImageProcessor.hpp"
#include "bslcv/ImageStream.hpp"
#include "bslcv/ImageViewer.hpp"
#include "bslcore/UtilLogger.hpp"

#include <iostream>
#include <string>

namespace {

    //
    //      A class that is used to stuff into a processor and have the image come 
    //      in a viewer (if given a view) or it will be stuffed into a memory bank
    //
    class ProcessSobel : public IMAGE::ProcessIf {
        
    public:
        // Construct
        ProcessSobel(std::string name, unsigned id, IMAGE::FrameRecvIf* viewer) :
            ProcessIf(name, id, viewer) {}

        // Process the image when called on by a processor
        IMAGE::ProcessResult process(IMAGE::Frame &frameIn) override {
            
            IMAGE::ProcessResult result(IMAGE::ProcessResult::DataType::IMAGE, frameIn);

            if(this->processViewer) {

                cv::Mat sobelFrame = sobelGenerator.process(frameIn.getRawMatrixRef());

                if(!sobelFrame.empty()) {
                    this->processViewer->recvImageFrame(IMAGE::Frame(sobelFrame));
                } else {
                    std::cout << "Empty frame :(" << std::endl;
                }
            }
            return result;
        }

    private:
        unsigned sobelThresh;
        bool sobelFillDeadSpace;
        ALGO::Sobel sobelGenerator;
    };
}


#define CAMERA_SOURCE "0" // String because most things in real life will prol be a path to a far-away system

int main(int argc, char** argv)
{
    UTIL::Logger logger("SobelLogger");

    logger.setLevel(LL_INFO);

    LOG_DEBUG(logger) << "Starting sobel app\n";

    // Setup producer and stream
    IMAGE::Producer* prod = IMAGE::Producer::Create(IMAGE::ProducerType::PT_VideoCameraLocal, CAMERA_SOURCE);

    IMAGE::Stream stream(*prod);

    // Create a receiver to view the raw images from the producer
    IMAGE::Viewer * sobelView = new IMAGE::Viewer(std::string("Sobel stream view"));

    // Create the process and give it a viewer
    ProcessSobel sobelProcess("sobel", 0, sobelView);

    // Create an image processor
    IMAGE::Processor * basicProcessor = new IMAGE::Processor(std::string("ViewerDemo"), 0);

    // Register the process with the processor
    basicProcessor->registerProcess(sobelProcess);
    
    stream.setFps(60);
    stream.registerRawFrameReceiver(*basicProcessor);
    //stream.registerRawFrameReceiver(*sobelView);

    while(1) {
        stream.tick();

        char c = static_cast<char>(cv::waitKey(25));
        if (c == 27)
            break;
    }

    delete basicProcessor;
    delete sobelView;
    delete prod;
    return 0;
}