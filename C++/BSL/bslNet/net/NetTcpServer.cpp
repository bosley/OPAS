#include "NetTcpServer.hpp"
#include <iostream>
#include <cstring>

namespace NET
{
    // ---------------------------------------------------------
    // TcpServer
    // ---------------------------------------------------------
    TcpServer::TcpServer(std::function<void(Socket client, void*callbackObj)> connectCb,
                                    std::function<void(SocketError)> errorCb,
                                    int port, 
                                    std::string ip,
                                    void* callbackObj,
                                    int maxPendingRequests) :
#ifdef BSL_NET_COMPILE_WITH_THREADS
                                                            Socket(errorCb),
                                                                connectCb(connectCb),
                                                                errorCb(errorCb),
                                                                callbackObj(callbackObj),
                                                                port(port),
                                                                ip(ip),
                                                                ready(false),
                                                                threadRunning(false)
#else
                                                            Socket(errorCb),
                                                                connectCb(connectCb),
                                                                errorCb(errorCb),
                                                                callbackObj(callbackObj),
                                                                port(port),
                                                                ip(ip),
                                                                ready(false)
#endif
    {

#ifdef _MSC_VER
        WSADATA ws_data;
        if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
        {
            errorCb(SocketError::WSAStartup);
            return;
        }
#endif
        if ((this->socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            errorCb(SocketError::SOCKET_CREATE);
            return;
        }
        
        memset(&this->sockAddr, 0, sizeof(this->sockAddr));
        this->sockAddr.sin_family = AF_INET;
        this->sockAddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
        this->sockAddr.sin_port = htons(this->port);

        if (::bind(this->socketFd, (sockaddr*)&this->sockAddr, sizeof(this->sockAddr)) < 0)
        {
            errorCb(SocketError::SOCKET_BIND);
            return;
        }

        // mark the socket so it will listen for incoming connections
        if (::listen(this->socketFd, maxPendingRequests) < 0)
        {
            errorCb(SocketError::SOCKET_LISTEN);
            return;
        }

        this->setupSocket(this->socketFd, this->sockAddr);
        ready = true;
    }

    // ---------------------------------------------------------
    // ~TcpServer
    // ---------------------------------------------------------
    TcpServer::~TcpServer()
    {
#if defined (_MSC_VER)
        WSACleanup();
#endif
        this->socketClose();
    }

    // ---------------------------------------------------------
    // tick
    // ---------------------------------------------------------
    void TcpServer::tick()
    {
        if(!ready)
        {
            std::cerr << "Server not ready" << std::endl;
            return;
        }
        
        int clientFd;
        sockaddr_in clientAddr;
#ifdef _MSC_VER
        int addrLen; 
#else
        socklen_t addrLen;
#endif
        addrLen = sizeof(clientAddr);

        if ((clientFd = accept(this->socketFd, (struct sockaddr*) &clientAddr,  &addrLen)) < 0)
        {
            return;
        }
        
         Socket clientSocket(errorCb);
        clientSocket.setupSocket(clientFd, clientAddr);

        // Send the socket so the owner can do whatever
        connectCb(clientSocket, callbackObj);

        clientSocket.socketClose();
    }

#ifdef BSL_NET_COMPILE_WITH_THREADS
    // ---------------------------------------------------------
    // startThreaded
    // ---------------------------------------------------------
    bool TcpServer::startThreaded()
    {
        {
            std::lock_guard<std::mutex> lock(threadMut);

            if (threadRunning) {
                std::cerr << "Thread already started" << std::endl;
                return false;
            }
        }

        threadRunning = true;

        serverThread = std::thread(
                [](TcpServer * server){

                    while(server->threadRunning) {

                        server->tick();

                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                    } // End func
                , this);

        return true;
    }

    // ---------------------------------------------------------
    // stopThreaded
    // ---------------------------------------------------------
    void TcpServer::stopThreaded()
    {
        std::lock_guard<std::mutex> lock(threadMut);

        if (!threadRunning) {
            std::cerr << "Thread not running" << std::endl;
        }

        threadRunning = false;

        serverThread.join();
    }
#endif
}