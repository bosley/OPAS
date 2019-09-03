#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

#define DEFAULT_VIDEO_STORAGE_LOCATION "/tmp/"
#define DEFAULT_VIDEO_DISPLAY_WIDTH  680
#define DEFAULT_VIDEO_DISPLAY_HEIGHT 420

namespace MOD { namespace SYSTEM {

//!
//! \brief The CameraConfig struct
//!        A configuration structure to define a camera
//!
struct CameraConfig
{
    unsigned    deviceNumber;
    std::string cameraName;
    bool        recordCamera;
    std::string recordFilePath;
    unsigned    imageWidth;
    unsigned    imageHeight;

    CameraConfig(std::string name,
                 unsigned deviceNumber,
                 unsigned width,
                 unsigned height) :
                                    deviceNumber(deviceNumber),
                                    cameraName(name),
                                    recordCamera(false),
                                    recordFilePath(DEFAULT_VIDEO_STORAGE_LOCATION),
                                    imageWidth(width),
                                    imageHeight(height)
    {}

    CameraConfig(std::string name,
                 unsigned deviceNumber,
                 bool     record,
                 std::string recordPath,
                 unsigned width,
                 unsigned height) :
                                    deviceNumber(deviceNumber),
                                    cameraName(name),
                                    recordCamera(record),
                                    recordFilePath(recordPath),
                                    imageWidth(width),
                                    imageHeight(height)
    {}

    CameraConfig(std::string name,
                 unsigned deviceNumber) :
                                    deviceNumber(deviceNumber),
                                    cameraName(name),
                                    recordCamera(false),
                                    recordFilePath(DEFAULT_VIDEO_STORAGE_LOCATION),
                                    imageWidth(DEFAULT_VIDEO_DISPLAY_WIDTH),
                                    imageHeight(DEFAULT_VIDEO_DISPLAY_HEIGHT)
    {}

};

//!
//! \brief The SystemConfig struct
//!        Lays out the configuration for the entire system
//!
struct HardwareConfig
{
    std::vector <CameraConfig> cameras;

    HardwareConfig& operator= (const HardwareConfig rhs)
    {
        this->cameras = rhs.cameras;
        return *this;
    }
};


} }

#endif // CONFIG_H
