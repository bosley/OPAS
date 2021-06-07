#include "NetStdWriter.hpp"

#include <cstring>

namespace NET
{
    // -------------------------------------------------------
    // StdWriter
    // -------------------------------------------------------

    StdWriter::StdWriter(std::function<void(Socket client, void*cb)> writerReady,
                    std::function<void(SocketError)> errCb,
                    StdWriterType wtype, std::string ip, int port, bool callWhenReady, void*cb) :
                                                                    Socket(errCb),
                                                                    writerType(wtype),
                                                                    ip(ip),
                                                                    port(port),
                                                                    errCb(errCb),
                                                                    writerReady(writerReady),
                                                                    ready(false),
                                                                    cb(cb)

    {

#ifdef _MSC_VER
        WSADATA ws_data;
        if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
        {
            errorCb(SocketError::WSAStartup);
            exit(1);
        }
#endif

        struct sockaddr_in serverAddr; // server address   
        if(wtype == StdWriterType::TCP)
        {
            // create a stream socket using TCP
            if ((this->socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            {   
                errCb(SocketError::SOCKET_CREATE);
                return;
            }
        }
        else
        {
            // create a stream socket using TCP
            if ((this->socketFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
            {   
                errCb(SocketError::SOCKET_CREATE);
                return;
            }
        }
        
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        if (inet_pton(AF_INET, this->ip.c_str(), &serverAddr.sin_addr) <= 0)
        {
            errCb(SocketError::SOCKET_CREATE);
            return;
        }

        serverAddr.sin_port = htons(this->port);

        if (connect(this->socketFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
        {
            errCb(SocketError::SOCKET_CONNECT);
            return;
        }

        this->setupSocket(this->socketFd, serverAddr);
        ready = true;

        if(callWhenReady)
        {
            writerReady(*this, cb);
            this->socketClose();
        }
    }

    // -------------------------------------------------------
    // ~StdWriter
    // -------------------------------------------------------
    
    StdWriter::~StdWriter()
    {
    }

    // -------------------------------------------------------
    // callStdWriter
    // -------------------------------------------------------
    
    void StdWriter::callWriter()
    {
        if(!ready)
        {
            return;
        }
        
        // If setup fails, the writer kills the application so this should always be okay
        writerReady(*this, cb);
    }

    // -------------------------------------------------------
    // close
    // -------------------------------------------------------
    
    void StdWriter::close()
    {
        this->socketClose();
    }
}