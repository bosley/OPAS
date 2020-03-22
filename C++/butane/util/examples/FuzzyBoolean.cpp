#include "UTILFuzzyBool.hpp"

#include <iostream>

int main()
{
    for(double i = 1; i <= 10; i++)
    {
        std::cout << "----------------[test:" << (int)i << "]----------------" << std::endl;

        double probability = (i/10);
        BUTANE::UTIL::FuzzyBool fuzzyBool(probability);

        std::cout << "fuzzyBool(probability=" << probability << ")   -> " << fuzzyBool.toBool() << std::endl;

        double probability0 = 1.0 - probability;
        BUTANE::UTIL::FuzzyBool fuzzyBool0(probability);
        std::cout << "fuzzyBool0(probability0=" << probability0 << ") -> " << fuzzyBool0.toBool() << std::endl;

        std::cout << "-----------------------------------------" << std::endl;

        std::cout << "fuzzyBool&&fuzzyBool0 = " << (fuzzyBool&&fuzzyBool0).get() << std::endl;
        std::cout << "fuzzyBool||fuzzyBool0 = " << (fuzzyBool||fuzzyBool0).get() << std::endl;
        std::cout << "fuzzyBool&&true = "       << (fuzzyBool&&true).get()       << std::endl;
        std::cout << "fuzzyBool&&false = "      << (fuzzyBool&&false).get()      << std::endl;
        std::cout << "fuzzyBool||true = "       << (fuzzyBool||true).get()       << std::endl;
        std::cout << "fuzzyBool||false = "      << (fuzzyBool||false).get()      << std::endl;
        std::cout << "!fuzzyBool = "            << (!fuzzyBool).get()            << std::endl;
        std::cout << "!fuzzyBool0 = "           << (!fuzzyBool0).get()           << std::endl;
    }


    return 0;
}