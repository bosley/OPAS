#ifndef TSGP_BUS_IMPL_HEADER_G
#define TSGP_BUS_IMPL_HEADER_G

#include "TSGPInterface.hpp"
#include "TSGPReceiver.hpp"

#include <atomic>
#include <map>
#include <mutex>

//!
//! \file TSGPBusImpl.hpp
//!
//! \brief A basic implementation of the CB::Interface object. 
//!        This, along with CB::Interface demonstrates a how to 
//!        setup a thread safe get-primary bus
//!
namespace TSGP
{
    //!
    //! \class BusImpl
    //!
    //! \brief A get-primary crux bus implementation
    //!
    class BusImpl : private Interface
    {
        public:
            //! 
            //! \breif BusImpl constructor
            //! 
            //! \param whiteList The initial white list for a bus object
            //! 
            //! \post Superclass Interface will initialize a bus object 
            //!
            BusImpl(CB::AddressList whiteList);
            
            //!
            //! \brief BusImpl destructor
            //!
            //! \post Superclass Interface will be destructed, and bus deleted
            //!
            ~BusImpl();

            //!
            //! \brief Register a callback with the Crux Bus implementation to receive information
            //!
            //! \param aFor The address that the callback is for
            //!
            //! \param receiver The TSGPReceiver to push messages to
            //!
            //! \retval true Callback is new, and registered
            //!
            //! \retval false Callback exists, and not registered
            //!
            //! \post Callback will be executed by autopolling if messages present
            //!
            const bool registerRecvCallback(CB::Address const aFor, Receiver *callback);

            const bool start();

            const bool stop();

            const bool writeBusMessage(CB::Address const aFor, CB::Message const msg);

        private:
            //!
            //! \copydoc Interface::addAddress
            //!

            // Inherited :: addAddress(Address const addr)

            //!
            //! \copydoc Interface::delAddress
            //!

            // Inherited :: delAddress(Address const addr)

            //!
            //! \copydoc Interface::writeMessage
            //!

            // Inherited :: writeMessage(Address const aFor, Message const msg)

            //! 
            //! \copydoc Interface::pollForMessages
            //! 

            // Inherited :: pollForMessages(Address const aFor, MessageList &list)
            
            std::mutex callbackMutex;
            std::map< CB::Address, Receiver* > callbacks; 
            std::atomic<bool> running;
    };

}

#endif
