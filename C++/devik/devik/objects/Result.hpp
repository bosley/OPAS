//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_RESULT_HPP
#define DEVIK_RESULT_HPP

#include "objects/Fwd.hpp"
#include "objects/Object.hpp"
#include "objects/ObjectHandler.hpp"

#include <memory>

namespace VM
{
    //!
    //! \brief A Result object
    //!
    class Result : public Object
    {
    public:
        //!
        //! \brief A result object
        //! \param good The _type that will appear if the result is 'good'
        //! \param bad  The _type that will appear if the result is 'bad'
        //! \post  Result object is ready to be finalized (data needs to be set)
        //!        data is set to not okay util set_okay() is called
        //!
        Result(ObjectType okay, ObjectType error) : Object(ObjectType::RESULT),
                                                    _okay(okay),
                                                    _error(error),
                                                    _is_okay(false){}

        //!
        //! \returns The _type that the result will return if okay
        //!
        ObjectType get_type_okay()  { return _okay;  }

        //!
        //! \returns The _type that the result will return if in error
        //!
        ObjectType get_type_error() { return _error; }

        //!
        //! \brief Set the result to 'okay'
        //!
        void set_okay() { _is_okay = true; }

        //!
        //! \brief Set the result to 'error'
        //!
        void set_error() { _is_okay = false; }

        //!
        //! \retval true iff the result was marked as okay
        //!
        bool is_okay() { return _is_okay; }

        //!
        //! \brief Set the result data
        //! \note  This method creates a copy of the shared data
        //!
        void set_data(std::shared_ptr<Object> obj) { _data = obj; }

        //!
        //! \returns A weak pointer to the resulting data
        //!
        std::weak_ptr<Object> get_inner() { return _data; }

        virtual void visit(ObjectHandler& handler) override { handler.accept(this); }
    private:
        ObjectType _okay;
        ObjectType _error;
        bool _is_okay;
        std::shared_ptr<Object> _data;
    };
}

#endif //DEVIK_RESULT_HPP
