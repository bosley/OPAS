#ifndef CBTYPES_HEADER_G
#define CBTYPES_HEADER_G

#include <iterator>
#include <map>
#include <utility>
#include <queue>

//!
//! \file CBTypes.hpp 
//! \brief typedefs used throughout the Crux Bus implementation
//! 

//!
//! \namespace CB 
//!
//! \brief The Crux Bus namespace, containing all of Crux Bus
//!
namespace CB
{
    class Message;
    class Address;

    //!
    //! \namespace TYPES 
    //!
    //! \brief Contains all of the types used in the Crux Bus implementation
    //!
    namespace TYPES
    {
        typedef std::pair< CB::Address, 
                           std::vector< Message > > AddrMsgPair;

        typedef std::map< CB::Address,
                          std::vector< Message > > AddrMsgMap;

        typedef uint8_t byte;

        typedef std::vector< byte > bytes;
    }
}

#endif
