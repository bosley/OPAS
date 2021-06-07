//
// Created by bosley on 3/23/20.
//

#ifndef BSL_SERV_OBJ_RECVIF_HPP
#define BSL_SERV_OBJ_RECVIF_HPP

#include <nlohmann/json.hpp>
#include "ServObjRemote.hpp"

typedef nlohmann::json json;

namespace SERV
{
    //!
    //! \class JRecvIf
    //! \brief Interface for something that receives some JSON data
    //!
    class JRecvIf
    {
    public:

        //!
        //! \brief Receive a message from a JRoute servers
        //! \param from Remote connection information of caller
        //! \param msg  The JSON Message sent from remote
        //!
        virtual void recvMsg(ObjRemote from, json msg) = 0;
    };
}

#endif //BSL_JRECVRIF_HPP
