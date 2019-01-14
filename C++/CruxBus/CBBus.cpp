#include "CBBus.hpp"
#include "CBAddress.hpp"

namespace CB
{
    // ========================[PUBLIC]==================================

    // ------------------------------------------------------------------
    // Bus() : public
    // ------------------------------------------------------------------
    Bus::Bus(AddressList whiteList)
    {
        // Construct the busData with Address objects mapping to message object vectors
        for(const Address addr: whiteList)
        {
            addAddress(addr);
        }
    }

    // ------------------------------------------------------------------
    // ~Bus() : public
    // ------------------------------------------------------------------
    Bus::~Bus()
    {

    }

    // ------------------------------------------------------------------
    // addNewAddress() : public
    // ------------------------------------------------------------------
    const bool Bus::addNewAddress(Address const addr)
    {
        if(isExistingAddress(addr))
        {
            return false;
        }
        addAddress(addr);
        return true;
    }

    // ------------------------------------------------------------------
    // removeExistingAddress() : public
    // ------------------------------------------------------------------
    const bool Bus::removeExistingAddress(Address const addr)
    {
        if(isExistingAddress(addr))
        {
            removeAddress(addr);
            return true;
        }
        return false;
    }

    // ------------------------------------------------------------------
    // containsAddresses() : public
    // ------------------------------------------------------------------
    const bool Bus::containsAddresses()
    {
        return !busData.empty();
    }

    // ------------------------------------------------------------------
    // containsMessages() : public
    // ------------------------------------------------------------------
    const bool Bus::containsMessages()
    {
        if(!containsAddresses())
        {
            return false;
        }

        for(const auto &item : busData)
        {
            if(!item.second.empty())
            {
                return true;
            }
        }
        return false;
    }

    // ------------------------------------------------------------------
    // writeMessage() : public
    // ------------------------------------------------------------------
    const bool Bus::writeMessage(Address const aFor, Message msg)
    {
        const auto i = busData.find(aFor);
        if(i == busData.end())
        {
            return false;
        }

        busData[aFor].push_back(msg);
        return true;
    }

    // ------------------------------------------------------------------
    // readMessage() : public
    // ------------------------------------------------------------------
    const bool Bus::readMessage(Address const aFor, Message &msg)
    {
        const auto i = busData.find(aFor);
        if(i == busData.end())
        {
            return false;
        }

        msg = i->second.front();
        return true;
    }

    // ------------------------------------------------------------------
    // getMessage() : public
    // ------------------------------------------------------------------
    const bool Bus::getMessage(Address const aFor, Message &msg)
    {
        const auto i = busData.find(aFor);
        if(i == busData.end())
        {
            return false;
        }

        msg = i->second.front();
        i->second.erase(i->second.begin());
        return true;
    }

    // ------------------------------------------------------------------
    // readMessages() : public
    // ------------------------------------------------------------------
    const bool Bus::readAllMessages(Address const aFor, MessageList &msgs)
    {
        const auto i = busData.find(aFor);
        if(i == busData.end())
        {
            return false;
        }
        msgs.clear();
        msgs = i->second;
        return true;
    }

    // ------------------------------------------------------------------
    // getMessages() : public
    // ------------------------------------------------------------------
    const bool Bus::getAllMessages(Address const aFor, MessageList &msgs)
    {
        const auto i = busData.find(aFor);
        if(i == busData.end())
        {
            return false;
        }

        if(i->second.empty())
        {
            return false;
        }

        msgs.clear();
        msgs  = MessageList(i->second);
        i->second.clear();
        return true;
    }

    // ------------------------------------------------------------------
    // isExistingAddress() : public
    // ------------------------------------------------------------------
    const bool Bus::isExistingAddress(Address const addr)
    {
        auto const i = busData.find(addr);
        return( i != busData.end() );
    }

    // =======================[PRIVATE]==================================

    // ------------------------------------------------------------------
    // addAddress() : private
    // ------------------------------------------------------------------
    void Bus::addAddress(Address const addr)
    {
        busData.insert( TYPES::AddrMsgPair(addr, MessageList() ) );
    }

    // ------------------------------------------------------------------
    // removeAddress() : private
    // ------------------------------------------------------------------
    void Bus::removeAddress(Address const addr)
    {
        busData.erase(addr);
    }
}
