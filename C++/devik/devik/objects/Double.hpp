//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_DOUBLE_HPP
#define DEVIK_DOUBLE_HPP

#include "objects/Fwd.hpp"
#include "objects/Object.hpp"
#include "objects/ObjectHandler.hpp"

namespace VM
{
    //!
    //! \brief A double object
    //!
    class Double : public Object
    {
    public:
        //!
        //! \brief A new double
        //! \post  The double will have the _value 0.00
        //!
        Double() : Object(ObjectType::DOUBLE), _value(0.00) {}

        //!
        //! \param value The double _value to store
        //!
        Double(double value) : Object(ObjectType::DOUBLE), _value(value) {}

        //!
        //! \param value The double _value to store
        //!
        Double(float value) : Object(ObjectType::DOUBLE), _value(static_cast<double>(value)) {}

        //!
        //! \returns _value of the double
        //!
        double get_value(){ return _value; }

        virtual void visit(ObjectHandler& handler) override { handler.accept(this); }

    private:
        double _value;
    };
}

#endif //DEVIK_DOUBLE_HPP
