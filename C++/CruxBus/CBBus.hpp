#ifndef CBBBUS_HEADER_G
#define CBBBUS_HEADER_G

#include "CBAddress.hpp"
#include "CBMessage.hpp"
#include "CBTypes.hpp"

//!
//! \file CBBus.hpp
//!
namespace CB
{
    //!
    //! \class Bus An object that acts as a data bus for
    //!        multiple types of communication 
    //!
    class Bus
    {
        public:
            //!
            //! \brief Construc a bus
            //!
            //! \param whiteList List of addresses initially allowed on the bus
            //!
            Bus(AddressList whiteList);
            ~Bus();

            //!
            //! \brief Add allowed address object
            //!
            //! \param addr Address object to add to the bus
            //!
            //! \retval true Item was added to list
            //!
            //! \retval false Item was not added to list
            //!
            const bool addNewAddress(Address const addr);

            //!
            //! \brief Remove an address from the bus
            //!
            //! \param addr Address object to remove
            //!
            //! \retval true Item was removed from list
            //!
            //! \retval false Item was not removed from list
            //!
            const bool removeExistingAddress(Address const addr);

            //!
            //! \breif Check if bus contains addresses
            //!
            //! \retval true Bus contains no addresses
            //!
            //! \retval false Bus contains addresses
            //!
            const bool containsAddresses();

            //!
            //! \breif Check if bus contains messages
            //!
            //! \retval true Bus contains messages
            //!
            //! \retval false Bus contains messages
            //!
            const bool containsMessages();

            //!
            //! \brief Check if an address exists
            //!
            //! \param addr Address to check for existence
            //!
            //! \retval true Address exists
            //!
            //! \retval false Address does not exist
            //!
            const bool isExistingAddress(Address const addr);

            //!
            //! \brief Write a message to the bus
            //!
            //! \param aFor Address object of the receiver
            //!
            //! \param msg CB::Message to place in the bu
            //!
            //! \retval true The message was able to be placed
            //!
            //! \post If returned true, the message will be 
            //!       placed, and accessible to msg.to address
            //!
            const bool writeMessage(Address const aFor, Message msg);

            //!
            //! \brief Reads the first message found from the bus
            //!
            //! \param[in] aFor Address object of the receiver
            //!
            //! \param[out] msg A bus message
            //!
            //! \retval true Message was read
            //!
            //! \retval false Message was not read
            //!
            //! \post Message remains in bus storage
            //!
            const bool readMessage(Address const aFor, Message &msg);

            //!
            //! \brief Gets the first message from the bus
            //!
            //! \param[in] aFor Address object of the receiver
            //!
            //! \param[out] msg A bus message
            //!
            //! \retval true Message was gotten
            //!
            //! \retval false Message was not gotten
            //!
            //! \post Message will no longer be in the bus after read
            //!
            const bool getMessage(Address const aFor, Message &msg);

            //!
            //! \brief Reads all the messages from the bus for receiver
            //!
            //! \param[in] aFor Address object of the receiver
            //!
            //! \param[out] msgs MessageList of all messages 
            //!
            //! \retval true Messages were read
            //!
            //! \retval false Messages were not read
            //!
            //! \post Messages remain in the bus storage 
            //!
            const bool readAllMessages(Address const aFor, MessageList &msgs);

            //!
            //! \brief Gets all the messages from the bus for receiver
            //!
            //! \param[in] aFor Address object of the receiver
            //!
            //! \param[out] msgs MessageList of all messages
            //!
            //! \retval true Messages were gotten
            //!
            //! \retval false Messages were not gotten
            //!
            //! \post Messages for receiver will no longer be in bus storage
            //!
            const bool getAllMessages(Address const aFor, MessageList &msgs);

        private:
            // Map of message vectors
            TYPES::AddrMsgMap busData;

            // Add address to busData
            void addAddress(Address const addr);

            // Remove address from busData
            void removeAddress(Address const addr);
    };
}


#endif
