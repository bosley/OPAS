//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_OBJECTHANDLER_HPP
#define DEVIK_OBJECTHANDLER_HPP

#include "objects/Fwd.hpp"

namespace VM
{
    //!
    //! \brief Object handler that objects can visit
    //!
    class ObjectHandler
    {
    public:
        ~ObjectHandler() = default;

        //!
        //! \brief Accept an integer _type
        //!
        virtual void accept(Integer *) = 0;

        //!
        //! \brief Accept a double _type
        //!
        virtual void accept(Double  *) = 0;

        //!
        //! \brief Accept a none _type
        //!
        virtual void accept(None *) = 0;

        //!
        //! \brief Accept an error _type
        //!
        virtual void accept(Error  *) = 0;

        //!
        //! \brief Accept a result _type
        //!
        virtual void accept(Result  *) = 0;

        //!
        //! \brief Accept a string _type
        //!
        virtual void accept(String  *) = 0;
    };
}

#endif //DEVIK_OBJECTHANDLER_HPP
