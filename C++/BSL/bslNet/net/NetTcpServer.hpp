#ifndef NET_TCP_SERVER_HPP
#define NET_TCP_SERVER_HPP

#include "NetSocket.hpp"
#include <string>

#ifdef BSL_NET_COMPILE_WITH_THREADS
    #include <atomic>
    #include <mutex>
    #include <thread>
#endif

//!
//! \file NETTcpServer.hpp
//! \brief A Tcp server that listens for connections and sends back the connection
//!        via a function callback
//!
namespace NET
{
    //!
    //! \class TcpServer
    //! \brief A Tcp server used to listen for connections and callback on
    //!
    class TcpServer : protected Socket
    {
    public:
        //!
        //! \brief Construct a TcpServer
        //! \param connectCb Callback when a connection is made
        //! \param errorCb Callback when an error occurs
        //! \param port Port to listen on
        //! \param ip The IP to bind to (default = 127.0.0.1)
        //!
        TcpServer(std::function<void(Socket client, void* callbackObj)> connectCb,
                    std::function<void(SocketError)> errorCb,
                    int port, 
                    std::string ip = "127.0.0.1",
                    void* callbackObj = nullptr,
                    int maxPendingRequests = 10);

        //!
        //! \brief Destructs a server
        //!
        ~TcpServer();

        //!
        //! \brief Progresses the processing of the server
        //!        This method should be called as often as possible
        //!        This is offered in the event that the targer application
        //!        Can not use threads
        //!
        void tick();

#ifdef BSL_NET_COMPILE_WITH_THREADS
        //!
        //! \brief Thread the calling of tick method so it doesn't have to be done manually
        //! \retval true if server started, false otherwise
        //!
        bool startThreaded();

        //!
        //! \brief Stop the server thread
        //!
        void stopThreaded();
#endif

    private:
        std::function<void(Socket client, void*callbackObj)> connectCb;
        std::function<void(SocketError)> errorCb;
        void* callbackObj;
        int port;
        std::string ip;
        bool ready;


#ifdef BSL_NET_COMPILE_WITH_THREADS
        std::atomic<bool> threadRunning;
        std::mutex  threadMut;
        std::thread serverThread;
#endif
        };
}

#endif