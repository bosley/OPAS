#ifndef BUTANE_NET_SOCKET_HPP
#define BUTANE_NET_SOCKET_HPP

#ifdef _MSC_VER
    #include <winsock.h>
    #include <ws2tcpip.h>

    #define CLOSE_FD(fd) closesocket(fd)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <unistd.h>
    #include <syslog.h>

    #define CLOSE_FD(fd) close(fd)
#endif

#include <string>
#include <functional>

//!
//! \file NETSockets.hpp
//! \brief Header for abstracting sockets
//!
namespace BUTANE
{
    namespace NET
    {
        constexpr int SOCKET_RECV_TIMEOUT_SEC = 10;  //! Socket timeout sec for recv sockets
        constexpr int SOCKET_RECV_TIMEOUT_MS  = 0;  //! Socket timeout ms for recv sockets
        constexpr int SOCKET_SEND_TIMEOUT_SEC = 10;  //! Socket timeout sec for send sockets
        constexpr int SOCKET_SEND_TIMEOUT_MS  = 0;  //! Socket timeout ms for send sockets

        //!
        //! \brief A socket error type
        //!
        enum class SocketError
        {
            SET_SOCK_OPT_RECV_TO,      //! Unable to set recv timeout
            SET_SOCK_OPT_SEND_TO,      //! Unable to set send timeout
            SOCKET_WRITE,              //! Error to write data
            ATTEMPT_INIT_SETUP_SOCKET, //! Trying to init an initialized socket
            SOCKET_CREATE,             //! Couldn't create a socket
            SOCKET_BIND,               //! Socket bind fail
            SOCKET_LISTEN,             //! Socket listen fail
            WSAStartup,                //! Socket setup fail
            SOCKET_CONNECT             //! Unable to connect to remote
        };

        //!
        //! \class Socket
        //! \brief Socket abstraction with read/write functionality
        //!
        class Socket
        {
            public:
                //!
                //! \brief Socket Constructor
                //!        Note: init method must be called before use
                //! \param errorCallback Function to call when an error occurs
                //!
                Socket(std::function<void(SocketError)> errorCallback);
                
                //!
                //! \brief Socket Destructor
                //!
                ~Socket();

                //!
                //! \brief socketWriteOut - Errors reported via errorCallback
                //! \param buffer The buffer to write out
                //! \param bufferLen Length of the given buffer
                //!
                void socketWriteOut(const void* buffer, int bufferLen);

                //!
                //! \brief Socket read
                //! \param buffer The buffer to read to
                //! \param bufferLen Length of the given buffer
                //! \retval Total bytes received
                //!
                int socketReadIn(void * buffer, int bufferLen);

                //!
                //! \brief Setup a socket - Errors reported via errorCallback
                //! \param socketFd Socket file desc
                //! \param sockAddr Socket address
                //!
                void setupSocket(int socketFd, sockaddr_in sockAddr);

                //!
                //! \brief Close the socket
                //! \post Socket _COULD_ be re-setup
                //!
                void socketClose();

            protected:
                bool isInitd;
                int socketFd;
                sockaddr_in sockAddr;
        
                struct timeval recvTimeout;
                struct timeval sendTimeout;

            private:
                std::function<void(SocketError)> infoCb;
        };
    }
}

#endif
