#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <string>

#include <opencv2/videoio/videoio.hpp>

class SourceManager
{
public:
    SourceManager();

    // Check if the source manager is ready
    bool isVideoReady();
    bool isCameraReady();

    // Return the selected media
    int getSelectedCamera();
    std::string getSelectedVideo();

    void setSelectedVideo(std::string selection);
    void setSelectedCamera(int selection);

    // Check the system for cameras
    void reloadCameras();

    // Return highest number of available cameras
    int getAvailableCameras();

private:
    bool videoReady;
    bool camerasReady;

    std::string videoFile;

    int cameraCount;
    int cameraSelection;
};

#endif // SOURCEMANAGER_H
