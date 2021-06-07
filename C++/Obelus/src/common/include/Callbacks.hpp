#ifndef APP_COMMON_CALLBACKS
#define APP_COMMON_CALLBACKS

#include <butane/NETSocket.hpp>

//!
//! \file Callbacks.hpp
//! \brief Contains callbacks required for setup
//!
namespace OBELUS
{
    //!
    //! \brief Callback for errors that occur within BUTANE 
    //!        relating to sockets
    //!
    void ErrorCallback(BUTANE::NET::SocketError err);
}
#endif