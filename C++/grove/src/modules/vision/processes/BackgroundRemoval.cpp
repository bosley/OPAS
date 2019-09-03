#include "BackgroundRemoval.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include <iostream>

namespace MOD { namespace VISION { namespace PROCESSING {

BackgroundRemoval::BackgroundRemoval()
{

}

cv::Mat BackgroundRemoval::process(cv::Mat &frame, bool &okay, std::vector<MOD::VISION::ImageArtifact> *artifacts)
{
    okay = false;

    // This class shouldn't be expected to find anything..
    if(artifacts != nullptr)
    {
        return frame;
    }

    cv::Mat input;
    frame.copyTo(input);

    addToHistory(input);

    cv::Mat avg = historicalAverage(&input);

    if(!avg.empty())
    {
        okay = true;
    }
    return input - avg;
}

void BackgroundRemoval::addToHistory(cv::Mat frame)
{
    history.push_front(frame);

    if(history.size() > GROVE_BACKGROUND_REMOVAL_MAX_HISTORY)
    {
        history.pop_back();
    }
}

cv::Mat BackgroundRemoval::historicalAverage(cv::Mat *frame)
{
    cv::Mat sum(frame->size(), CV_64F, cv::Scalar(0));
    for(auto i = history.begin(); i != history.end(); ++i)
    {
        cv::accumulate((*i), sum);
    }

    cv::Mat avg;
    sum.convertTo(avg, CV_8U, 1.0/static_cast<double>(history.size()));

    return avg;
}

} } }
