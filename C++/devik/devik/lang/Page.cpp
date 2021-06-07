//
// Created by Josh Bosley on 6/3/21.
//

#include "lang/Page.hpp"

#include <filesystem>
#include <fstream>

namespace LANG
{
    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    bool Page::load(const std::string& file)
    {
        if( !std::filesystem::exists(file) )
        {
            return false;
        }

        // Load the page
        std::ifstream in(file);
        _page = std::string((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());

        return true;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    size_t Page::get_len()
    {
        return _page.length();
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    bool Page::is_valid(size_t pos)
    {
        return pos < _page.length();
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    char Page::char_at(size_t pos)
    {
        if( !is_valid(pos) )
        {
            return 0;
        }

        return _page[pos];
    }

}