//
// Created by bosley on 3/23/20.
//

#ifndef BSL_SERV_OBJREMOTE_HPP
#define BSL_SERV_OBJREMOTE_HPP

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace SERV
{
    //!
    //! \brief A remote object that contains information regarding a remote transporter
    //!
    class ObjRemote {
    public:

        //!
        //! \brief Construct a blank remote
        //! \post IP=127.0.0.1 PORT=4096 DESTNAME=default
        //!
        ObjRemote();

        //!
        //! \brief Construct a specific remote
        //! \param address Remote transporter's address
        //! \param port Remote transporter's port
        //! \param destinationName Destination name on remote transport
        //!
        ObjRemote(std::string address, uint16_t port, std::string destinationName);

        //!
        //! \return IP Address
        //!
        std::string getAddress() const;

        //!
        //! \return Port Number
        //!
        int getPort() const;

        //!
        //! \return Destination name
        //!
        std::string getName() const;

        //!
        //! \return Json object containing address, name, and port
        //!
        json asJson();

    private:
        std::string address;
        std::string name;
        int port;
    };
}


#endif //BSL_SERVObjRemote_HPP
