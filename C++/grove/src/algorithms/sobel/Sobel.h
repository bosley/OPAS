#ifndef SOBEL_H
#define SOBEL_H

#include <opencv2/imgproc.hpp>

namespace ALGORITHMS {

//!
//! \brief Sobel
//! \param[in] source - Source frame to operate on
//! \param thresh - Minimum pixel value to exceed
//! \param fillDeadSpace - Fill in non-contour points with black
//! \param angles - Angles of contours
//! \param contours - Point list of contours found
//! \retval cv::Mat - Processed sobel image
//!
cv::Mat Sobel(cv::Mat &source,
              int thresh=120,
              bool fillDeadSpace=true,
              std::vector<double>* angles=nullptr,
              std::vector<cv::Point>* contours=nullptr);

}
#endif // SOBEL_H
