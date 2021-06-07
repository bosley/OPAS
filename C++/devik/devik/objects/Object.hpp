//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_OBJECT_HPP
#define DEVIK_OBJECT_HPP

#include "Fwd.hpp"
#include "Types.hpp"

namespace VM
{
    //!
    //! \brief The object parent class for all VM objects
    //!
    class Object
    {
    public:
        //!
        //! \brief Construct an object
        //! \param type The data _type that the object will represent
        //!
        Object(ObjectType type) : _data_type(type) { }

        //!
        //! \brief Default constructor
        //!
        ~Object() = default;

        //!
        //! \brief Visit a handler object
        //! \param handler The object to visit
        //!
        virtual void visit(ObjectHandler& handler) = 0;

        //!
        //! \brief Retrieve the object _type without having to be visited
        //! \return Type definition of the underlying object
        //!
        ObjectType get_type() { return _data_type; }

    private:
        ObjectType _data_type;   // The data _type
    };

}

#endif //DEVIK_OBJECT_HPP
