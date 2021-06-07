#ifndef KEYVIS_ENCODEABLE_IF_HPP
#define KEYVIS_ENCODEABLE_IF_HPP

#include <vector>
#include <stdint.h>

namespace KEYVIS
{

//!
//! \brief An interface that specifies the behavior of something that can be encoded
//!
class EncodeableIf
{
public:
    //!
    //! \brief Encode something to a byte vector
    //! \param[out] okay Should return true if encode was a success, false otherwise
    //! \returns vector of bytes representing the underlying data
    //!
    virtual std::vector<uint8_t> encode(bool & okay);

    //!
    //! \brief Decode a byte vector into the object that inherits this interface
    //! \param[in] data Reference to data that will be used to build object
    //! \retval true The decode process was a success
    //! \retval false The decode process failed
    //!
    virtual bool decode(std::vector<uint8_t> & data);
};
    
}
#endif