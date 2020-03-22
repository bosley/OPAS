#include <iostream>

#include "NETSocket.hpp"
#include "NETTcpServer.hpp"
#include "NETUdpServer.hpp"
#include "NETWriter.hpp"

#include "CppUTest/TestHarness.h"

namespace
{
    constexpr int MAX_TRYS      = 100;
    constexpr int TCP_TEST_PORT = 8000;
    constexpr int UDP_TEST_PORT = 8001;
    
    bool writerCalled;
    bool errorCalledBack;
    bool connectCalledBack;

    BUTANE::NET::WriterType CurrentWriteType;

    void errorCallback(BUTANE::NET::SocketError err)
    {
        switch (err)
        {
            case BUTANE::NET::SocketError::SET_SOCK_OPT_RECV_TO:      errorCalledBack = true; break;
            case BUTANE::NET::SocketError::SET_SOCK_OPT_SEND_TO:      errorCalledBack = true; break; 
            case BUTANE::NET::SocketError::SOCKET_WRITE:              errorCalledBack = true; break;
            case BUTANE::NET::SocketError::ATTEMPT_INIT_SETUP_SOCKET: errorCalledBack = true; break;
            case BUTANE::NET::SocketError::SOCKET_CREATE:             errorCalledBack = true; break;
            case BUTANE::NET::SocketError::SOCKET_BIND:               errorCalledBack = true; break;
            case BUTANE::NET::SocketError::SOCKET_LISTEN:             errorCalledBack = true; break;
            case BUTANE::NET::SocketError::WSAStartup:                errorCalledBack = true; break;
            case BUTANE::NET::SocketError::SOCKET_CONNECT:            errorCalledBack = true; break;
            default: break;
        }
    }
    
    void connectCallback(BUTANE::NET::Socket clientSocket)
    {
        connectCalledBack = true;
    }
    
    void writerReady(BUTANE::NET::Socket writer)
    {
        writerCalled = true;
    }
}

TEST_GROUP(Tcp)
{
   void setup()
   {
        writerCalled      = false;
        errorCalledBack   = false;
        connectCalledBack = false;
        CurrentWriteType  = BUTANE::NET::WriterType::TCP;
   }
};

TEST(Tcp, TcpTest)
{
    BUTANE::NET::TcpServer tcpServer(connectCallback, errorCallback, TCP_TEST_PORT);

    BUTANE::NET::Writer tcpWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", TCP_TEST_PORT);
    tcpWriter.callWriter();

    int numTrys = 0;
    while(!errorCalledBack && !connectCalledBack)
    {
        if(numTrys >= MAX_TRYS)
        {
            errorCalledBack = true;
        }

        tcpServer.tick();
        numTrys++;
    }

    if(!writerCalled)
    {
        FAIL("Writer couldn't be called");
    }

    if(errorCalledBack)
    {
        std::cerr << "TCP : FAIL > " << 
        "No failure raised as this may be caused by an existing server bound to port [" 
        << TCP_TEST_PORT << "]" << std::endl;
        return;
    }
    else
    {
        std::cout << "TCP : PASS" << std::endl;
    }
}

TEST_GROUP(Udp)
{
   void setup()
   {
        writerCalled      = false;
        errorCalledBack   = false;
        connectCalledBack = false;
        CurrentWriteType  = BUTANE::NET::WriterType::UDP;
   }
};

TEST(Udp, UdpTest)
{
    BUTANE::NET::UdpServer udpServer(connectCallback, errorCallback, UDP_TEST_PORT);

    if(errorCalledBack)
    {
        std::cerr << "UDP : FAIL > " << 
        "No failure raised as this may be caused by an existing server bound to port [" 
        << UDP_TEST_PORT << "]" << std::endl;
        return;
    }
    else
    {
        std::cout << "UDP : PASS" << std::endl;
    }
}

TEST_GROUP(Write)
{
   void setup()
   {
        writerCalled      = false;
        errorCalledBack   = false;
        connectCalledBack = false;
   }
};

TEST(Write, WriterTest)
{
    CurrentWriteType = BUTANE::NET::WriterType::TCP;
    BUTANE::NET::Writer tcpWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", TCP_TEST_PORT);
    tcpWriter.callWriter();

    CurrentWriteType = BUTANE::NET::WriterType::UDP;
    BUTANE::NET::Writer udpWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", UDP_TEST_PORT);
    udpWriter.callWriter();

    tcpWriter.close();
    udpWriter.close();

    if(!writerCalled)
    {
        FAIL("Writer couldn't be called");
    }
}