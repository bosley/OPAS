#ifndef MOD_VISION_H
#define MOD_VISION_H

/*

    Functions to enable interactions with camera hardware
    on the host machine.

*/

#include <chrono>
#include <string>
#include <vector>

#include "modules/types.h"
#include "modules/system/HardwareConfig.h"

#include <opencv2/videoio.hpp>

namespace MOD { namespace VISION
{


//!
//! \brief The ImageArtifact struct
//!        An object or trigger found in an image
//!
struct ImageArtifact
{
    unsigned artifactId;        // ID of the artifact
    cv::Rect areaOfInterest;    // Where the artifact resides
    std::string designation;    // Description of what the artifact is
    MOD::TYPES::ctpoint time;         // Seconds from VisionProcessor::startTime

    ImageArtifact & operator= (const ImageArtifact rhs)
    {
        this->areaOfInterest = rhs.areaOfInterest;
        this->designation = rhs.designation;
        return *this;
    }
};

//!
//! \brief visionBindCamera
//! \param cameraName - Name of the camera
//! \param cameraNumber - Number of the camera to bind
//! \retval true - Camera was bound
//! \retval false - Camera was not bound
//!
bool visionBindCamera(std::string cameraName, int cameraNumber);

//!
//! \brief visionFreeCamera
//! \param cameraName - Name of the camera
//! \retval true - Camera was freed
//! \retval false - Camera was not freed
//!
bool visionFreeCamera(std::string cameraName);

//!
//! \brief visionGetFrame
//! \param cameraName - Name of the camera
//! \param[out] okay - Indicates if frame capture was a success
//! \return cv::Mat - The matrix containing image data
//!
cv::Mat visionGetFrame(std::string cameraName, bool &okay,
                       int width=DEFAULT_VIDEO_DISPLAY_WIDTH,
                       int height=DEFAULT_VIDEO_DISPLAY_HEIGHT);

//!
//! \brief visionResetCamera
//! \param cameraName - Name of the camera
//! \retval true - Camera was reset
//! \retval false - Camera was not reset
//!
bool visionResetCamera(std::string cameraName);

//!
//! \brief visionGetFps
//! \param cameraName - Name of the camera
//! \return FPS as-reported by OpenCV
//!
double visionGetFps(std::string cameraName);

//!
//! \brief visionBindOutputStream
//! \param cameraName - Name of the camera
//! \retval true - Stream was created
//! \retval false - Stream was not created
//!
bool visionBindOutputStream(std::string cameraName, double fps, cv::Size imageSize,
                            std::string directory=DEFAULT_VIDEO_STORAGE_LOCATION);

//!
//! \brief visionWriteToStream
//!        Only one thread should call this function per-camera
//!        to ensure video file correctness.
//! \param cameraName - Name of the camera
//! \param image - The image to write
//! \param artifact - Item in the image
//! \retval true - Data was writen
//! \retval false - Data was not writen
//!
bool visionWriteToStream(std::string cameraName,
                         cv::Mat image,
                         std::vector<ImageArtifact> *artifacts);

//!
//! \brief visionFreeStream
//! \param cameraName - Name of the camera
//! \retval true - Stream was close
//! \retval false - Stream was not created
//!
bool visionFreeStream(std::string cameraName);

} }

#endif // MODVISION_H
