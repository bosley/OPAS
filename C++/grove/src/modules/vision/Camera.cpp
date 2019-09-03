#include "Camera.h"
#include "Vision.h"

#include <thread>

namespace MOD { namespace VISION {


Camera::Camera(std::string cameraName, int devceNumber, MOD::BUS::Bus<cv::Mat> *bus) :
                                                           running(false),
                                                           cameraNumber(devceNumber),
                                                           cameraName(cameraName),
                                                           currentFrameOkay(false),
                                                           currentFPS(0.0)
{
    MOD::VISION::visionBindCamera(cameraName, cameraNumber);

    currentFrame = MOD::VISION::visionGetFrame(cameraName, currentFrameOkay);
    currentFPS = MOD::VISION::visionGetFps(cameraName);

    MOD::VISION::visionBindOutputStream(cameraName,
                                currentFPS,
                                currentFrame.size());

    startTime = std::chrono::system_clock::now();

    if(nullptr != bus)
    {
        cameraBus = bus;
        cameraBus->newPublisher(this->cameraName);
    }
}

void Camera::start()
{
    if(this->running)
    {
        return;
    }

    this->running = true;

    loop();
}

void Camera::shutdown()
{
    this->running = false;
}

bool Camera::isRunning()
{
    return this->running;
}

double Camera::getFps()
{
    return this->currentFPS;
}

void Camera::loop()
{
    while(running)
    {
        if(nullptr != cameraBus)
        {
            currentFrame = MOD::VISION::visionGetFrame(cameraName, currentFrameOkay);

            if(currentFrameOkay)
            {
                cameraBus->publish(this->cameraName, currentFrame);
            }
        }

        std::this_thread::sleep_for(
                            std::chrono::nanoseconds(10)
                            );
    }
}

} }
