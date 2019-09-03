#ifndef RECORDER_H
#define RECORDER_H

#include "modules/bus/Bus.h"
#include "Vision.h"
#include <opencv2/core.hpp>
#include <queue>

namespace MOD { namespace VISION {

class Recorder : public BUS::Subscriber<cv::Mat>
{
public:
    Recorder(int givenBusId,
             std::string cameraBusChannel,
             cv::Size frameSize,
             MOD::BUS::Bus<cv::Mat> *cameraBus);
    ~Recorder() override;

    //!
    //! \brief Start
    //!        Start Recorder
    //!
    void start();

    //!
    //! \brief Shutdown
    //!        Shutdown Recorder
    //!
    void shutdown();

    //!
    //! \brief isRunning
    //! \retval true - Recorder is running
    //! \retval false - Recorder is not running
    //!
    bool isRunning();

    //!
    //! \brief addArtifacts
    //! \param artifacts - List of artifacts to write for stream
    //!                    (Called from processor)
    //!
    void addArtifacts(std::vector<ImageArtifact> artifacts);

    // From BUS::Subscriber
    int getId() const override;
    void busNotification(cv::Mat data) override;
    void unsubscribed(std::string publisher) override;

private:
    int busId;
    std::atomic<bool>       running;
    std::string             busChannel;
    MOD::BUS::Bus<cv::Mat> *cameraBus;
    std::queue<cv::Mat>     imageQueue;
    std::queue<std::vector<ImageArtifact>> artifactQueue;

    void loop();
};

} }
#endif // RECORDER_H
