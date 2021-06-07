#ifndef NET_UDP_SERVER_HPP
#define NET_UDP_SERVER_HPP

#include "NetSocket.hpp"
#include <string>

//!
//! \file NETUdpServer.hpp
//! \brief A Udp server that sets up the UDP socket and once readt, will
//!        callback to the creater via readyCb with a socket ready to read/write data/
//!        It is up-to the creator to keep the server alive
//!
namespace NET
{
    //!
    //! \class UdpServer
    //! \brief A Udp server that sets up a socket and hands it back to a user to serve
    //!
    class UdpServer : protected Socket
    {
    public:
        //!
        //! \brief Construct a UdpServer
        //! \param readyCb When the server is setup and ready the socket is passed back to read/ write
        //! \param errorCb Callback when an error occurs
        //! \param port Port to listen on
        //! \param ip The IP to bind to (default = 127.0.0.1)
        //!
        UdpServer(std::function<void(Socket, void*)> readyCb,
                    std::function<void(SocketError)> errorCb,
                    int port, 
                    std::string ip = "127.0.0.1",
                    void* cbObj = nullptr,
                    int maxPendingRequests = 10);

        //!
        //! \brief Destructs a server
        //!
        ~UdpServer();

    private:
        std::function<void(Socket, void*)> readyCb;
        std::function<void(SocketError)> errorCb;
        int port;
        std::string ip;
        void* cbObject;
        };

}

#endif