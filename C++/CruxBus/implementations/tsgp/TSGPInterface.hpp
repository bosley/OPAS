#ifndef CBSYSTEM_IF_HEADER_G
#define CBSYSTEM_IF_HEADER_G

#include "CBBus.hpp"
#include "CBTypes.hpp"
#include "CBAddress.hpp"
#include "CBMessage.hpp"

#include <mutex>

//!
//! \file Interface.hpp
//!
//! \brief Wraps a basic CB::Bus for a thread safe get-based usage 
//!        of the bus. This is used as an IF to abstract away the 
//!        bus operations from the implementation of interactions
//!        between the bus and the users.
//!
namespace TSGP
{
    class Interface
    {
        public:

            //!
            //! \brief Creates a new interface
            //!
            //! \param initialWhiteList The initial address list to create
            //!        the data bus with
            //!
            //! \post A Bus will be created
            //!
            Interface(CB::AddressList initialWhiteList)
            {
                dataBus = new CB::Bus(initialWhiteList);
            }

            //!
            //! \brief Destructs a the interface
            //!
            //! \post The Bus will be deleted
            //!
            ~Interface()
            {
                delete dataBus;
            }

            //!
            //! \brief Add address to bus
            //!
            //! \param addr Address to add 
            //!
            //! \retval true Address added
            //!
            //! \retval false Address unable to be added
            //!
            const bool addAddress(CB::Address const addr)  
            {
                // Lock used in add to ensure changing of bus size while
                // iterating is stopped
                std::lock_guard<std::mutex> lock(busMutex);
                return dataBus->addNewAddress(addr);
            }
            
            //!
            //! \brief Delete address from bus
            //!
            //! \param addr Address to delete 
            //!
            //! \retval true Address deleted
            //!
            //! \retval false Address unable to be deleted
            //!
            const bool delAddress(CB::Address const addr)  
            {
                std::lock_guard<std::mutex> lock(busMutex);
                return dataBus->removeExistingAddress(addr);
            }

            //!
            //! \brief Add address to bus
            //!
            //! \param aFor Address of receiver
            //!
            //! \param msg Message to write on the bus
            //!
            //! \retval true Message written
            //!
            //! \retval false Message failed to write
            //!
            const bool writeMessage(CB::Address const aFor, CB::Message const msg)  
            {
                std::lock_guard<std::mutex> lock(busMutex);
                return dataBus->writeMessage(aFor, msg);
            }

            //!
            //! \brief Poll the bus for messages
            //!
            //! \param[in] aFor The address to get messages for
            //!
            //! \param[out] list Message list to be populated
            //!
            //! \retval true Messages existed, and list is populated
            //!
            //! \retval false No messages on the bus, list is empty
            //!
            const bool pollForMessages(CB::Address const aFor,  CB::MessageList &list)  
            {
                list.clear();
                std::lock_guard<std::mutex> lock(busMutex);
                return dataBus->getAllMessages(aFor, list);
            }

        private:
            std::mutex busMutex;
            CB::Bus * dataBus;
    };

}

#endif
