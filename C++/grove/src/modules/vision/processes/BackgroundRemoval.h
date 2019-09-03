#ifndef BACKGROUNDREMOVAL_H
#define BACKGROUNDREMOVAL_H

#include "modules/vision/processes/ProcessIF.h"

#include <deque>

#include <opencv2/core.hpp>

#define GROVE_BACKGROUND_REMOVAL_MAX_HISTORY 25

namespace MOD { namespace VISION { namespace PROCESSING {

/*
        Remove the background from an image that has already been
        processed by Sobel (Or anything with a pixel range of 0-255)

*/
class BackgroundRemoval : public ProcessIF<cv::Mat>
{
public:
    BackgroundRemoval();
    cv::Mat process(cv::Mat &frame, bool &okay, std::vector<MOD::VISION::ImageArtifact> *artifacts);

private:

    std::deque<cv::Mat> history;

    void addToHistory(cv::Mat frame);
    cv::Mat historicalAverage(cv::Mat * frame);
};

} } }

#endif // BACKGROUNDREMOVAL_H
