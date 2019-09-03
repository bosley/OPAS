#include "CTCPThreadedTcpServer.hpp"
#include "CTCPTypes.hpp"
#include "CTCPCommon.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

namespace CTCP
{
    namespace
    {
        void debug(std::string d)
        {
#ifdef CTCPLIB_DEBUG
            std::cout << d << std::endl;
#endif
        }
    }

    // ---------------------------------------------------------------------------------
    // ThreadedTcpServer
    // ---------------------------------------------------------------------------------
    ThreadedTcpServer::ThreadedTcpServer(int port, int maxConnections) :  
                                                                       port(port),
                                                                       maxConnections(maxConnections),
                                                                       connectionCount(0)   
    {}

    // ---------------------------------------------------------------------------------
    // ~ThreadedTcpServer
    // ---------------------------------------------------------------------------------
    ThreadedTcpServer::~ThreadedTcpServer()
    {}

    // ---------------------------------------------------------------------------------
    // registerMessageReceiver
    // ---------------------------------------------------------------------------------
    void ThreadedTcpServer::registerMessageReceiver(ReceiverIf *receiver)
    {
        receivers.push_back(receiver);
    }

    // ---------------------------------------------------------------------------------
    // serve
    // ---------------------------------------------------------------------------------
    bool ThreadedTcpServer::serve()
    {
        // Create socket stuff
        int socket_desc, 
            new_socket,
            client_socket_len,
            *threaded_socket;

        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if(socket_desc == -1)
        {
            debug(std::string("Unable to create socket"));
            return false;
        }

        struct sockaddr_in server, client;

        server.sin_family       = AF_INET;
        server.sin_addr.s_addr  = INADDR_ANY;
        server.sin_port         = htons(this->port);

        // Bind
        if( bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
        {
            debug(std::string("Unable to bind"));
            return false;
        }

        // Start listening
        listen(socket_desc, CTCPLIB_LISTEN_BACKLOG);

        client_socket_len = sizeof(sockaddr_in);

        // Whenever someone new arrives, spawn a thread for them
        while( (new_socket = 
                    accept(socket_desc, 
                           (struct sockaddr *)&client, 
                           (socklen_t*)&client_socket_len) 
                ))
        {
            if(connectionCount < maxConnections)
            {
                int         clientPort    = ntohs(client.sin_port);
                std::string clientAddress = inet_ntoa(client.sin_addr);
            
                pthread_t starter_thread;
                threaded_socket = (int*)malloc(sizeof(int));
                *threaded_socket = new_socket;

                HandleData hd {threaded_socket, this, clientPort, clientAddress};

                if(pthread_create(&starter_thread, NULL, handler, (void*)&hd) < 0)
                {
                    debug(std::string("Unable to create thread"));
                    return false;
                }

                debug(std::string("-----------[Client Connected]----------"));
                connectionCount++;
                debug(std::string("Connection count: ") + std::to_string(connectionCount));
            }
        }

        if(threaded_socket < 0)
        {
            debug(std::string("Accept failed"));
            return false;
        }

        return true;
    }

    // ---------------------------------------------------------------------------------
    // safeStringToInt
    // ---------------------------------------------------------------------------------
    int ThreadedTcpServer::safeStringToInt(std::string s)
    {
        int size = -1;
        try
        {
            size = std::stoi(s);
        }
        catch (std::invalid_argument const &e)
        {
            debug(std::string("Error receiving chunk size -> Non integer given for chunk length"));
            return -1;
        }
        catch (std::out_of_range const &e)
        {
            debug(std::string("Error receiving chunk size -> Non integer given for chunk length"));
            return -1;
        }
        return size;
    }

    // ---------------------------------------------------------------------------------
    // handler
    // ---------------------------------------------------------------------------------
    void *ThreadedTcpServer::handler(void* handlerData)
    {
        HandleData *hd = static_cast<HandleData*>(handlerData);
        int sock = *(int*) hd->sock_desc;

        int read_size;
        int chunk_size = -1;
        char chunkLen[CTCPLIB_STD_CHUNK_TELL];

        // Get the number of chunks that they intend on delivering
        read_size = recv(sock, chunkLen, CTCPLIB_STD_CHUNK_TELL, 0);

        if(read_size == 0)
        {
            debug(std::string("Failed to read"));
            free(hd->sock_desc);
            return nullptr;
        }
        
        chunk_size = hd->owner->safeStringToInt(std::string(chunkLen));

        if(chunk_size <= 0)
        {
            debug(std::string("Error receiving chunk size"));
            write(sock, CTCPLIB_STD_REPLY_DENY, strlen(CTCPLIB_STD_REPLY_DENY));
            free(hd->sock_desc);
            return nullptr;
        }

        // This is a ridiculous chunk, so only include if want to debug
#ifdef CTCPLIB_DEBUG
        debug(std::string(hd->clientAddress + 
                std::string(":") +
                std::to_string(hd->clientPort) +
                std::string(" | Wants to send [") + 
                std::to_string(chunk_size) + 
                std::string("] chunks"))
                ); 
#endif

        // Inform the client that we got their size
        write(sock, CTCPLIB_STD_REPLY_CONFIRM, strlen(CTCPLIB_STD_REPLY_CONFIRM));

        // Receive the information telling us how much of the last chunk matters
        int  trail_size;
        char trailLen[CTCPLIB_STD_CHUNK_TELL];
        read_size = recv(sock, trailLen, CTCPLIB_STD_CHUNK_TELL, 0);

        if(read_size == 0)
        {
            debug(std::string("Failed to read"));
            free(hd->sock_desc);
            return nullptr;
        }
        
        trail_size = hd->owner->safeStringToInt(std::string(trailLen));

        if(trail_size < 0)
        {
            debug(std::string("Error receiving trail size"));
            write(sock, CTCPLIB_STD_REPLY_DENY, strlen(CTCPLIB_STD_REPLY_DENY));
            free(hd->sock_desc);
            return nullptr;
        }

        // Inform the client that we got their size
        write(sock, CTCPLIB_STD_REPLY_CONFIRM, strlen(CTCPLIB_STD_REPLY_CONFIRM));

        // Receive all the chunks

        Chunks receivedChunks;
        char chunk[CTCPLIB_STD_CHUNK_SIZE];

        for(int i = 0; i < chunk_size; i++)
        {
            read_size = 0;

            memset(&chunk[0], 0, sizeof(CTCPLIB_STD_CHUNK_SIZE));

            read_size = recv(sock, chunk, CTCPLIB_STD_CHUNK_SIZE, 0);

            if(read_size == 0)
            {
                debug(std::string("Failed to read a chunk - aborting"));
                free(hd->sock_desc);
                return nullptr;
            }
             
            std::string schunk = std::string(chunk);

            if(i == chunk_size-1 && trail_size > 0)
            {
                schunk = schunk.substr(0, trail_size);
            }

            receivedChunks.push_back(schunk);
        }

#ifdef CTCPLIB_DEBUG
        debug(std::string("Received the following chunks:"));
        for(int i = 0; i < receivedChunks.size(); i++)
        {
            debug(receivedChunks[i]);
        }
#endif

        // Create a data block
        MessageData clientMessageData { hd->clientAddress,
                                        hd->clientPort,
                                        receivedChunks };

        // Pass the messages to the receivers
        for(int i = 0; i < hd->owner->receivers.size(); i++)
        {
            hd->owner->receivers[i]->receive(clientMessageData);
        }

        // Dec the active thread counter
        hd->owner->connectionCount--;

        // Free the sock and exit
        free(hd->sock_desc);
    }
}
