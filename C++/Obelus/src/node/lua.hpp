#ifndef LUA_HEADER_GUARD_OBELUS_HPP
#define LUA_HEADER_GUARD_OBELUS_HPP

#include <butane/NETSocket.hpp>

#include <string>

//!
//! \file lua.hpp
//! \brief Contains the methods required to enable lua functionality
//!
namespace APP
{
    //!
    //! \brief Handles TCP connections meant for the LUA script
    //! \param socket The socket bound to the connected client
    //!
    void LuaHandler(BUTANE::NET::Socket socket);

    //!
    //! \brief Sets information for lua setup and ensures that the script
    //!        contains function that is called from Obelus when a connection is made
    //! \param LuaIp The Ip address set for the lua server
    //! \param LuaPort The Port set for the lua server
    //! \param script The lua script that will be used 
    //! \retval[true] The script should be okay
    //! \retval[false] The script either doesn't exist, or doesn't contain the Request function
    //!
    bool LuaPreProcess(std::string LuaIP, int LuaPort, std::string script);

    //!
    //! \brief Processes the outbound queue to enable the writing of of messages
    //!        called by ObelusWrite within the script
    //!
    void LuaTickOutbound();
}

#endif