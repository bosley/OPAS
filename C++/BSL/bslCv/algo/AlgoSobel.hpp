#ifndef BSLCV_PROCESSSOBEL_H
#define BSLCV_PROCESSSOBEL_H

#include <opencv2/opencv.hpp>

namespace ALGO {
    //!
    //! \class Sobel
    //! \brief Runs the sobel algo on a matrix and returns the resulting matrix
    //!
    class Sobel {
        
    public:
        static constexpr unsigned  DEFAULT_SOBEL_THRESH    = 120; 
        static constexpr bool      DEFAULT_FILL_DEAD_SPACE = true;

        //!
        //! \brief Construct the object
        //! \post Settings for algo will be the default values 
        //!
        Sobel();

        //!
        //! \brief Sets the contour threshold
        //! \param thresh Threshold to set [+0, +255] 
        //!
        void setThresh(unsigned thresh);

        //!
        //! \brief Enable the filling of dead space
        //! \param fill Sets the flag to determine if the object will color-in black on 
        //!             non-contour pixels
        //!
        void setFillDeadspace(bool fill);

        //!
        //! \brief Process a frame and get the resulting frame
        //! \param original The rame to run the sobel algo on
        //! \returns The resulting matrix
        //!
        cv::Mat process(cv::Mat &original);

    private:
        unsigned sobelThresh;
        bool sobelFillDeadSpace;
    };

}

#endif // APEX_PROCESS_SOBEL_H