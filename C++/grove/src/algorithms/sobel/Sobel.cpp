#include "Sobel.h"

namespace ALGORITHMS {

/*

 Matrix Operations

 GX             GY
    1, 0,-1,        1, 2, 1,
    2, 0,-2,        0, 0, 0,
    1, 0,-1        -1,-2,-1

*/

cv::Mat Sobel(cv::Mat &source,
              int thresh,
              bool fillDeadSpace,
              std::vector<double>* angles,
              std::vector<cv::Point>* contours)
{
    cv::Mat gray;
    cv::Mat destination;
    cv::cvtColor(source, gray, CV_BGR2GRAY);
    gray.copyTo(destination);

    if(nullptr != contours)
    {
        contours->clear();
    }

    for(int x = 1; x < gray.rows-2; x++)
    {
        for(int y = 1; y < gray.cols-2; y++)
        {
            // Bottom
            short b0 = static_cast<short> (gray.at<uchar>(x-1,y-1));
            short b1 = static_cast<short> (gray.at<uchar>(x,y-1));
            short b2 = static_cast<short> (gray.at<uchar>(x+1,y-1));

            // Middle
            short m0 = static_cast<short> (gray.at<uchar>(x-1,y));
            // Middle value (x,y) always multd by 0
            short m2 = static_cast<short> (gray.at<uchar>(x+1,y));

            // Top
            short t0 = static_cast<short> (gray.at<uchar>(x-1,y+1));
            short t1 = static_cast<short> (gray.at<uchar>(x,y+1));
            short t2 = static_cast<short> (gray.at<uchar>(x+1,y+1));

            // Reduced from matrix operations
            int px = b0 + (-b2) + (2 * m0) + (-2 * m2) + t0 + (-t2);
            int py = b0 + (2 * b1) + b2 + (-t0) + (-2 * t1) + (-t2);

            int value = static_cast<int>(std::ceil(std::sqrt((px*px) + (py*py))));
            if(value > 255)
            {
                value = 255;
            }

            if(value >= thresh)
            {
                if(nullptr != contours)
                {
                    contours->push_back(cv::Point(x,y));
                }

                if(nullptr != angles)
                {
                    if(px != 0)
                    {
                        angles->push_back( std::atan(py/px) );
                    }
                    else
                    {
                        angles->push_back(0.0);
                    }
                }

                destination.at<uchar>(x,y) = static_cast<uchar>(value);
            }
            else if (fillDeadSpace)
            {
                destination.at<uchar>(x,y) = 0;
            }
        } // End Y
    }// End X

    return destination;
}

}
