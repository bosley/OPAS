#ifndef BUTANE_NET_TCP_SERVER_HPP
#define BUTANE_NET_TCP_SERVER_HPP

#include "NETSocket.hpp"
#include <string>

//!
//! \file NETTcpServer.hpp
//! \brief A Tcp server that listens for connections and sends back the connection
//!        via a function callback
//!
namespace BUTANE
{
    namespace NET
    {
        //!
        //! \class TcpServer
        //! \brief A Tcp server used to listen for connections and callback on
        //!
        class TcpServer : protected NET::Socket
        {
        public:
            //!
            //! \brief Construct a TcpServer
            //! \param connectCb Callback when a connection is made
            //! \param errorCb Callback when an error occurs
            //! \param port Port to listen on
            //! \param ip The IP to bind to (default = 127.0.0.1)
            //!
            TcpServer(std::function<void(Socket client)> connectCb,
                      std::function<void(SocketError)> errorCb,
                      int port, 
                      std::string ip = "127.0.0.1",
                      int maxPendingRequests = 10);

            //!
            //! \brief Destructs a server
            //!
            ~TcpServer();

            //!
            //! \brief Progresses the processing of the server
            //!        This method should be called as often as possible
            //!
            void tick();

        private:
            std::function<void(Socket client)> connectCb;
            std::function<void(SocketError)> errorCb;
            int port;
            std::string ip;
            bool ready;
         };
    }
}

#endif