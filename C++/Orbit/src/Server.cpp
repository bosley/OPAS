//
// Created by bosley on 3/28/20.
//

#include "Server.hpp"
#include "Request.hpp"

#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace ORBIT
{
    Server::Server(HostPort hp, RequestHandler &reqHandler, uint64_t maxBuffer, uint64_t maxThreads) :
                                        hp(hp),                                              // Server host/port info
                                        maxBuffer(maxBuffer),                                // Maximum buffer tansfer allowed
                                        server(Server::connectCallback,                      // Connection callback function
                                                  NET::ErrorSink,                            // Standard error sink
                                                  static_cast<int>(hp.getPort()),            // Transporter port
                                                  hp.getHost(),                              // IP Address
                                                  reinterpret_cast<void*>(this)), // Pointer to this transporter so callback can access obj data
                                                  maxThreads(maxThreads),
                                                  numThreads(0),
                                                  requestHandler(reqHandler)
    {

    }


    bool Server::reserveThread() {

        std::lock_guard<std::mutex> lg(threadCountMutex);

        if(numThreads >= maxThreads) {

            // We might want to loop and check for a certain amount of time instead of auto-rejecting
            std::cerr << "Too many threads running, dropping connection." << std::endl;

            return false;
        }

        // Reserve a thread
        numThreads += 1;

        return true;
    }

    void Server::markThreadComplete() {

        std::lock_guard<std::mutex> lg(threadCountMutex);

        if(numThreads != 0) {

            numThreads--;
        }
    }

    void Server::connectCallback(NET::Socket socket, void* cbObj) {

        // This is a static method b/c NET::TcpServer, so 'this' is given as a void*
        // Now we have 'this' .. essentially
        Server *obj = reinterpret_cast<Server*>(cbObj);

        if(! obj->reserveThread() ) {

            socket.socketClose();
            return;
        }

        // This can be handled better
        std::thread th(obj->handleClient, obj, socket);
        th.detach();
    }

    void Server::handleClient(Server * thisServer, NET::Socket socket) {

        // Buffer to get the 4 bytes that comprise the buffer size about to be sent
        uint8_t sizeData[4];

        // get the uint8s representing the size
        socket.socketReadIn(sizeData, 4);

        // Construct expected buffer size
        uint32_t actualSize = sizeData[0] | (sizeData[1] << 8) | (sizeData[2] << 16) | (sizeData[3] << 24);

        // Ensure we can handle a buffer that size
        if(actualSize > thisServer->maxBuffer)
        {
            // Warn someone
            std::cerr << "Server [" << thisServer->hp.getHost() << ":" << (int)thisServer->hp.getPort() << "] : Data exceeds maximum allowed buffer. Dropping connection" << std::endl;

            // Close
            socket.socketClose();
            thisServer->markThreadComplete();
            return;
        }

        // Create the data buffer
        uint8_t * dataBuff = (uint8_t*)malloc( actualSize * sizeof(uint8_t));

        // Read in the data
        socket.socketReadIn(dataBuff, actualSize);

        // Convert the data to a vector (required by json)
        //std::vector<uint8_t> msgPack (dataBuff, dataBuff + actualSize);

        // Convert inbound data into a json message
        json jsonMsg;
        try {

            jsonMsg = json::from_msgpack(dataBuff);

        } catch( ... ) {

            std::cerr << "Error converting raw msgPack into json object" << std::endl;

            free(dataBuff);                     // Free the malloc
            socket.socketClose();               // Close the socket
            thisServer->markThreadComplete();   // Mark thread as complete
        }

        // Free the memory
        free(dataBuff);

        // Create a response object
        Response serverResponse;

        // Decode the client request
        try {

            Request clientRequest = jsonMsg.get<Request>();

            std::cout << "Client agent : " << clientRequest.getAgent().getName()     << " : " << clientRequest.getAgent().getVersion() << std::endl;
            std::cout << "\tRequest : "    << clientRequest.getTypeString()          << std::endl;
            std::cout << "\tData    : "    << clientRequest.getData().dump(4) << std::endl;

            // Request handler should't throw exceptions. If its called, it responds.
            serverResponse = thisServer->requestHandler.handleRequest(clientRequest);

        } catch ( ... ) {

            std::cerr << "Server received a malformed request!! " << std::endl;

            serverResponse = Response(Response::Code::REQUEST_MALFORMED,
                                      "Error deserializing request");
        }

        // Create response vector
        std::vector<uint8_t> responseData;

        // Attempt to encode, or if it fails, encode that response
        try {
            responseData = json::to_msgpack(serverResponse);

        } catch ( ... ) {

            // If it failed, attempt a fall-back response. If that fails ... we just stop

            std::cerr << "Server created a malformed response!! " << std::endl;

            Response fail(Response::Code::SERVER_RESPONSE_MALFORMED,
                            "The server produced a result that wasn't able to be encoded");

            try {
                responseData = json::to_msgpack(fail);

            } catch  (... ) {

                std::cerr << "Something horrible is happening and it needs to be investigated.\ "
                             "A fall-back error response errored out on encode and thats just wrong" << std::endl;

                socket.socketClose();
                thisServer->markThreadComplete();
            }
        }

        // Get size and construct 4 bytes that represent a uint32
        uint32_t vsize = responseData.size();

        uint8_t responseSizeBuffer[4];
        responseSizeBuffer[0] = ( vsize  & 0x000000FF);
        responseSizeBuffer[1] = ( vsize  & 0x0000FF00) >> 8;
        responseSizeBuffer[2] = ( vsize  & 0x00FF0000) >> 16;
        responseSizeBuffer[3] = ( vsize  & 0xFF000000) >> 24;

        // Write the size
        socket.socketWriteOut( static_cast<void*>(responseSizeBuffer) , 4);

        // Write the data
        socket.socketWriteOut( static_cast<void*>(responseData.data()), responseData.size());

        // Close
        socket.socketClose();
        thisServer->markThreadComplete();
    }
}