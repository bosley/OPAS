//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_TYPES_HPP
#define DEVIK_TYPES_HPP

namespace VM
{
    //!
    //! \brief All of the different object types enumerated
    //!
    enum class ObjectType
    {
        NONE    = 0x00,
        INTEGER = 0x01,
        DOUBLE  = 0x03,
        STRING  = 0x04,
        RESULT  = 0xAA,
        ERROR   = 0xFF,
    };
}

#endif //DEVIK_TYPES_HPP
