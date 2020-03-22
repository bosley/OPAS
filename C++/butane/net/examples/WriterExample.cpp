#include "NETWriter.hpp"
#include "NETTcpServer.hpp"
#include "NETUdpServer.hpp"

#include <iostream>

/*


        Use this writer along-side TCP Server and UDP server examples to see the data written
        and populated in the respetive servers!


*/

namespace 
{
    BUTANE::NET::WriterType CurrentWriteType;
    bool tcpAvailable;
}

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
        case BUTANE::NET::SocketError::SOCKET_CONNECT: 

            std::cout << "Can't connect to host!" << std::endl; 

            if(CurrentWriteType == BUTANE::NET::WriterType::TCP)
            {
                tcpAvailable = false;
                std::cout << "A TCP server needs to run before writer will be able to connect" << std::endl;
            }
            break;
        default: break;
    }
}

void writerReady(BUTANE::NET::Socket writer)
{
    std::cout << "Writer ready!" << std::endl;

    if(CurrentWriteType == BUTANE::NET::WriterType::UDP)
    {
        std::string data = "UDP DATA!\n"; 
        writer.socketWriteOut((char *) data.c_str(), data.length());
    }
    else if(tcpAvailable) // TCP server needs to be available to attempt write with TCP
    {
        std::string data = "TCP DATA!";
        
        writer.socketWriteOut((char *) data.c_str(), data.length());
    }
}

int main()
{
    tcpAvailable = true; // Assume there is a server connected unless an error comes back.

    std::cout << "TCP Writer" << std::endl;
    CurrentWriteType = BUTANE::NET::WriterType::TCP;
    BUTANE::NET::Writer tcpWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", 8000);
    tcpWriter.callWriter();

    std::cout << "UDP Writer" << std::endl;
    CurrentWriteType = BUTANE::NET::WriterType::UDP;
    BUTANE::NET::Writer udpWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", 8001);
    udpWriter.callWriter();

    std::cout << "Closing" << std::endl;
    tcpWriter.close();
    udpWriter.close();
    return 0;
}