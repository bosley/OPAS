#include "ImageProcessor.hpp"

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Processor
    // --------------------------------------------------------------------
    Processor::Processor(std::string name, unsigned id) :
                                processName(name),
                                processId(id) {

    }

    // --------------------------------------------------------------------
    //  ~Processor
    // --------------------------------------------------------------------
    Processor::~Processor() {

        // Ensure all process threads dead before destruction
        //for(auto i = processes.begin(); i != processes.end(); ++i) {
        //    unsigned pid = (*i)->getId();
        //    if (procExe[pid].procThread.joinable()) {
        //        procExe[pid].procThread.join();
        //    }
        //}
    }

    // --------------------------------------------------------------------
    //  registerProcess
    // --------------------------------------------------------------------
    void Processor::registerProcess(ProcessIf &process, UTIL::MemBank<IMAGE::ProcessResult> *memory) {

        // Store the process pointer
        processes.push_back(&process);

        procExe[process.getId()].procMem = memory;

        // Mark the process thread as ready to be used
        //procThreads[process.getId()].complete = true;
    }

    // --------------------------------------------------------------------
    //  recvImageFrame
    // --------------------------------------------------------------------
    void Processor::recvImageFrame(IMAGE::Frame frame) {

        // Thread off the execution of all processes
        for(auto i = processes.begin(); i != processes.end(); ++i) {

            //unsigned pid = (*i)->getId();
            //if (procExe[pid].complete) {

            //    if (procExe[pid].procThread.joinable()) {
            //        procExe[pid].procThread.join();
            //    }

            //    procExe[pid].procThread = std::thread(&Processor::processOne, this, (*i), frame);
            //}

            processOne((*i), frame);
        }
    }

    // --------------------------------------------------------------------
    // processOne - Run a single frame through all processes given to processor
    // --------------------------------------------------------------------
    void Processor::processOne(IMAGE::ProcessIf *proc, IMAGE::Frame &frame) {

        unsigned pid = proc->getId();

        //procThreads[pid].complete = false;

        IMAGE::ProcessResult procRes = proc->process(frame);

        // If a memory module is given to the processor, store the return data
        if(procExe[pid].procMem != nullptr) {
            procExe[pid].procMem->pushData(procRes);
        }

        //procThreads[pid].complete = true;

    }
}
