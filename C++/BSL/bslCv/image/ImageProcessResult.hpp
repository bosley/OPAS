//
//  ProcessResult - Used to store the result of an image process.
//                  Uses a DataType set to indicate what types of data within the result
//                  have been populated. This might be refactored for better general-use.
//

// ---------------------------------------------------------------------------------
// TODO: Need to add a means to store found artifacts, and add the methods
//       to get/set them
// ---------------------------------------------------------------------------------

#ifndef IMAGE_PROCESSRESULT_HPP
#define IMAGE_PROCESSRESULT_HPP

#include "ImageFrame.hpp"
#include "ImageArtifacts.hpp"

#include <opencv2/opencv.hpp>
#include <set>

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  ProcessResult
    // --------------------------------------------------------------------
    class ProcessResult
    {
    public:

        // Types of data that might come from a result
        enum class DataType {
            IMAGE,
            ARTIFACTS
        };

        // Create the ProcessResult given a flag to mark the intended data
        // type stored within the object, and an image frame to see what the
        // process was run against
        ProcessResult(DataType resultDataType, IMAGE::Frame originFrame);

        // Insert DataType flags into the set which indicates what type
        // of data the ProcessResult contains
        void addExtraDataType(DataType dataType);

        // If the processTypes set contains the IMAGE DataType,
        // this method will set the stored matrix as the input matrix
        void setMatrix(cv::Mat matrix);

        // Get the matrix
        cv::Mat getMatrix() const;

        // Get the matrix as a frame - frame timestamp will be that of when this
        // method is called
        IMAGE::Frame getFrame() const;

        // Get the origin frame - Original frame data (image, timestamp, etc)
        IMAGE::Frame getOriginFrame() const;

        // Add a region of interest
        void addRegionOfInterest(std::string regionName, cv::Point point1, cv::Point point2);

        // Get the regions of interest
        std::vector<IMAGE::ARTIFACT::Rectangle> getRegionsOfInterest() const;

    private:
        std::set<DataType> processTypes;
        cv::Mat matrix;
        IMAGE::Frame originFrame;
        std::vector<IMAGE::ARTIFACT::Rectangle> roi;
    };
}

#endif // IMAGEPROCESSRESULT_HPP
