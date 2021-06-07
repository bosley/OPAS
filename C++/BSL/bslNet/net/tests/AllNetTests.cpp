#include <iostream>

#include "NetSocket.hpp"
#include "NetTcpServer.hpp"
#include "NetUdpServer.hpp"
#include "NetStdWriter.hpp"

#include "CppUTest/TestHarness.h"

namespace
{
    constexpr int MAX_TRYS               = 100;
    constexpr int TCP_TEST_PORT          = 8000;
    constexpr int TCP_THREADED_TEST_PORT = 8010;
    constexpr int UDP_TEST_PORT          = 8001;
    
    bool writerCalled;
    bool errorCalledBack;
    bool connectCalledBack;

    std::string *currentTest;

    NET::StdWriterType CurrentWriteType;

    void errorCallback(NET::SocketError err)
    {
        switch (err)
        {
            case NET::SocketError::SET_SOCK_OPT_RECV_TO:      std::cout << *currentTest << "ERROR: SET_SOCK_OPT_RECV_TO" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::SET_SOCK_OPT_SEND_TO:      std::cout << *currentTest << "ERROR: SET_SOCK_OPT_SEND_TO" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::SOCKET_WRITE:              std::cout << *currentTest << "ERROR: SOCKET_WRITE" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::ATTEMPT_INIT_SETUP_SOCKET: std::cout << *currentTest << "ERROR: ATTEMPT_INIT_SETUP_SOCKET" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::SOCKET_CREATE:             std::cout << *currentTest << "ERROR: SOCKET_CREATE" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::SOCKET_BIND:               std::cout << *currentTest << "ERROR: SOCKET_BIND" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::SOCKET_LISTEN:             std::cout << *currentTest << "ERROR: SOCKET_LISTEN" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::WSAStartup:                std::cout << *currentTest << "ERROR: WSAStartup" << std::endl; errorCalledBack = true; break;
            case NET::SocketError::SOCKET_CONNECT:            std::cout << *currentTest << "ERROR: SOCKET_CONNECT" << std::endl; errorCalledBack = true; break;
            default: break;
        }
    }
    
    void connectCallback(NET::Socket clientSocket, void*cbObj)
    {
        connectCalledBack = true;
    }
    
    void writerReady(NET::Socket writer, void*cb)
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
        CurrentWriteType  = NET::StdWriterType::TCP;
        currentTest       = new std::string("Tcp ");
   }

    void teardown()
    {
        delete currentTest;
    }
};

TEST(Tcp, TcpTest)
{
    NET::TcpServer tcpServer(connectCallback, errorCallback, TCP_TEST_PORT);

    NET::StdWriter tcpStdWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", TCP_TEST_PORT);
    tcpStdWriter.callWriter();

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
        FAIL("StdWriter couldn't be called");
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
        CurrentWriteType  = NET::StdWriterType::UDP;
       currentTest       = new std::string("Udp ");
   }

    void teardown()
    {
        delete currentTest;
    }
};

TEST(Udp, UdpTest)
{
    NET::UdpServer udpServer(connectCallback, errorCallback, UDP_TEST_PORT);

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
        currentTest       = new std::string("Write [Expect Error: SOCKET_CONNECT] ");
   }

   void teardown()
   {
       delete currentTest;
   }
};

TEST(Write, StdWriterTest)
{
    CurrentWriteType = NET::StdWriterType::TCP;
    NET::StdWriter tcpStdWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", TCP_TEST_PORT);
    tcpStdWriter.callWriter();

    CurrentWriteType = NET::StdWriterType::UDP;
    NET::StdWriter udpStdWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", UDP_TEST_PORT);
    udpStdWriter.callWriter();

    tcpStdWriter.close();
    udpStdWriter.close();

    if(!writerCalled)
    {
        FAIL("StdWriter couldn't be called");
    }
}


#ifdef BSL_NET_COMPILE_WITH_THREADS

TEST_GROUP(ThreadedTcp)
{
    void setup()
    {
        writerCalled      = false;
        errorCalledBack   = false;
        connectCalledBack = false;
        currentTest       = new std::string("Threaded Tcp ");
    }

    void teardown()
    {
        delete currentTest;
    }
};

TEST(ThreadedTcp, thrTcpTest)
{
    NET::TcpServer tcpServer(connectCallback, errorCallback, TCP_THREADED_TEST_PORT);

    CHECK_TRUE(tcpServer.startThreaded());

    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    NET::StdWriter tcpStdWriter(writerReady, errorCallback, CurrentWriteType, "127.0.0.1", TCP_THREADED_TEST_PORT);

    tcpStdWriter.callWriter();

    if(!writerCalled)
    {
        FAIL("StdWriter couldn't be called in threaded test");
    }

    if(errorCalledBack)
    {
        std::cerr << "THREADED TCP : FAIL > " <<
                  "No failure raised as this may be caused by an existing server bound to port ["
                  << TCP_THREADED_TEST_PORT << "]" << std::endl;
        return;
    }
    else
    {
        std::cout << "THREADED TCP : PASS" << std::endl;
    }

    try{
        tcpServer.stopThreaded();
    } catch (...) {
        FAIL("Unable to stop thread : A join error?");
    }
}

#endif