#ifndef PROCESS_H
#define PROCESS_H

#include "modules/vision/Vision.h"

#include <opencv2/core.hpp>
#include <vector>

namespace MOD { namespace VISION { namespace PROCESSING {

//!
//! \brief The ProcessIF class
//!        This class is used to create processes that will be ran by the processor
//!        Each process is ran with an attempt to find artifacts within an image, or
//!        to edit an image in a particular manner using algorithms found in algorithms/
//!
template<class T>
class ProcessIF
{
public:
    virtual ~ProcessIF() = default;
    virtual T process(cv::Mat &frame, bool &okay, std::vector<ImageArtifact> * artifacts) = 0;
};

} } }
#endif // PROCESS_H
