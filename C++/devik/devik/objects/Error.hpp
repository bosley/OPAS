//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_ERROR_HPP
#define DEVIK_ERROR_HPP

#include "objects/Fwd.hpp"
#include "objects/Object.hpp"
#include "objects/ObjectHandler.hpp"

#include <string>

namespace VM
{
    //!
    //! \brief An error object
    //!
    class Error : public Object
    {
    public:
        //!
        //! \brief Simple readability enumeration
        //!
        enum Level
        {
            FATAL,
            NON_FATAL
        };

        //!
        //! \brief A new error
        //! \param isFatal If true, the error is
        //!
        Error(Level isFatal, std::string msg) : Object(ObjectType::ERROR), _is_fatal(isFatal), _message(msg) {}

        //!
        //! \retval true iff the error should be considered fatal
        //!
        const bool get_is_fatal() { return Level::FATAL == _is_fatal; }

        //!
        //! \returns error message
        //!
        const std::string get_message() { return _message; }

        virtual void visit(ObjectHandler& handler) override { handler.accept(this); }

    protected:
        Level _is_fatal;
        std::string _message;
    };
}

#endif //DEVIK_ERROR_HPP
