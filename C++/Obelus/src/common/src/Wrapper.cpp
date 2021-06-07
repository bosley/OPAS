#include "Wrapper.hpp"

namespace OBELUS
{
    //---------------------------------------------------------
    // Wrapper
    //---------------------------------------------------------
    Wrapper::Wrapper()
    {
    }

    //---------------------------------------------------------
    // ~Wrapper
    //---------------------------------------------------------
    Wrapper::~Wrapper()
    {}

    //---------------------------------------------------------
    // wrapMessage
    //---------------------------------------------------------
    bool Wrapper::wrapMessage(TxInfo sender, TxInfo recvr, TransportMethod method,  nlohmann::json data, nlohmann::json &result)
    {
        std::string stringMethod = (method == TransportMethod::TCP) ? "TCP" : "UDP";

        result = 
        {
            {"transport",  stringMethod},
            {"senderIp",   sender.ip},
            {"senderPort", sender.port},
            {"recvrIp",    recvr.ip},
            {"recvrPort",  recvr.port},
            {"data",       data}
        };

        return true;
    }

}