#ifndef IMAGE_OPENCV_VIEWER_H
#define IMAGE_OPENCV_VIEWER_H

#include <string>

#include "ImageFrame.hpp"
#include "ImageFrameReceiverIf.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

namespace IMAGE
{
    class Viewer : public IMAGE::FrameRecvIf {
    public:
        Viewer(std::string title, unsigned size=cv::WINDOW_AUTOSIZE) : title(title) {
            cv::namedWindow(title.c_str(), size); // Create an OpenCV window for display.
        }

        ~Viewer() override {
            cv::destroyWindow(title.c_str());
        }

        virtual void recvImageFrame(IMAGE::Frame frame) override {

            cv::Mat image = frame.getRawMatrixRef();

            if(image.empty()) {
                return;
            }
            cv::imshow(title.c_str(), image);
        }

    private:
        std::string title;
    };
}

#endif // HELPEROPENCVVIEWER_H