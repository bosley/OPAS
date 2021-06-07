//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_STRING_HPP
#define DEVIK_STRING_HPP

#include "objects/Fwd.hpp"
#include "objects/Object.hpp"
#include "objects/ObjectHandler.hpp"

#include <string>

namespace VM
{
    //!
    //! \brief A string object
    //!
    class String : public Object
    {
    public:
        //!
        //! \brief A new string
        //! \post  The string will be empty
        //!
        String() : Object(ObjectType::STRING), _value("") {}

        //!
        //! \param value The string _value to store
        //!
        String(std::string value) : Object(ObjectType::STRING), _value(value) {}

        //!
        //! \returns _value of the string
        //!
        std::string get_value(){ return _value; }

        virtual void visit(ObjectHandler& handler) override { handler.accept(this); }

    private:
        std::string _value;
    };
}

#endif //DEVIK_STRING_HPP
