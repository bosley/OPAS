#include <iostream>

#include "NetUdpServer.hpp"

void udpReady(NET::Socket udpSocket, void* cbObj)
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
    // Create a udp server,               Standard error sink,  port
    NET::UdpServer udpServer(udpReady, NET::ErrorSink,    8001);

    return 0;
}