//
// Created by Josh Bosley on 6/3/21.
//

#define DEVIK_NONE_HPP
#ifndef DEVIK_NONE_HPP

#include "objects/Fwd.hpp"
#include "objects/Object.hpp"
#include "objects/ObjectHandler.hpp"

namespace VM
{
    //!
    //! \brief A None object
    //!
    class None : public Object
    {
    public:
        None() : Object(ObjectType::NONE) {}

        virtual void visit(ObjectHandler& handler) override { handler.accept(this); }
    };
}

#endif //DEVIK_NONE_HPP
