#include "NETUdpServer.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
#include <mutex>
#include <atomic>

namespace BUTANE
{
    namespace NET
    {
        // ---------------------------------------------------------
        // UdpServer
        // ---------------------------------------------------------
        UdpServer::UdpServer(std::function<void(Socket)> readyCb,
                                    std::function<void(SocketError)> errorCb,
                                    int port, 
                                    std::string ip,
                                    int maxPendingRequests) : NET::Socket(errorCb),
                                                                readyCb(readyCb),
                                                                errorCb(errorCb),
                                                                port(port),
                                                                ip(ip)
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

            if ((this->socketFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
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

            this->setupSocket(this->socketFd, this->sockAddr);

            readyCb(*this);
        }

        // ---------------------------------------------------------
        // ~UdpServer
        // ---------------------------------------------------------
        UdpServer::~UdpServer()
        {
    #if defined (_MSC_VER)
            WSACleanup();
    #endif
        }
    }
}