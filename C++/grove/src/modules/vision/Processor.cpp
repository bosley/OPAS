#include "Processor.h"

#include "Vision.h"

#include <thread>
#include <iostream>
#include <string>

#include <opencv2/imgcodecs.hpp>
#include "algorithms/sobel/Sobel.h"

namespace MOD { namespace VISION {

Processor::Processor(int givenBusId,
                     std::string cameraBusChannel,
                     MOD::BUS::Bus<cv::Mat> *givenDataBus,
                     MOD::VISION::Recorder  *cameraRecorder) :
                                                          busId(givenBusId),
                                                          running(false),
                                                          busChannel(cameraBusChannel),
                                                          dataBus(givenDataBus),
                                                          recorder(cameraRecorder),
                                                          localPublisherId(std::to_string(givenBusId)),
                                                          allowedToRun(false)
{
    if(dataBus != nullptr)
    {
        dataBus->newSubscriber(this->busChannel, this);
        dataBus->newPublisher(localPublisherId);
        allowedToRun = true;
    }
}

Processor::~Processor()
{

}

void Processor::start()
{
    if(!allowedToRun)
    {
        std::cout << "Processor failed check 'allowedToRun' - Will not start" << std::endl;
        return;
    }

    if(this->running)
    {
        return;
    }

    this->running = true;

    loop();
}

void Processor::shutdown()
{
    dataBus->unsubscribe(busChannel, this);
    this->running = false;
}

bool Processor::isRunning()
{
    return this->running;
}

int Processor::getId() const
{
    return this->busId;
}

void Processor::busNotification(cv::Mat frame)
{
    if(!frame.empty())
    {
        imageQueue.push(frame);
    }
}

void Processor::unsubscribed(std::string publisher)
{
    std::cout << "Processor["
              << this->busId
              << "]::unsubscribed("
              << publisher
              << ") -> Stopping thread"
              << std::endl;
    this->running = false;
}

void Processor::loop()
{
    while(this->running)
    {
        std::this_thread::sleep_for(
                            std::chrono::microseconds(1)
                            );
        if(!imageQueue.empty())
        {
            imageQueue.front().copyTo(currentFrame);
            imageQueue.pop();
        }

        if(!currentFrame.empty())
        {
            process();
        }

    }
}

void Processor::scanForRoi(cv::Mat &sobel)
{
    bool okay = false;
    cv::Mat bgRemovalMat = bgRemoval.process(sobel, okay, nullptr);

    // Items in bgRemoval mat have a higher prob. of being something
    // of interest.

    // Scan the image here to find something interesting!!!

    // If interesting regions appear, mark as a list of cv::Rects to send to
    // identification methods
}

void Processor::process()
{
    // Get an edge detected frame using sobel
    cv::Mat sobelImage = ALGORITHMS::Sobel(currentFrame);


    // Send something back for the processor driver
    dataBus->publish(localPublisherId, sobelImage);

    // Try to find interesting areas through movements
    scanForRoi(sobelImage);





    // Store found information
    store();
}

void Processor::store()
{
    if(artifacts.size() > 0)
    {
        if(recorder != nullptr)
        {
            recorder->addArtifacts(artifacts);
        }

        /*
        if(activeMemory != nullptr)
        {
            activeMemory->addArtifacts(artifacts);
        }
        */

        artifacts.clear();
    }
}

} }
