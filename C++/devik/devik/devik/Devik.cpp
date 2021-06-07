//
// Created by Josh Bosley on 6/3/21.
//

#include "Devik.hpp"

namespace DEVIK
{
    Devik::Result Devik::launch(const std::string& file_name)
    {
        // If the source manager fails to load the item, indicate that there was a load fail
        if( !source_code.load_source(file_name) )
        {
            return Result::FAILED_TO_LOAD_SOURCE;
        }

        // TODO : Create the AST evaluator and plop a definition of it here.
        //        Once its here we can call it like so:
        //          evaluator.eval(source_code.get_tree(file_name))

        // TODO : Increase the granularity of errors reported back from this thing

        return Result::OKAY;
    }
}