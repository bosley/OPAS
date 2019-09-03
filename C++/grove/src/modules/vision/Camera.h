#ifndef CAMERA_H
#define CAMERA_H

#include <atomic>
#include <string>
#include <opencv2/videoio.hpp>

#include "modules/bus/Bus.h"
#include "modules/types.h"

namespace MOD { namespace VISION {

class Camera
{
public:
    Camera(std::string cameraName, int devceNumber, MOD::BUS::Bus<cv::Mat> *bus);

    //!
    //! \brief start
    //!        Start the camera
    //!
    void start();

    //!
    //! \brief shutdown
    //!        Shutdown the camera
    //!
    void shutdown();

    //!
    //! \brief isRunning
    //!
    //! \retval true Camera is running
    //! \retval false Camera is not running
    //!
    bool isRunning();

    //!
    //! \brief getFps
    //! \return Frames per second
    //!
    double getFps();

private:
    std::atomic<bool>   running;
    int                 cameraNumber;
    std::string         cameraName;
    bool                currentFrameOkay;
    double              currentFPS;
    cv::VideoCapture    videoCap;
    cv::Mat             currentFrame;
    MOD::TYPES::ctpoint startTime;
    MOD::BUS::Bus<cv::Mat> *cameraBus;

    void loop();
};

} }
#endif // CAMERA_H
