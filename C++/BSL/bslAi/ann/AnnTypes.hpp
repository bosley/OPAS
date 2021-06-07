#ifndef BSLAI_ANN_TYPES_HPP
#define BSLAI_ANN_TYPES_HPP

#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace ANN
{
    //!
    //! \brief Connection representation
    //!
    struct Connection {
        double weight;
        double deltaWeight;
    };

    inline void to_json(json& j, const Connection& c)
    {
        j = {{"weight", c.weight}, {"deltaWeight", c.deltaWeight}};
    }

    inline void from_json(const json& j, Connection& c) {
        j.at("weight").get_to(c.weight);
        j.at("deltaWeight").get_to(c.deltaWeight);
    }

    inline bool operator== (const Connection &lhs, const Connection &rhs) {
        return (lhs.weight      == rhs.weight && 
                lhs.deltaWeight == rhs.deltaWeight);
    }

    //!
    //! \brief Transfer functions available in the network
    //!
    enum class TransferFunction {
        TANH,
        SIGMOID
    };
}

#endif