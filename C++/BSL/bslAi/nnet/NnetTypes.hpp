#ifndef BSLAI_NNET_TYPES_HPP
#define BSLAI_NNET_TYPES_HPP

#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace NNET
{
    //!
    //! \brief Transfer functions available in the network
    //!
    enum class TransferFunction {
        TANH,
        SIGMOID
    };

    //!
    //! \brief Connection representation
    //!
    struct Connection {
        double weight;
        double deltaWeight;
            
        void operator= ( const Connection &rhs) {
            weight      = rhs.weight;
            deltaWeight = rhs.deltaWeight;
        }
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
    //! \brief Record connected cells in a network to reconnect from json
    //!
    struct CellToCell {

        unsigned idFrom;
        unsigned idTo;

        void operator= ( const CellToCell &rhs) {
            idFrom  = rhs.idFrom;
            idTo    = rhs.idTo;
        }

    };

    inline bool operator== ( const CellToCell &lhs,  const CellToCell &rhs ) {
        return(
            lhs.idFrom  == rhs.idFrom &&
            lhs.idTo == rhs.idTo
        );
    }

    inline void to_json(json& j, const CellToCell& c)
    {
        j = {{"idFrom", c.idFrom}, {"idTo", c.idTo}};
    }

    inline void from_json(const json& j, CellToCell& c) {
        j.at("idFrom").get_to(c.idFrom);
        j.at("idTo").get_to(c.idTo);
    }
}

#endif