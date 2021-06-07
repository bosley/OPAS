#ifndef MSG_WRAPPER_HPP
#define MSG_WRAPPER_HPP

#include <nlohmann/json.hpp>

//!
//! \file Wrapper.hpp
//! \brief Wrappers a message for network sending. This is classed-out rather than done in-place
//!        to ensure that any future changes to packing can be done in a single location
//!
namespace OBELUS
{
    enum class TransportMethod
    {
        TCP,
        UDP
    };

    struct TxInfo
    {
        std::string ip;
        int port;
    };

    //!
    //! \class Wrapper
    //! \brief Wrappers a message for network txs
    //!        It is recommended that packs only be used once
    //!
    class Wrapper
    {
    public:
        //!
        //! \brief Construct a pack
        //!
        Wrapper();

        //!
        //! \brief Destruct a pack
        //!
        ~Wrapper();

        //!
        //! \brief Wrap a message and get it ready for network transport
        //! \param sender Information about the sender
        //! \param recvr  Information about the receiver
        //! \param method Information regarding how to send the information
        //! \param data   Data to send over the network
        //! \param[out] reusult The resulting wrapped message
        //! \retval[true] The message was wrapped
        //! \retval[false] The message was not wrapped
        //!
        bool wrapMessage(TxInfo sender, TxInfo recvr, TransportMethod method, nlohmann::json data, nlohmann::json &result);
    };
}
#endif
