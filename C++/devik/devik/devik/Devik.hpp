//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_DEVIK_HPP
#define DEVIK_DEVIK_HPP

#include "lang/SourceTree.hpp"

#include <string>

namespace DEVIK
{
    //!
    //! \brief The Devik interface
    //!
    class Devik
    {
    public:
        //!
        //! \brief Potential result codes
        //!
        enum class Result
        {
            OKAY,
            FAILED_TO_LOAD_SOURCE,
            EXECUTION_FAILURE
        };

        //!
        //! \param file_name SourceTree file containing code to launch from
        //! \returns corresponding result code from attempt to launch
        //!
        Result launch(const std::string& file_name);

    private:
        LANG::SourceTree source_code;
    };
}


#endif //DEVIK_MACHINE_HPP
