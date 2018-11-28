/*
 * The MIT License
 *
 * Copyright 2018 Josh A. Bosley.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   core.cpp
 * Author: Josh A. Bosley
 * 
 * Created on March 30, 2018, 2:18 PM
 */

#include "core.h"

namespace dcore
{
 
boost::atomic<bool> core_shutdown(false);

static boost::thread_group dynos_threads;


bool CheckForShutdown()
{
    return core_shutdown;
}

void TriggerShutdown()
{
    if(CheckForShutdown())
        return ;
    core_shutdown = true;
}

void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGABRT:
            std::cout << "ABRT\n";
            TriggerShutdown();
        break;
        case SIGFPE:
            std::cout << "FPE\n";
            TriggerShutdown();
        break;
        case SIGILL:
            std::cout << "ILL\n";
            TriggerShutdown();
        break;
        case SIGINT:
            std::cout << "\nInterrupt\n";
            TriggerShutdown();
        break;
        case SIGSEGV:
            std::cout << "SEGV\n";
            exit(0);
        break;
        case SIGTERM:
            std::cout << "TERM\n";
            TriggerShutdown();
        break;
    }
}

bool Start()
{
    if(CheckForShutdown())
    {
        std::cout << "Can not start while program is shutting down.\n";
        return false;
    }
    
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGFPE, signal_handler);
    std::signal(SIGILL, signal_handler);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    std::cout << "Starting seed manager...\n";
    
    // Seed prioritizer
    boost::thread *t2 = new boost::thread(dnet::SeedPrior);
    dynos_threads.add_thread(t2);
    
    std::cout << "Starting RPC server...\n";
    
    // RPC Server
    boost::thread *t1 = new boost::thread(dnet::StartRPC);
    dynos_threads.add_thread(t1);
    
    return true;
}  


int PerformShutdown()
{
    std::cout << "Shutting down\n";
    // Kill things!
    
    dnet::StopRPC();
    
    
    dynos_threads.join_all();
    
    return 0;
}


}