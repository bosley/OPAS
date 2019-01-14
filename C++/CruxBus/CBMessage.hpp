#ifndef CBMESSAGE_HEADER_H
#define CBMESSAGE_HEADER_H

#include "CBAddress.hpp"
#include "CBTypes.hpp"
#include <vector>

//!
//! \file CBMessage.hpp
//!
namespace CB
{
    //!
    //! \class Message
    //!
    //! \brief A bus message
    //!
    class Message
    {
        public:

            //!
            //! \enum PRIORITY
            //!
            //! \brief Represents message priority
            //!
            enum class PRIORITY
            {
                LOW,
                MEDIUM,
                HIGH,
                EXTREME,
                NONE
            }; 

            //!
            //! \brief Constructs a message
            //!
            //! \param from Address of sender
            //!
            //! \param priority The priority of the message
            //!
            //! \param data The message data
            //!
            Message(Address from, PRIORITY priority, TYPES::bytes data) : 
                                                       from(from),
                                                       messagePriority(priority),
                                                       data(data) 
            {
            }
            
            //!
            //! \brief Constructs a message
            //!
            //! \post A message object with nothing set is created
            //!
            Message() : from(CB_DEFAULT_ADDRESS),
                        messagePriority(PRIORITY::NONE),
                        data(TYPES::bytes())
            {}

            //!
            //! \brief Destructs a message
            //!
            ~Message() = default;

            //!
            //! \breif Address object reprecenting the sender
            //!
            Address from;

            //!
            //! \breif PRIOORITY enumeration indicating the priority of the message
            //!        Default = PRIORITY::NONE
            //!
            PRIORITY messagePriority = PRIORITY::NONE;

            //!
            //! \breif Message data
            //!
            TYPES::bytes data;

            //!
            //! \brief Operator = for Message object
            //!
            Message& operator= (const Message &rhs)
            {
                this->from = rhs.from;
                this->messagePriority = rhs.messagePriority;
                this->data = rhs.data;
                return *this;
            }

            bool operator== (const Message &rhs)
            {
                return (this->from == rhs.from 
                        && this->messagePriority == rhs.messagePriority 
                        && this->data == rhs.data);
            }
    };

    //!
    //! \brief Typedef a list of Message objects
    //!
    typedef std::vector< Message > MessageList;
}

#endif
