//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_MEMORY_HPP
#define DEVIK_MEMORY_HPP

#include "objects/Fwd.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace VM
{
    //!
    //! \brief The VM memory object
    //!
    class Memory
    {
    public:
        //!
        //! \brief Construct a new memory object
        //!
        Memory();

        //!
        //! \brief Creates a new memory object context
        //!
        void new_context();

        //!
        //! \brief Leave the current memory object context
        //!
        void pop_context();

        //!
        //! \param name The _name of the object to set
        //! \param obj  The object to set
        //! \post This method std::moves the shared_ptr to obj into memory
        //!
        void set(std::string name, std::shared_ptr<Object> obj);

        //!
        //! \param name The _name of the object to get
        //! \returns unique pointer to a result object that may
        //!          or may not contain the requested object
        //!
        std::unique_ptr<Result> get(std::string name);

    private:
        uint64_t _max_obj_per;

        std::vector< std::unordered_map<std::string,
                     std::shared_ptr<Object> > > _memory_contexts;
    };
}

#endif //DEVIK_MEMORY_HPP
