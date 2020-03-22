#include <iostream>

#include "NETTcpServer.hpp"

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

void connectCallback(BUTANE::NET::Socket clientSocket)
{
    std::cout << "Client conencted!" << std::endl;

    std::string welcomeMsg = "H E L L O C L I E N T !\n"
                             "-----------------------\n"
                             "  Please send 10 chars \n"
                             "-----------------------\n";

    clientSocket.socketWriteOut((char*)welcomeMsg.c_str(), welcomeMsg.length());

    char buffer[10];

    clientSocket.socketReadIn(buffer, 10);

    std::cout << "Client sent: " << buffer << std::endl;

    clientSocket.socketClose(); // Optional, But return from callback will close socket
}

int main()
{
    BUTANE::NET::TcpServer tcpServer(connectCallback, errorCallback, 8000);

    while(1)
    {
        tcpServer.tick();
    }

    return 0;
}