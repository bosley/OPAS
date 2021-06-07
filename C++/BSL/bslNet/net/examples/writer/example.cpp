#include "NetStdWriter.hpp"
#include "NetTcpServer.hpp"
#include "NetUdpServer.hpp"

#include <iostream>

/*


        Use this writer along-side TCP Server and UDP server examples to see the data written
        and populated in the respetive servers!


*/

namespace 
{
    NET::StdWriterType CurrentWriteType;
    bool tcpAvailable;
}

void errorCallback(NET::SocketError err)
{
    switch (err)
    {
        case NET::SocketError::SET_SOCK_OPT_RECV_TO: std::cout << "SET_SOCK_OPT_RECV_TO" << std::endl;  break;
        case NET::SocketError::SET_SOCK_OPT_SEND_TO: std::cout << "SET_SOCK_OPT_SEND_TO" << std::endl; break;
        case NET::SocketError::SOCKET_WRITE: std::cout << "SOCKET_WRITE" << std::endl; break;
        case NET::SocketError::ATTEMPT_INIT_SETUP_SOCKET: std::cout << "ATTEMPT_INIT_SETUP_SOCKET" << std::endl; break;
        case NET::SocketError::SOCKET_CREATE: std::cout << "SOCKET_CREATE" << std::endl; break;
        case NET::SocketError::SOCKET_BIND: std::cout << "SOCKET_BIND" << std::endl; break;
        case NET::SocketError::SOCKET_LISTEN: std::cout << "SOCKET_LISTEN" << std::endl; break;
        case NET::SocketError::WSAStartup: std::cout << "WSAStartup" << std::endl; break;
        case NET::SocketError::SOCKET_CONNECT:

            std::cout << "Can't connect to host!" << std::endl; 

            if(CurrentWriteType == NET::StdWriterType::TCP)
            {
                tcpAvailable = false;
                std::cout << "A TCP server needs to run before writer will be able to connect" << std::endl;
            }
            break;
        default: break;
    }
}

void writerReady(NET::Socket writer, void*cb)
{
    std::cout << "Writer ready!" << std::endl;

    if(CurrentWriteType == NET::StdWriterType::UDP)
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

void inplaceExample()
{
    // This setsup a writer, defines data to send, and sends data as soon as writer is ready, then closes connection
    // TO make this UDP, just change NET::StdWriterType to UDP

    std::cout << "in place examples " << std::endl;

    NET::StdWriter tcpWriter([](NET::Socket writer, void*cb) {

        std::vector<uint8_t> uintData = { 23, 33, 122, 44, 99 , 55, 200, 3, 9, 10}; // Example server expects 10 datas

        writer.socketWriteOut( static_cast<void*>(uintData.data()) , uintData.size());

    } , errorCallback, NET::StdWriterType::TCP, "127.0.0.1", 8000, true); // Define error callback, of-type TCP, addr, port, call when ready

}

int main()
{
    tcpAvailable = true; // Assume there is a server connected unless an error comes back.

    std::cout << "TCP Writer" << std::endl;
    CurrentWriteType = NET::StdWriterType::TCP;
    NET::StdWriter tcpWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", 8000);
    tcpWriter.callWriter();


    std::cout << "UDP Writer" << std::endl;
    CurrentWriteType = NET::StdWriterType::UDP;
    NET::StdWriter udpWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", 8001);
    udpWriter.callWriter();

    std::cout << "Closing" << std::endl;
    tcpWriter.close();
    udpWriter.close();


    inplaceExample();

    return 0;
}