#ifndef HPIC_HEADER_GUARD_HPP
#define HPIC_HEADER_GUARD_HPP

#include <butane/NETSocket.hpp>

//!
//! \file hpic.hpp
//! \brief Contains the methods required to enable the HPIC
//!
namespace APP
{
    //!
    //! \brief Information that the HPIC will dole out during requests
    //!
    struct HpicInformation
    {
        std::string HpicIp;             //! IP Address of the HPIC server
        int HpicPort;                   //! PORT of the HPIC server
        
        std::string LuaIP;              //! IP Address of the LUA server
        int LuaPort;                    //! PORT of the LUA server

        std::string serverDescription;  //! Description of the server

        std::string serverVersion;      //! Version of the server
    };

    //!
    //! \brief Set the information for HPIC
    //! \param hpInfo The HpicInformation
    //!
    void HpicSetInfo(HpicInformation hpInfo);

    //!
    //! \brief Handles TCP connections meant for the HPIC server
    //! \param socket The socket bound to the connected client
    //!
    void HpicHandler(BUTANE::NET::Socket socket);
}

#endif