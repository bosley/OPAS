//
// Created by bosley on 3/23/20.
//

#ifndef BSL_SERV_OBJTRANSPORT_HPP
#define BSL_SERV_OBJTRANSPORT_HPP

#include <cstdint>
#include <map>
#include <queue>
#include <limits>
#include <nlohmann/json.hpp>
#include "bslcore/UtilTickableIf.hpp"
#include "NetTcpServer.hpp"
#include "ServObjRecvrIf.hpp"
#include "ServObjRemote.hpp"

typedef nlohmann::json json;

namespace SERV
{
    //!
    //! \class ObjTransport
    //! \brief A JSON Object sender/receiver
    //!
    class ObjTransport : public UTIL::Tickable{
    public:

        //!
        //! \param name   Name of the transporter
        //! \param ipAddress Address that the transporter can be reached at
        //! \param port Port to listen on
        //! \param maxBuffer Maximum buffer size allowed to receive (defaults to uint32_t max)
        //!
        ObjTransport(std::string name, std::string ipAddress, uint16_t port, uint32_t maxBuffer = std::numeric_limits<uint32_t>::max() );

        //!
        //! \brief Destruct the transporter
        //!
        ~ObjTransport();

        //!
        //! \brief Add a local receiver
        //! \param name Name of the receiver
        //! \param recvr A receiver interface that can accept json objects and their sender's information
        //!
        void addLocalReceiver(std::string name, JRecvIf & recvr);

        //!
        //! \brief Add a remote
        //! \param remote The remote information
        //! \post Remote will be added to internal remote map
        //!
        void addRemote(ObjRemote remote);

        //!
        //! \brief Send an object to a remote
        //! \param remote The name of the remote object (must exist within internal remote map)
        //! \param data The json object data to send
        //!
        void send(std::string remote, json data);

        //
        //  From UTIL::Tickable
        //
        virtual void tick() override;

    private:

        ObjRemote thisServer;

        NET::TcpServer tcpServer;

        std::map<std::string, ObjRemote> outbound;
        std::map<std::string, JRecvIf *> local;
        std::queue< std::vector<uint8_t> > inboundQueue;

        uint8_t dataBuff[4];
        std::vector<uint8_t> outboundMsgPack;
        uint32_t maxBufferAllowed;

        static void connectCallback(NET::Socket sock, void* cbObj);
    };
}


#endif //BSL_NETRPC_HPP
