#include "gui/MainWindow.h"
#include <QApplication>

#include <thread>

#include "modules/bus/Bus.h"
#include "modules/vision/Vision.h"
#include "modules/vision/Camera.h"
#include "modules/vision/Processor.h"
#include "modules/vision/Recorder.h"

#include "gui/video/CVMatDisplayDriver.h"

void startCamera(MOD::VISION::Camera *cam)
{
    cam->start();
}

void startProcessor(MOD::VISION::Processor *processor)
{
    processor->start();
}

void startRecorder(MOD::VISION::Recorder *recorder)
{
    recorder->start();
}

int main(int argc, char *argv[])
{
    cv::setUseOptimized(true);
    cv::setNumThreads(5);

    // setup

    // Camera bus
    MOD::BUS::Bus<cv::Mat> dataBus;

    // Camera 0
    MOD::VISION::Camera primaryCamera(
                "PrimaryCamera",
                0,
                &dataBus);

    // Recorder for camera 0
    MOD::VISION::Recorder primaryRecorder(
                BUS_SUB_ID_0,
                "PrimaryCamera",
                cv::Size(DEFAULT_VIDEO_DISPLAY_WIDTH, DEFAULT_VIDEO_DISPLAY_HEIGHT),
                &dataBus
                );

    // Processor for camera 0
    MOD::VISION::Processor primaryProcessor(
                BUS_SUB_ID_1,
                "PrimaryCamera",
                &dataBus,
                &primaryRecorder);

    // USE: Vieiwing un-edited camera output


    std::vector<GUI::CVMatDisplayDriver*> drivers;

    // Display driver for camera 0
    GUI::CVMatDisplayDriver primaryDriver(
                BUS_SUB_ID_2,
                "PrimaryCamera",
                &dataBus,
                primaryCamera.getFps());

    drivers.push_back(&primaryDriver);

    // Display driver for processed output
    GUI::CVMatDisplayDriver processedImageDriver(
                BUS_SUB_ID_3,
                std::to_string(BUS_SUB_ID_1),   // Get data off the bus from the processor
                &dataBus,
                primaryCamera.getFps());

    drivers.push_back(&processedImageDriver);

    // run

    std::thread cameraThread(startCamera, &primaryCamera);
    std::thread processorThread(startProcessor, &primaryProcessor);
    std::thread recorderThread(startRecorder, &primaryRecorder);

    QApplication a(argc, argv);

   // MainWindow w(&primaryDriver);

    MainWindow w(&drivers);


    w.show();
    int qtReturnCode = a.exec();

    // Shutdown

    primaryCamera.shutdown();
    primaryProcessor.shutdown();
    primaryRecorder.shutdown();

    cameraThread.join();
    processorThread.join();
    recorderThread.join();

    return qtReturnCode;
}
