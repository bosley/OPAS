//
//  Processor - Takes a series of processes that do image transformation,
//              or detection and runs a frame through them sequentially.
//
#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <queue>
#include <vector>
#include <map>
//#include <thread>
#include <atomic>

#include "ImageFrameReceiverIf.hpp"
#include "ImageProcessIf.hpp"
#include "bslcore/UtilMemBank.hpp"

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Processor
    // --------------------------------------------------------------------
    class Processor : public IMAGE::FrameRecvIf
    {
    public:

        // Create a processor
        Processor(std::string processName, unsigned processId);
        ~Processor() override;

        // Each process starts a thread for execution for every frame
        // (if the previous frame process has completed)
        void registerProcess(IMAGE::ProcessIf &process, UTIL::MemBank<IMAGE::ProcessResult> *memory = nullptr);

        // From FrameRecvIf
        virtual void recvImageFrame(IMAGE::Frame frame) override;

    private:
        std::string processName;
        unsigned processId;

        // Each process to be run on an image
        std::vector<IMAGE::ProcessIf *> processes;

        // Process thread information
        struct ProcMon {
            //std::atomic<bool>               complete;   // Flag to mark that the thread is complete
            //std::thread                     procThread; // The thread to manage processing
            UTIL::MemBank<IMAGE::ProcessResult> *procMem;    // Memory for return object from thread
        };

        // Thread map for each process
        std::map<unsigned, ProcMon> procExe;

        // Method that is threaded for the processing of a frame
        void processOne(IMAGE::ProcessIf* process, IMAGE::Frame &frame);

    };
}
#endif // IMAGE_PROCESSOR_H
