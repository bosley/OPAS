//
//  Image producers - Types of inputs that can produce OpenCV mats
//                    Each producer is given its own methods for opening and creation
//                    while they are being developed to handle the specifics that may arise.
//                    If more generalities are found, this should be refactored.
//
#ifndef IMAGE_PRODUCER_HPP
#define IMAGE_PRODUCER_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <string>

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Producer Types
    // --------------------------------------------------------------------
    enum class ProducerType {
        PT_VideoFile,
        PT_VideoCameraLocal,
        PT_VideoCameraRemote,
        PT_ImageFile,
    };

    // --------------------------------------------------------------------
    //  Producer
    // --------------------------------------------------------------------
    class Producer {
    public:
        static Producer* Create(ProducerType type, std::string source);
        virtual ~Producer(){}

        virtual void open() = 0;

        void close() {
            this->cvCapture.release();
        }

        bool isReady(){
            return this->cvCapture.isOpened();
        }
        void getFrame(cv::Mat &frame) {
            this->cvCapture >> frame;
        }

    protected:
        cv::VideoCapture cvCapture;
        std::string      source;
    };

    // --------------------------------------------------------------------
    //  VideoCameraLocal
    // --------------------------------------------------------------------
    class VideoCameraLocal : public Producer {
    public:

        VideoCameraLocal(std::string cameraSource) {
            this->source = cameraSource;
        }

        void open()  {

            if(this->cvCapture.isOpened()) {
                return;
            }

            int cameraNumber = 0;
            std::istringstream(this->source) >> cameraNumber;

            if (cameraNumber != INT_MAX && cameraNumber != INT_MIN){
                this->cvCapture.open(cameraNumber);
            } else {
                // Log here that an error occured
            }
        }

    };

    // --------------------------------------------------------------------
    //  VideoFile
    // --------------------------------------------------------------------
    class VideoFile : public Producer {
    public:

        VideoFile(std::string file) {
            this->source = file;
        }

        void open() {
            if(this->cvCapture.isOpened()) {
                return;
            }
            this->cvCapture.open(this->source);
        }
    };

    // --------------------------------------------------------------------
    //  VideoCameraRemote
    // --------------------------------------------------------------------
    class VideoCameraRemote : public Producer {
    public:

        VideoCameraRemote(std::string cameraSource) {
            this->source = cameraSource;
        }

        void open() {
            if(this->cvCapture.isOpened()) {
                return;
            }
            this->cvCapture.open(this->source);
        }

    };

    // --------------------------------------------------------------------
    //  ImageFile
    // --------------------------------------------------------------------
    class ImageFile : public Producer {
    public:

        ImageFile(std::string file) {
            this->source = file;
        }

        void open() {
            if(this->cvCapture.isOpened()) {
                return;
            }
            this->cvCapture.open(this->source);
        }

    };

}

#endif // IMAGE_PRODUCER_IF_HPP
