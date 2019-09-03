#ifndef CTCPLIB_RECEIVER_IF_H
#define CTCPLIB_RECEIVER_IF_H

#include "CTCPTypes.hpp"

namespace CTCP
{
    class ReceiverIf
    {
        public:
            virtual void receive(CTCP::MessageData msg) = 0;
    };
}

#endif
