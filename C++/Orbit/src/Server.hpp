//
// Created by bosley on 3/28/20.
//

#ifndef ORBIT_SERVER_HPP
#define ORBIT_SERVER_HPP

#include "HostPort.hpp"
#include "RequestHandler.hpp"

#include <bslnet/NetTcpServer.hpp>

#include <string>
#include <atomic>
#include <mutex>
#include <thread>

namespace ORBIT
{
    class Server {
    public:
        Server(HostPort hostPortInformation,
                RequestHandler &reqHandler,
                uint64_t maxBuffer=65536,
                uint64_t maxThreads=20);

    private:
        HostPort hp;
        uint64_t maxBuffer;
        NET::TcpServer server;

        uint64_t maxThreads;
        std::atomic<uint64_t> numThreads;
        RequestHandler &requestHandler;
        std::mutex threadCountMutex;

        bool reserveThread();
        void markThreadComplete();

        static void connectCallback(NET::Socket sock, void* cbObj);

        static void handleClient(Server * thisServer, NET::Socket socket);
    };
}


#endif //ORBIT_SERVER_HPP
