#ifndef CVMATDISPLAYDRIVER_H
#define CVMATDISPLAYDRIVER_H

#include <atomic>
#include <mutex>
#include <queue>
#include "gui/widgets/video/WidgetVideoDisplay.h"
#include "modules/bus/Bus.h"
namespace GUI {

class CVMatDisplayDriver : public MOD::BUS::Subscriber<cv::Mat>
{
public:
    CVMatDisplayDriver(int givenBusId, std::string cameraBusChannel, MOD::BUS::Bus<cv::Mat> *cameraBus, double cameraFps);
    ~CVMatDisplayDriver() override;

    void setDisplayWidget(VideoDisplay *widget);

    bool start();

    void shutdown();

    bool isRunning();

    std::string getCameraName() const;

    // From BUS::Subscriber
    int getId() const override;
    void busNotification(cv::Mat data) override;
    void unsubscribed(std::string publisher) override;

private:
    int                          busId;
    std::atomic<bool>            running;
    VideoDisplay                *widget;
    MOD::BUS::Bus<cv::Mat>      *bus;
    std::string                  busChannel;
    double                       fps;
    std::queue<cv::Mat>          frameQueue;
    std::mutex                   driverMutex;
    cv::Mat lastFrame;
    void loop();
};

}

#endif // CVMATDISPLAYDRIVER_H
