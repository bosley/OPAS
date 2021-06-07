#ifndef NET_WRITER
#define NET_WRITER

#include "NetSocket.hpp"

#include <string>
#include <functional>

//!
//! \file NETWriter.hpp
//! \brief A network writer that can write TCP and UDP messages
//!
namespace NET
{
    //!
    //! \brief Enum for setting writer type
    //!
    enum class StdWriterType
    {
        TCP,    //! A TCP writer
        UDP     //! A UDP writer
    };

    //!
    //! \class StdWriter
    //! \brief StdWriter creator for TCP and UDP connections 
    //!
    class StdWriter : public Socket
    {
    public:
        //!
        //! \brief Construct a writer - Failing to bind request sockets on local machine will 
        //!                             Kill the software. 
        //! \param writerReady Callback that will be triggered by callWriter() 
        //! \param errorCb Callback when an error occurs
        //! \param wtype The writer type
        //! \param ip The IP to bind
        //! \param port Port to listen on
        //! \param callWhenReady Call the writer as soon as it is ready
        //!
        StdWriter(std::function<void(Socket writer, void*cb)> writerReady,
                std::function<void(SocketError)> errorCb, 
                StdWriterType wtype, 
                std::string ip, 
                int port,
                bool callWhenReady = false,
                void*cb=nullptr);

        //!
        //! \brief Deconstruct a writer
        //!
        ~StdWriter();

        //!
        //! \brief Triggers a callback to the writer
        //!        Note: The user must keep in-mind that errors could be returned on construction
        //!              Depending on any errors generated, this might not yeild useable results.
        //!
        void callWriter();

        //!
        //! \brief Close the connection
        //! \post Writer will be unusable after this call
        //!
        void close();

    private:
        StdWriterType writerType;
        std::string ip;
        int port;
        std::function<void(SocketError)> errCb;
        std::function<void(Socket writer, void*cb)> writerReady;
        bool ready;
        void*cb;
    };
}


#endif