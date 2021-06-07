//
// Created by Josh Bosley on 6/3/21.
//

#include "Memory.hpp"
#include "objects/Error.hpp"
#include "objects/Result.hpp"

namespace VM
{
    // -------------------------------------------------------------------
    //
    // -------------------------------------------------------------------

    Memory::Memory()
    {
        new_context();
    }

    // -------------------------------------------------------------------
    //
    // -------------------------------------------------------------------

    void Memory::new_context()
    {
        _memory_contexts.emplace_back(
                std::unordered_map<std::string, std::shared_ptr<Object> >()
                        );
    }

    // -------------------------------------------------------------------
    //
    // -------------------------------------------------------------------

    void Memory::pop_context()
    {
        _memory_contexts.pop_back();
    }

    // -------------------------------------------------------------------
    //
    // -------------------------------------------------------------------

    void Memory::set(std::string name, std::shared_ptr<Object> obj)
    {
        // TODO: We MAY need to std::move the thing to an overall container and put the shared
        //       reference into the context. That way we can have return values from contexts stored
        //       and delete things with a gc or something

        // Ensure that we have a context to work with
        if( _memory_contexts.empty() )
        {
            new_context();
        }

        _memory_contexts.back()[name] = std::move(obj);
    }

    // -------------------------------------------------------------------
    //
    // -------------------------------------------------------------------

    std::unique_ptr<Result> Memory::get(std::string name)
    {
        // Ensure that there is a context to work with
        if( _memory_contexts.empty() )
        {
            auto result = Result(ObjectType::NONE, ObjectType::ERROR);

            result.set_data(
                    std::make_shared<Error>(
                            Error(Error::FATAL, "Attempted to retrieve variable with no given context")
                            )
                    );

            result.set_error();

            return std::make_unique<Result>(result);
        }

        // Reverse iterate over contexts to find the variable they might be wanting
        // This allows us to do variable shadowing!
        for(auto ctx = _memory_contexts.rbegin(); ctx != _memory_contexts.rend(); ++ctx)
        {
            if( ctx->find(name) != ctx->end() )
            {
                auto result = Result( (*ctx)[name]->get_type(), ObjectType::ERROR);

                result.set_data(
                        (*ctx)[name]
                        );

                result.set_okay();

                return std::make_unique<Result>(result);
            }
        }

        auto result = Result(ObjectType::NONE, ObjectType::ERROR);

        std::string error_message = "Unknown identifier <" + name + ">";

        // Item wasn't found, all is lost
        result.set_data(
                std::make_shared<Error>(
                        Error(Error::FATAL, error_message)
                )
        );

        result.set_error();

        return std::make_unique<Result>(result);
    }
}