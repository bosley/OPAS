#ifndef CTCPLIB_SERVER_H
#define CTCPLIB_SERVER_H

#include "CTCPCommon.h"
#include "CTCPTypes.hpp"
#include "CTCPReceiverIf.hpp"

#include <string>
#include <vector>

namespace CTCP
{
    //!
    //! \class ThreadedTcpServer
    //! \brief A TCP server that will communicate with a CTCPTransmitter
    //!        for data transfers and will pipe data to a CTCPReceiverIf
    //!
    class ThreadedTcpServer
    {
    public:
        //!
        //! \brief Constructs a TCP server
        //! \param port The port to listen for connections on
        //! \param maxConnections Maximum number of threads able to spawn
        //!
        ThreadedTcpServer(int port, int maxConnections);

        //!
        //! \brief Destruct the TCP server
        //!
        ~ThreadedTcpServer();

        //!
        //! \brief Register a message receiver
        //! \param receiver The interface to be called back on when data comes in
        //! \post Data that comes in over the port will be sent to the receiver
        //!
        void registerMessageReceiver(CTCP::ReceiverIf *receiver);

        //!
        //! \brief Runs the server
        //!
        bool serve();

    private:
        int port;
        int maxConnections;
        int connectionCount;

        std::vector<CTCP::ReceiverIf*> receivers;

        struct HandleData
        {
            int *sock_desc;
            ThreadedTcpServer *owner;
            int clientPort;
            std::string clientAddress;
        };

        int safeStringToInt(std::string suspect);

        static void *handler(void* handlerData);
    };
}


#endif
