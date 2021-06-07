#include "ImageProcessResult.hpp"

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  ProcessResult
    // --------------------------------------------------------------------
    ProcessResult::ProcessResult(DataType resultDataType, IMAGE::Frame frame) {
        processTypes.insert(resultDataType);
        originFrame = frame;
    }

    // --------------------------------------------------------------------
    //  addExtraDataType
    // --------------------------------------------------------------------
    void ProcessResult::addExtraDataType(ProcessResult::DataType dataType) {
        processTypes.insert(dataType);
    }

    // --------------------------------------------------------------------
    //  setMatrix
    // --------------------------------------------------------------------
    void ProcessResult::setMatrix(cv::Mat matrix) {

        if (processTypes.count(DataType::IMAGE)) {
            matrix.copyTo(this->matrix);
        }
    }

    // --------------------------------------------------------------------
    //  getMatrix
    // --------------------------------------------------------------------
    cv::Mat ProcessResult::getMatrix() const {
        return matrix;
    }

    // --------------------------------------------------------------------
    //  getFrame
    // --------------------------------------------------------------------
    Frame ProcessResult::getFrame() const {
        return IMAGE::Frame(matrix);
    }

    // --------------------------------------------------------------------
    //  getFrame
    // --------------------------------------------------------------------
    Frame ProcessResult::getOriginFrame() const {
        return originFrame;
    }

    // --------------------------------------------------------------------
    //  addRegionOfInterest
    // --------------------------------------------------------------------
    void ProcessResult::addRegionOfInterest(std::string regionName, cv::Point point1, cv::Point point2){
        roi.push_back(
           IMAGE::ARTIFACT::Rectangle(
                        regionName,
                        point1,
                        point2,
                        cv::Scalar(255, 255, 255),
                        5
                        )
                    );
    }

    // --------------------------------------------------------------------
    //  getRegionsOfInterest
    // --------------------------------------------------------------------
    std::vector<ARTIFACT::Rectangle> ProcessResult::getRegionsOfInterest() const {
        return roi;
    }
}
