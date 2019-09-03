#ifndef CTCPLIB_TRANSMITTER_H
#define CTCPLIB_TRANSMITTER_H

#ifdef __linux__
    #include <stdio.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#elif _WIN32
    #error "ctcplib::No support for Windows OS"
#else
    #error "ctcplib::Unsupported OS"
#endif

#include <string>

namespace CTCP
{
//!
//! \class Transmitter
//! \brief Transmits data to a CTCP server object using TCP
//!
class Transmitter
{
public:
    //!
    //! \brief Construct a transmitter
    //! \param ipAddress  The dotted ip address of receiver
    //! \param port       The port of the receiver address
    //!
    Transmitter(std::string ipAddress, int port);

    //!
    //! \brief Destruct a transmitter
    //!
    ~Transmitter();

    //!
    //! \brief Initialize the transmitter
    //! \post  transmit() will be able to be called
    //!
    bool init();

    //!
    //! \brief Sends a string of data. 
    //! \pre   init() must have been called.
    //! \post  data will be sent 
    //!
    bool transmit(std::string data);

private:
    bool initd;
};

}



#endif
