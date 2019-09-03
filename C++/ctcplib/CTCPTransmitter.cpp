#include "CTCPCommon.h"
#include "CTCPTransmitter.hpp"

#include <cstring>
#include <vector>

#ifdef CTCPLIB_DEBUG
    #include <iostream>
#endif

namespace CTCP
{

    namespace
    {
        int     socket_desc;
        struct  sockaddr_in remote;
        char    remote_ip[20];
        int     remote_port;
        bool    good_params;

        void debug(std::string d)
        {
#ifdef CTCPLIB_DEBUG
            std::cout << d << std::endl;
#endif
        }
    }

    // ----------------------------------------------------------------------------------
    // Transmitter
    // ----------------------------------------------------------------------------------
    Transmitter::Transmitter(std::string ipAddress, int port) : initd(false)
    {
        if(ipAddress.size() > 20 || port < 0)
        {
            good_params = false;
        }
        else
        {
            strcpy(remote_ip, ipAddress.c_str());
            remote_port = port;
            good_params = true;
        }
    }

    // ----------------------------------------------------------------------------------
    // ~Transmitter
    // ----------------------------------------------------------------------------------
    Transmitter::~Transmitter()
    {
    }

    // ----------------------------------------------------------------------------------
    // init
    // ----------------------------------------------------------------------------------
    bool Transmitter::init()
    {
        if(!good_params)
        {
            return good_params;
        }

        socket_desc = socket(AF_INET,
                             SOCK_STREAM,
                             0);
        
        if(socket_desc == -1)
        {
            // Could not create socket
            return false;
        }

        remote.sin_addr.s_addr  = inet_addr(remote_ip);
        remote.sin_family       = AF_INET;
        remote.sin_port         = htons(remote_port);
        
        this->initd = true;
        return this->initd;
    }

    // ----------------------------------------------------------------------------------
    // transmit
    // ----------------------------------------------------------------------------------
    bool Transmitter::transmit(std::string data)
    {
        if(!initd)
        {
            debug(std::string(
                "Class not initialized!"
                        ));
            return initd;
        }

        // Divide the data into however many chunks is required
        // based on the max data length defined by CTCPLIB_MAX_CHUNK_SIZE
        int remainder = 0;
        std::vector< std::string> chunks;
        if(data.size() > CTCPLIB_STD_CHUNK_SIZE)
        {
            remainder = data.size() % CTCPLIB_STD_CHUNK_SIZE;
            int divisions = (data.size() - remainder) / CTCPLIB_STD_CHUNK_SIZE;

            for(int i = 0; i < divisions; i++)
            {
                chunks.push_back(data.substr( i * CTCPLIB_STD_CHUNK_SIZE, 
                                              CTCPLIB_STD_CHUNK_SIZE));
            }

            if(remainder != 0)
            {
                std::string trailingData = data.substr( data.size() - remainder, remainder);
                chunks.push_back(trailingData);
            }
        }
        else
        {
            chunks.push_back(data);
        }

        // Get chunk count
        std::string chunkCnt = std::to_string(chunks.size());
        std::string reminCnt = std::to_string(remainder);

        debug(std::string("About to send [" + chunkCnt + std::string("] chunks")));

        if(connect(socket_desc, (struct sockaddr *)&remote, sizeof(remote)) < 0)
        {
            // Could not connect
            debug(std::string(
                        "Could not connect"
                        ));
            return false;
        }

        // Inform the server how many chunks are to be expected
        char * chunkCount = &chunkCnt[0];
        if(send(socket_desc, chunkCount, strlen(chunkCount), 0) < 0)
        {
            debug(std::string(
                        "Sending chunk count failed"
                        ));
            close(socket_desc);
            return false;
        }

        // Confirm that they received size, and are expecting the chunks
        char remote_reply[CTCPLIB_STD_REPLY_SIZE];
        if(recv(socket_desc, remote_reply, CTCPLIB_STD_REPLY_SIZE, 0) < 0)
        {
            debug(std::string(
                        "Failed to get a reply from the server"
                        ));
            close(socket_desc);
            return false;
        }

        // Inform the server how many garbage are to be expected
        char * remCount = &reminCnt[0];
        if(send(socket_desc, remCount, strlen(remCount), 0) < 0)
        {
            debug(std::string(
                        "Sending garbage count failed"
                        ));
            close(socket_desc);
            return false;
        }

        // Confirm that they received size, and are expecting the chunks
        char remote_reply0[CTCPLIB_STD_REPLY_SIZE];
        if(recv(socket_desc, remote_reply0, CTCPLIB_STD_REPLY_SIZE, 0) < 0)
        {
            debug(std::string(
                        "Failed to get a reply from the server"
                        ));
            close(socket_desc);
            return false;
        }

        // Send chunks
        for(int i = 0; i < chunks.size(); i++)
        {
            char *chunk = &chunks[i][0];

            debug(std::string("Sending chunk : ") + std::string(chunk));

            if(send(socket_desc, chunk, strlen(chunk), 0) < 0)
            {
                debug(std::string("Failed sending chunks @ chunk: " + std::to_string(i)));
                close(socket_desc);
                return false;
            }
        }

        // Confirm delivery of chunks
        char remote_confirmation[CTCPLIB_STD_REPLY_SIZE];
        if(recv(socket_desc, remote_confirmation, CTCPLIB_STD_REPLY_SIZE, 0) < 0)
        {
            debug(std::string(
                        "Failed to get send confirmation from the server"
                        ));
            close(socket_desc);
            return false;
        }

        debug(std::string("All chunks away!"));

        // Go home
        close(socket_desc);
        return true;
    }

}
