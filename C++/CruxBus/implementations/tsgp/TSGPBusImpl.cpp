#include "TSGPBusImpl.hpp"

#include <iostream>
#include <thread>        
#include <chrono>

namespace TSGP
{

    BusImpl::BusImpl(CB::AddressList whiteList) : Interface(whiteList),
                                                  running(false)
    {
    }

    BusImpl::~BusImpl()
    {
    }

    const bool BusImpl::registerRecvCallback(CB::Address const aFor, Receiver *callback)
    {
        std::lock_guard<std::mutex> lock(callbackMutex);
        const auto i = callbacks.find(aFor);
        if( i != callbacks.end() )
        {
            return false;
        }

        callbacks.insert( std::pair<CB::Address, Receiver*>(aFor, callback) );
        return true;
    }

    const bool BusImpl::start()
    {
        running = true;
        while(running)
        {
            CB::MessageList msgs;
            for(const auto &item : callbacks)
            {
                msgs.clear();
                if( pollForMessages(item.first, msgs) )
                {
                    if(!msgs.empty())
                    {
                        item.second->msgsAvailable(msgs);
                    }
                }
                std::this_thread::sleep_for (std::chrono::nanoseconds(1) );
            }
        }
        return true;
    }

    const bool BusImpl::stop()
    {
        if(!this->running)
        {
            return false;
        }
        running = false;
        return true;
    }

    const bool BusImpl::writeBusMessage(CB::Address const aFor, CB::Message const msg)
    {
        return writeMessage(aFor, msg);
    }
}
