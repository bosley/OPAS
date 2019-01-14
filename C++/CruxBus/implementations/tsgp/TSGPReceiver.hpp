#ifndef TSGP_RECEIVER_HEADER_G
#define TSGP_RECEIVER_HEADER_G

#include "CBMessage.hpp"

//!
//! \file TSGPReceiver.hpp
//!
//! \brief Defines a callback for a program to receive
//!        auto-polled messages from the TSGPInterface
//!
namespace TSGP
{
    class Receiver
    {
        public:
            //!
            //! \brief Called from TSGPInterface when messages are
            //!        on the bus for the receiver
            //!
            //! \param list Message list 
            //!
            virtual void msgsAvailable(CB::MessageList list) = 0;
    };
}

#endif
