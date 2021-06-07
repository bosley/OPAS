#include <iostream>

#include "NetTcpServer.hpp"

void connectCallback(NET::Socket clientSocket, void* cbPointer)
{
    std::cout << "Client conencted!" << std::endl;

    char buffer[10]; // This could be uint8_t too

    clientSocket.socketReadIn(buffer, 10);

    std::cout << "Client sent: " << buffer << std::endl;

    clientSocket.socketClose(); // Optional, But return from callback will close socket
}

int main()
{
    // A tcp server             on-connect       std error sink     port
    NET::TcpServer tcpServer(connectCallback, NET::ErrorSink, 8000);

    tcpServer.startThreaded();

    int i = 0;

    std::cout << "Running for 1 minute" << std::endl;

    while(i++ != 60)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Stopping server thread" << std::endl;

    tcpServer.stopThreaded();

    return 0;
}