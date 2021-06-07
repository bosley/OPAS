//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_INTEGER_HPP
#define DEVIK_INTEGER_HPP

#include "objects/Types.hpp"
#include "objects/Object.hpp"
#include "objects/ObjectHandler.hpp"

namespace VM
{
    //!
    //! \brief An integer object
    //!
    class Integer : public Object
    {
    public:
        //!
        //! \brief A new integer
        //! \post  The integer will have the _value 0
        //!
        Integer() : Object(ObjectType::INTEGER), _value(0) {}

        //!
        //! \param value The integer _value to store
        //!
        Integer(long value) : Object(ObjectType::INTEGER), _value(value) {}

        //!
        //! \param value The integer _value to store
        //!
        Integer(int  value) : Object(ObjectType::INTEGER), _value(static_cast<long>(value)) {}

        //!
        //! \param value The integer _value to store
        //!
        Integer(signed short value) : Object(ObjectType::INTEGER), _value(static_cast<long>(value)) {}

        //!
        //! \returns _value of the integer
        //!
        long get_value(){ return _value; }

        virtual void visit(ObjectHandler& handler) override { handler.accept(this); }

    private:
        long _value;
    };
}


#endif //DEVIK_INTEGER_HPP
