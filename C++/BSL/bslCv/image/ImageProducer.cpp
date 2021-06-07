#include "ImageProducer.hpp"

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Producer::Create
    // --------------------------------------------------------------------
    Producer * Producer::Create(ProducerType type, std::string source) {
        switch (type) {
            case ProducerType::PT_VideoFile:         return new VideoFile(source);
            case ProducerType::PT_ImageFile:         return new ImageFile(source);
            case ProducerType::PT_VideoCameraLocal:  return new VideoCameraLocal(source);
            case ProducerType::PT_VideoCameraRemote: return new VideoCameraRemote(source);
        }
        return nullptr;
    }
}
