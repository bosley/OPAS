#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <atomic>
#include <string>
#include <queue>
#include <opencv2/core.hpp>

#include "modules/bus/Bus.h"
#include "modules/vision/Recorder.h"

#include "modules/vision/processes/BackgroundRemoval.h"

namespace MOD { namespace VISION {

class Processor : public BUS::Subscriber<cv::Mat>
{
public:
    Processor(int givenBusId,
              std::string cameraBusChannel,
              MOD::BUS::Bus<cv::Mat> *dataBus,
              MOD::VISION::Recorder *cameraRecorder);
    ~Processor() override;

    //!
    //! \brief Start
    //!        Start processing
    //!
    void start();

    //!
    //! \brief Shutdown
    //!        Shutdown processing
    //!
    void shutdown();

    //!
    //! \brief isRunning
    //! \retval true - Processor is running
    //! \retval false - Processor is not running
    //!
    bool isRunning();

    // From BUS::Subscriber
    int getId() const override;
    void busNotification(cv::Mat data) override;
    void unsubscribed(std::string publisher) override;

private:
    int busId;
    std::atomic<bool>       running;
    std::string             busChannel;
    MOD::BUS::Bus<cv::Mat> *dataBus;
    std::queue<cv::Mat>     imageQueue;
    cv::Mat                 currentFrame;
    MOD::VISION::Recorder  *recorder;
    std::vector<ImageArtifact> artifacts;
    std::string             localPublisherId;
    bool                    allowedToRun;

    // ------------ [ Processing ] ------------

    MOD::VISION::PROCESSING::BackgroundRemoval bgRemoval;

    //  Scan for potential regions of interest
    //  in an image through image substraction
    void scanForRoi(cv::Mat &sobel);

    void loop();
    void process();
    void store();
};

} }
#endif // PROCESSOR_H
