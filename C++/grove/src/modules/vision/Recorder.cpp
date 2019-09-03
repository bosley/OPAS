#include "Recorder.h"

#include <thread>

namespace MOD { namespace VISION {

Recorder::Recorder(int givenBusId,
                   std::string cameraBusChannel,
                   cv::Size frameSize,
                   MOD::BUS::Bus<cv::Mat> *cameraBus) :
                        busId(givenBusId),
                        running(false),
                        busChannel(cameraBusChannel),
                        cameraBus(cameraBus)

{
    if(cameraBus != nullptr)
    {
        cameraBus->newSubscriber(this->busChannel, this);

        visionBindOutputStream(busChannel,
                               visionGetFps(cameraBusChannel),
                               frameSize);
    }
}

Recorder::~Recorder()
{
}

void Recorder::start()
{
    if(this->running)
    {
        return;
    }

    this->running = true;

    loop();
}

void Recorder::shutdown()
{
    cameraBus->unsubscribe(busChannel, this);
    this->running = false;
}

bool Recorder::isRunning()
{
    return this->running;
}


void Recorder::addArtifacts(std::vector<ImageArtifact> artifacts)
{
    artifactQueue.push(artifacts);
}

int Recorder::getId() const
{
    return this->busId;
}

void Recorder::busNotification(cv::Mat frame)
{
    if(!frame.empty())
    {
        imageQueue.push(frame);
    }
}

void Recorder::unsubscribed(std::string publisher)
{
    std::cout << "Recorder["
              << this->busId
              << "]::unsubscribed("
              << publisher
              << ") -> Stopping thread"
              << std::endl;
    this->running = false;
    std::this_thread::sleep_for(
                        std::chrono::microseconds(100)
                        );
}

void Recorder::loop()
{
    while(this->running)
    {
        if(!imageQueue.empty())
        {
            if(!artifactQueue.empty())
            {
                std::cout << "Recorder::loop() -> recording artifacts" << std::endl;

                visionWriteToStream(busChannel, imageQueue.front(), &artifactQueue.front());
                artifactQueue.pop();
            }
            else
            {
                visionWriteToStream(busChannel, imageQueue.front(), nullptr);
            }
            imageQueue.pop();
        }

        std::this_thread::sleep_for(
                            std::chrono::microseconds(1)
                            );
    }
}

} }
