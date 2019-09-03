#include "Vision.h"

#include <chrono>
#include <mutex>
#include <map>
#include <iomanip>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

namespace MOD { namespace VISION
{

namespace
{
    struct _cameraData
    {
        int number;
        cv::VideoCapture cap;

    };

    struct _streamData
    {
        std::string file;
        cv::VideoWriter vout;
    };

    std::mutex _cameraMutex;
    std::mutex _streamMutex;
    std::map<std::string, _cameraData> _cameras;
    std::map<std::string, _streamData> _streams;

    bool cameraExists(std::string name)
    {
        for(auto i = _cameras.begin(); i != _cameras.end(); ++i)
        {
            if(name == i->first)
            {
                return true;
            }
        }
        return false;
    }

    bool cameraExists(int number)
    {
        for(auto i = _cameras.begin(); i != _cameras.end(); ++i)
        {
            if(number == i->second.number)
            {
                return true;
            }
        }
        return false;
    }

    bool streamExists(std::string name)
    {
        for(auto i = _streams.begin(); i != _streams.end(); ++i)
        {
            if(name == i->first)
            {
                return true;
            }
        }
        return false;
    }
}

bool visionBindCamera(std::string cameraName, int cameraNumber)
{
    if(cameraExists(cameraName) || cameraExists(cameraNumber) || cameraNumber < 0)
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(_cameraMutex);
        _cameras[cameraName] = _cameraData();
        _cameras[cameraName].number = cameraNumber;

        if(!_cameras[cameraName].cap.open(cameraNumber))
        {
            return false;
        }


    }
    return true;
}

bool visionFreeCamera(std::string cameraName)
{
    if(!cameraExists(cameraName))
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(_cameraMutex);
        _cameras[cameraName].cap.release();
        _cameras.erase(cameraName);
    }
    return true;
}

cv::Mat visionGetFrame(std::string cameraName, bool &okay, int width, int height)
{
    cv::Mat frame;
    cv::Mat sized;
    if(!cameraExists(cameraName))
    {
        okay = false;
        return frame;
    }
    else
    {
        if(!_cameras[cameraName].cap.isOpened())
        {
            okay = false;
            return frame;
        }

        if(!_cameras[cameraName].cap.read(frame))
        {
            okay = false;
            return frame;
        }

        cv::resize(frame, sized,
                   cv::Size(width, height)
                   );
    }

    (sized.empty()) ? okay = false : okay = true;
    return sized;
}


bool visionResetCamera(std::string cameraName)
{
    if(!cameraExists(cameraName))
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(_cameraMutex);
        _cameras[cameraName].cap.release();

        if(!_cameras[cameraName].cap.open(_cameras[cameraName].number))
        {
            return false;
        }
    }
    return true;
}

double visionGetFps(std::string cameraName)
{
    if(!cameraExists(cameraName))
    {
        return 0.0;
    }
    return _cameras[cameraName].cap.get(cv::CAP_PROP_FPS);
}


bool visionBindOutputStream(std::string cameraName, double fps, cv::Size imageSize, std::string directory)
{
    if(streamExists(cameraName))
    {
        return false;
    }
    std::lock_guard<std::mutex> lock(_streamMutex);

    std::stringstream ss;
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    ss << directory << cameraName << std::put_time( std::localtime(&time), "-%Y-%m-%d_%H:%M:%S");

    _streams[cameraName] = {ss.str(), cv::VideoWriter()};

    _streams[cameraName].vout.open(
                (_streams[cameraName].file + ".avi"),
                CV_FOURCC('M','J','P','G'),
                static_cast<int>(fps),
                imageSize,
                true
                );
    return true;
}

bool visionWriteToStream(std::string camera,
                         cv::Mat image,
                         std::vector<ImageArtifact> *artifacts)
{
    if(!streamExists(camera))
    {
        return false;
    }

    if(!_streams[camera].vout.isOpened())
    {
        std::cout << __FUNCTION__
                  << " : VideoWriter not open ("
                  << _streams[camera].file
                  << ".avi)"
                  << std::endl;
        return false;
    }

    _streams[camera].vout.write(image);

    // Append artifact file
    if(artifacts != nullptr)
    {
        std::ofstream writer(_streams[camera].file + ".artifacts", std::ios::app);

        if(!writer)
        {
            std::cout << __FUNCTION__
                      << " : ofstream writer not open ("
                      << _streams[camera].file
                      << ".index)"
                      << std::endl;
            return false;
        }

        for(auto artifact = artifacts->begin(); artifact != artifacts->end(); ++artifact)
        {
            std::time_t time   = std::chrono::system_clock::to_time_t(artifact->time);

            std::stringstream sw;
            sw << std::put_time( std::localtime(&time), "H:%M:%S")
               << "|"
               << artifact->artifactId
               << "|"
               << artifact->designation
               << "|"
               << artifact->areaOfInterest.x
               << "|"
               << artifact->areaOfInterest.y
               << "|"
               << artifact->areaOfInterest.height
               << "|"
               << artifact->areaOfInterest.width
               << "\n";
            writer << sw.str();
        }
    }

    return true;

}

bool visionFreeStream(std::string cameraName)
{
    if(!streamExists(cameraName))
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(_streamMutex);
    _streams[cameraName].vout.release();
    _streams.erase(cameraName);
    return true;
}

} }
