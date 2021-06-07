
#include "AlgoSobel.hpp"
#include <opencv2/imgproc/imgproc.hpp>

/*
 GX             GY
    1, 0,-1,        1, 2, 1,
    2, 0,-2,        0, 0, 0,
    1, 0,-1        -1,-2,-1
*/

namespace ALGO {

    // --------------------------------------------------------------------
    //  Sobel
    // --------------------------------------------------------------------
    Sobel::Sobel() :
        sobelThresh(DEFAULT_SOBEL_THRESH),
        sobelFillDeadSpace(DEFAULT_FILL_DEAD_SPACE)
    {

    }

    // --------------------------------------------------------------------
    //  setThresh
    // --------------------------------------------------------------------
    void Sobel::setThresh(unsigned thresh)
    {
        sobelThresh = thresh;
    }

    // --------------------------------------------------------------------
    //  setFillDeadspace
    // --------------------------------------------------------------------
    void Sobel::setFillDeadspace(bool fill)
    {
        sobelFillDeadSpace = fill;
    }

    // --------------------------------------------------------------------
    //  ProcessResult
    // --------------------------------------------------------------------
    cv::Mat Sobel::process(cv::Mat &frameIn) {

        // Grayscale the image
        cv::Mat frame; frameIn.copyTo(frame);
        cv::Mat gray; cv::Mat resultMatrix;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        gray.copyTo(resultMatrix);

        for(int x = 1; x < gray.rows-2; x++)
        {
            for(int y = 1; y < gray.cols-2; y++)
            {
                // Bottom

                short b0 = static_cast<short>(gray.at<uchar>(x-1,y-1));
                short b1 = static_cast<short>(gray.at<uchar>(x,y-1));
                short b2 = static_cast<short>(gray.at<uchar>(x+1,y-1));

                // Middle
                short m0 = static_cast<short>(gray.at<uchar>(x-1,y));
                // Middle value (x,y) always multd by 0
                short m2 = static_cast<short>(gray.at<uchar>(x+1,y));

                // Top
                short t0 = static_cast<short>(gray.at<uchar>(x-1,y+1));
                short t1 = static_cast<short>(gray.at<uchar>(x,y+1));
                short t2 = static_cast<short>(gray.at<uchar>(x+1,y+1));

                // Reduced from matrix operations
                int px = b0 + (-b2) + (2 * m0) + (-2 * m2) + t0 + (-t2);
                int py = b0 + (2 * b1) + b2 + (-t0) + (-2 * t1) + (-t2);

                int value = static_cast<int>(std::ceil(std::sqrt((px*px) + (py*py))));
                if(value > 255)
                    value = 255;

                if(value >= static_cast<int>(sobelThresh))
                {
                    resultMatrix.at<uchar>(x,y) =  static_cast<uchar>(value);
                }
                else if (sobelFillDeadSpace)
                {
                    resultMatrix.at<uchar>(x,y) = 0;
                }
            } // End Y
        }// End X

        return resultMatrix;
    }
}