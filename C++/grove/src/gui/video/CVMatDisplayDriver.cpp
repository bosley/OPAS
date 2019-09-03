#include "CVMatDisplayDriver.h"
#include <thread>

#include <iostream>

namespace GUI {

CVMatDisplayDriver::CVMatDisplayDriver(int givenBusId,
                                       std::string cameraBusChannel,
                                       MOD::BUS::Bus<cv::Mat> *cameraBus,
                                       double cameraFps) :
                                           busId(givenBusId),
                                           running(false),
                                           widget(nullptr),
                                           bus(cameraBus),
                                           busChannel(cameraBusChannel),
                                           fps(cameraFps)

{
    std::cout << "CVMatDisplayDriver() -> Camera @ " << fps << " fps" << std::endl;
}

CVMatDisplayDriver::~CVMatDisplayDriver()
{
    if(bus != nullptr)
    {
        bus->unsubscribe(this->busChannel, this);
    }
}

void CVMatDisplayDriver::setDisplayWidget(VideoDisplay *widget)
{
    if(nullptr != widget)
    {
        this->widget = widget;
    }
}

bool CVMatDisplayDriver::start()
{
    if(widget == nullptr)
    {
        return false;
    }

    if(this->running)
    {
        return false;
    }

    this->running = true;

    if(bus != nullptr)
    {
        bus->newSubscriber(this->busChannel, this);
    }

    loop();

    return true;
}

void CVMatDisplayDriver::shutdown()
{
    bus->unsubscribe(busChannel, this);
    this->running = false;
}

bool CVMatDisplayDriver::isRunning()
{
    return this->running;
}


std::string CVMatDisplayDriver::getCameraName() const
{
    return this->busChannel;
}

int CVMatDisplayDriver::getId() const
{
    return this->busId;
}

void CVMatDisplayDriver::busNotification(cv::Mat frame)
{
    if(!this->running)
    {
        return;
    }

    if(!frame.empty())
    {
        frameQueue.push(frame);
    }
}

void CVMatDisplayDriver::unsubscribed(std::string publisher)
{
    std::cout << "CVMatDisplayDriver["
              << this->busId
              << "]::unsubscribed("
              << publisher
              << ") -> Stopping thread"
              << std::endl;
    this->running = false;
}

void CVMatDisplayDriver::loop()
{
    while(this->running)
    {
        if(!frameQueue.empty())
        {
            cv::Mat frame;
            frameQueue.front().copyTo(frame);
            widget->imageReady(frame);
            frameQueue.pop();
        }

        std::this_thread::sleep_for(
                            std::chrono::microseconds(1)
                            );
    }
}

}
