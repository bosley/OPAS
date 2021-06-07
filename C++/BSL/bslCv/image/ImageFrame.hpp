//
//  Frame - Simple wrapper for an image matrix to hold image data
//

#ifndef IMAGE_FRAME_H
#define IMAGE_FRAME_H

#include <ctime>
#include <string>

#include <opencv2/opencv.hpp>

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Image frame
    // --------------------------------------------------------------------
    class Frame
    {
    public:

        // Create a basic frame
        Frame();

        // Create a frame given a matrix
        Frame(cv::Mat matrix);

        // Get a reference to the matrix ref
        cv::Mat& getRawMatrixRef();

        // Get a copy of the matrix
        cv::Mat getMatrix();

        // Set the timestamp
        void setTimeStamp();

        // Get string version of the timestamp
        std::string getTimeStamp() const;

        // Set the matrix to something
        void setMatrix(cv::Mat matrix);

    private:
        cv::Mat     matrix;
        std::string timestamp;
    };
}

#endif // IMAGEFRAME_H
