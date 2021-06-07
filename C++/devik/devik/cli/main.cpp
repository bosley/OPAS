//
// Created by Josh Bosley on 6/3/21.
//

#include "devik/Devik.hpp"

#include <iostream>

int main(int argc, char **argv)
{
    DEVIK::Devik devik;

    switch(devik.launch("some_file.dev"))
    {
        case DEVIK::Devik::Result::OKAY:
        {
            std::cout << "Result : Complete" << std::endl;
            return 0;
        }
        case DEVIK::Devik::Result::FAILED_TO_LOAD_SOURCE:
        {
            std::cout << "Result : Failed to load source file" << std::endl;
            return 1;
        }
        case DEVIK::Devik::Result::EXECUTION_FAILURE:
        {
            std::cout << "Result : Execution Failure" << std::endl;
            return 1;
        }
        default:
        {
            std::cout << "Unknown code returned from Devik" << std::endl;
            return 1;
        }
    }
}
