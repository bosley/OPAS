#include "Callbacks.hpp"

#include <iostream>

namespace OBELUS
{
    void ErrorCallback(BUTANE::NET::SocketError err)
    {
        switch (err)
        {
            case BUTANE::NET::SocketError::SET_SOCK_OPT_RECV_TO:      std::cerr << "SET_SOCK_OPT_RECV_TO"       << std::endl;  break;
            case BUTANE::NET::SocketError::SET_SOCK_OPT_SEND_TO:      std::cerr << "SET_SOCK_OPT_SEND_TO"       << std::endl; break; 
            case BUTANE::NET::SocketError::SOCKET_WRITE:              std::cerr << "SOCKET_WRITE"               << std::endl; break;
            case BUTANE::NET::SocketError::ATTEMPT_INIT_SETUP_SOCKET: std::cerr << "ATTEMPT_INIT_SETUP_SOCKET"  << std::endl; break;
            case BUTANE::NET::SocketError::SOCKET_CREATE:             std::cerr << "SOCKET_CREATE"              << std::endl; break;
            case BUTANE::NET::SocketError::SOCKET_BIND:               std::cerr << "SOCKET_BIND"                << std::endl; break;
            case BUTANE::NET::SocketError::SOCKET_LISTEN:             std::cerr << "SOCKET_LISTEN"              << std::endl; break;
            case BUTANE::NET::SocketError::WSAStartup:                std::cerr << "WSAStartup"                 << std::endl; break;
            default: break;
        }
    }
}