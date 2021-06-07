//
// Created by bosley on 3/23/20.
//

#include "ServObjTransport.hpp"

#include <stdlib.h>
#include <iostream>
#include "NetStdWriter.hpp"

namespace SERV
{
    // ---------------------------------------------------------
    // ObjTransport
    // ---------------------------------------------------------

    ObjTransport::ObjTransport(std::string name, std::string ipAddress, uint16_t port, uint32_t maxBuffer) :
                        maxBufferAllowed(maxBuffer),                // Max buffer
                        tcpServer(ObjTransport::connectCallback,    // Connection callback function
                                    NET::ErrorSink,                 // Standard error sink
                                    static_cast<int>(port),         // Transporter port
                                    ipAddress,                      // IP Address
                                    reinterpret_cast<void*>(this)), // Pointer to this transporter so callback can access obj data
                        thisServer(ipAddress,                       // IP Address
                                    static_cast<int>(port),         // Transporter port
                                    name)                           // Transporter name
    {
    }

    // ---------------------------------------------------------
    // ~ObjTransport
    // ---------------------------------------------------------

    ObjTransport::~ObjTransport() {

        for(auto & i : local) {
            i.second = nullptr;
        }
    }

    // ---------------------------------------------------------
    // tick
    // ---------------------------------------------------------

    void ObjTransport::tick() {

        // Tick the server
        tcpServer.tick();

        // Check if there is anything in the queue
        if(!inboundQueue.empty())
        {
            // Get the msgpack data
            std::vector<uint8_t> data = inboundQueue.front();

            try {

                // Take the data and construct the json / remote information
                json obj = json::from_msgpack(data);
                std::string destination = obj["txremote"].get<std::string>();
                std::string senderAddr  = obj["txsender"]["address"].get<std::string>();
                std::string senderName  = obj["txsender"]["name"].get<std::string>();
                int senderPort          = obj["txsender"]["port"].get<int>();

                // Check if the destination exists on this transporter
                if(!(local.find(obj["txremote"]) == local.end())) {

                    // Hand the information to the destination receiver
                    local[destination]->recvMsg(ObjRemote(senderAddr, senderPort, senderName), obj["txdata"]);

                } else {

                    // Warn that the destination doesn't exist on this transporter
                    std::cerr << "Information given to transporter given a destination not held by this transporter" << std::endl;
                }

            } catch(...) {

                // Inform that there was some error decoding the json information
                std::cerr << "Error decoding json. Dropping the information" << std::endl;
            }

            // Pop the item off the queue
            inboundQueue.pop();
        }
    }

    // ---------------------------------------------------------
    // addLocalReceiver
    // ---------------------------------------------------------

    void ObjTransport::addLocalReceiver(std::string name, JRecvIf & recvr) {

        if(local.find(name) == local.end()) {

            local[name] = &recvr;
        }
    }

    // ---------------------------------------------------------
    // addRemote
    // ---------------------------------------------------------

    void ObjTransport::addRemote(ObjRemote remote) {

        outbound[remote.getName()] = remote;
    }

    // ---------------------------------------------------------
    // send
    // ---------------------------------------------------------

    void ObjTransport::send(std::string remote, json data) {

        // Ensure remote destination is stored in our remote map
        if(outbound.find(remote) == outbound.end()) {
            std::cerr << "Remote: " << remote << " not located in outbound map" << std::endl;
            return;
        }

        // Create a data 'wrapper' and wrap the outgoing data with local transporter information
        json obj;
        obj["txremote"] = remote;
        obj["txdata"]   = data;
        obj["txsender"] = thisServer.asJson();

        // Convert to msgpack
        outboundMsgPack = json::to_msgpack(obj);

        // Get size and construct 4 bytes that represent a uint32
        uint32_t vsize = outboundMsgPack.size();

        dataBuff[0] = ( vsize  & 0x000000FF);
        dataBuff[1] = ( vsize  & 0x0000FF00) >> 8;
        dataBuff[2] = ( vsize  & 0x00FF0000) >> 16;
        dataBuff[3] = ( vsize  & 0xFF000000) >> 24;

        // Construct lambda that requests a write to the remote server
        NET::StdWriter tcpWriter([](NET::Socket writer, void* cb) {

            // The transporter object fed into the writer, given back to us
            ObjTransport *obj = reinterpret_cast<ObjTransport*>(cb);

            // Write the data size-buffer constructed above
            writer.socketWriteOut( static_cast<void*>(obj->dataBuff) , 4);

            // Write the msgpack data that was constructed above
            writer.socketWriteOut( static_cast<void*>(obj->outboundMsgPack.data()), obj->outboundMsgPack.size());

        } , NET::ErrorSink,
                NET::StdWriterType::TCP,        // Using TCP
                outbound[remote].getAddress(),  // Remote address
                outbound[remote].getPort(),     // Remote port
                true,                           // Send as-soon-as ready
                reinterpret_cast<void*>(this)); // Hand a pointer to this transporter so it can be used in the lambda
    }

    // ---------------------------------------------------------
    // connectCallback
    // ---------------------------------------------------------

    void ObjTransport::connectCallback(NET::Socket clientSocket, void* cbObj) {

        // This is a static method b/c NET::TcpServer, so 'this' is given as a void*
        // Now we have 'this' .. essentially
        ObjTransport *obj = reinterpret_cast<ObjTransport*>(cbObj);

        // Buffer to get the 4 bytes that comprise the buffer size about to be sent
        uint8_t sizeData[4];

        // get the uint8s representing the size
        clientSocket.socketReadIn(sizeData, 4);

        // Construct expected buffer size
        uint32_t actualSize = sizeData[0] | (sizeData[1] << 8) | (sizeData[2] << 16) | (sizeData[3] << 24);

        // Ensure we can handle a buffer that size
        if(actualSize > obj->maxBufferAllowed)
        {
            // Warn someone
            std::cerr << obj->thisServer.getName() << " : Data exceeds maximum allowed buffer. Dropping connection" << std::endl;

            // Close
            clientSocket.socketClose();
            return;
        }

        // Create the data buffer
        uint8_t * dataBuff = (uint8_t*)malloc( actualSize * sizeof(uint8_t));

        // Read in the data
        clientSocket.socketReadIn(dataBuff, actualSize);

        // Convert the data to a vector (required by json)
        std::vector<uint8_t> msgPack (dataBuff, dataBuff + actualSize);

        // Queue the data for later processing
        obj->inboundQueue.push(msgPack );

        // Free the memory
        free(dataBuff);

        // Close
        clientSocket.socketClose();
    }
}