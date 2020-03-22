#ifndef BUTANE_NET_WRITER
#define BUTANE_NET_WRITER

#include "NETSocket.hpp"

#include <string>
#include <functional>

//!
//! \file NETWriter.hpp
//! \brief A network writer that can write TCP and UDP messages
//!
namespace BUTANE
{
    namespace NET
    {
        //!
        //! \brief Enum for setting writer type
        //!
        enum class WriterType
        {
            TCP,    //! A TCP writer
            UDP     //! A UDP writer
        };

        //!
        //! \class Writer
        //! \brief Writer creator for TCP and UDP connections 
        //!
        class Writer : public Socket
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
            //!
            Writer(std::function<void(Socket writer)> writerReady,
                   std::function<void(SocketError)> errorCb, 
                   WriterType wtype, 
                   std::string ip, 
                   int port);

            //!
            //! \brief Deconstruct a writer
            //!
            ~Writer();

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
            WriterType writerType;
            std::string ip;
            int port;
            std::function<void(SocketError)> errCb;
            std::function<void(Socket writer)> writerReady;
            bool ready;
        };
    }
}


#endif