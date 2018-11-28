#include "sourcemanager.h"

SourceManager::SourceManager()
{
    cameraCount = -1;
    videoReady = false;
    camerasReady = false;
}

bool SourceManager::isVideoReady()
{
    return videoReady;
}

bool SourceManager::isCameraReady()
{
    return camerasReady;
}

int SourceManager::getSelectedCamera()
{
    return cameraSelection;
}

std::string SourceManager::getSelectedVideo()
{
    return videoFile;
}

void SourceManager::setSelectedVideo(std::string selection)
{
    videoFile = selection;
}

void SourceManager::setSelectedCamera(int selection)
{
    cameraSelection = selection;
}

void SourceManager::reloadCameras()
{
    cameraCount = 0;
    bool search = true;
    while (search)
    {
        cv::VideoCapture cap;
        cap.open(cameraCount++);
        if(cap.isOpened())
            cap.release();
        else
           search = false;
    }
    cameraCount--;
}

int SourceManager::getAvailableCameras()
{
    return cameraCount;
}
