//
//  ProcessIf - An interface for creating a process that can be loaded into
//              a processor object for image processing
//

#ifndef IMAGEPROCESSIF_H
#define IMAGEPROCESSIF_H

#include "ImageFrame.hpp"
#include "ImageProcessResult.hpp"
#include "ImageFrameReceiverIf.hpp"

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  ProcessIf
    // --------------------------------------------------------------------
    class ProcessIf
    {
    public:
        ProcessIf(std::string name,                     // Name of the process
                  unsigned id,                          // Uid of process
                  IMAGE::FrameRecvIf * processViewer)   // Means to see the process (or nullptr)
                                                 : name(name),
                                                   id(id),
                                                   processViewer(processViewer){}
        virtual ~ProcessIf(){}

        unsigned getId() const {
            return id;
        }

        std::string getName() const {
            return name;
        }

        // Takes in an image frame, runs its process, and stores
        // any required information within the frame data
        virtual IMAGE::ProcessResult process(IMAGE::Frame &frame) = 0;

    protected:
        std::string name;
        unsigned id;
        IMAGE::FrameRecvIf* processViewer;
    };
}
#endif // IMAGEPROCESSIF_H
