//
// Created by bosley on 3/27/20.
//

#ifndef BSL_APP_RANDOM_HPP
#define BSL_APP_RANDOM_HPP

#include <random>

namespace APP
{
    namespace HELPERS
    {
        // Generate a random string of a specified length
        std::string generate_random_string(std::string::size_type length)
        {
            static auto& chrs = "0123456789"
                                "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

            thread_local static std::mt19937 rg{std::random_device{}()};
            thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

            std::string s;

            s.reserve(length);

            while(length--)
                s += chrs[pick(rg)];

            return s;
        }
    }
}

#endif //BSL_APP_RANDOM_HPP
