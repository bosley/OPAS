#include <iostream>

#include "NETUdpServer.hpp"

void errorCallback(BUTANE::NET::SocketError err)
{
    switch (err)
    {
        case BUTANE::NET::SocketError::SET_SOCK_OPT_RECV_TO: std::cout << "SET_SOCK_OPT_RECV_TO" << std::endl;  break;
        case BUTANE::NET::SocketError::SET_SOCK_OPT_SEND_TO: std::cout << "SET_SOCK_OPT_SEND_TO" << std::endl; break; 
        case BUTANE::NET::SocketError::SOCKET_WRITE: std::cout << "SOCKET_WRITE" << std::endl; break;
        case BUTANE::NET::SocketError::ATTEMPT_INIT_SETUP_SOCKET: std::cout << "ATTEMPT_INIT_SETUP_SOCKET" << std::endl; break;
        case BUTANE::NET::SocketError::SOCKET_CREATE: std::cout << "SOCKET_CREATE" << std::endl; break;
        case BUTANE::NET::SocketError::SOCKET_BIND: std::cout << "SOCKET_BIND" << std::endl; break;
        case BUTANE::NET::SocketError::SOCKET_LISTEN: std::cout << "SOCKET_LISTEN" << std::endl; break;
        case BUTANE::NET::SocketError::WSAStartup: std::cout << "WSAStartup" << std::endl; break;
        default: break;
    }
}

void udpReady(BUTANE::NET::Socket udpSocket)
{
    std::cout << "UDP Server is serving: " << std::endl;
    while(1)
    {
        int recvSize;
        char recvBuff[10];

        recvSize = udpSocket.socketReadIn(recvBuff, 10);

        if(recvSize < 0)
        {
            std::cout << recvSize << std::endl;
            continue;
        }
        
        std::cout << recvBuff;
    }
}

int main()
{
    BUTANE::NET::UdpServer udpServer(udpReady, errorCallback, 8001);


    return 0;
}