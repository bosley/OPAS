//
// Created by Josh Bosley on 6/4/21.
//

#include "lang/Token.hpp"
#include "lang/Keywords.hpp"

namespace LANG
{
    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    Keywords::Keywords()
    {
        _keywords["let"]    = TOKEN::LET;
        _keywords["fn"]     = TOKEN::FUNCTION;
        _keywords["true"]   = TOKEN::TRUE;
        _keywords["false"]  = TOKEN::FALSE;
        _keywords["if"]     = TOKEN::IF;
        _keywords["else"]   = TOKEN::ELSE;
        _keywords["return"] = TOKEN::RETURN;
        _keywords["while"]  = TOKEN::WHILE;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::string Keywords::lookup_ident(const std::string& ident)
    {
        if(_keywords.find(ident) == _keywords.end())
        {
            return TOKEN::IDENT;
        }

        return _keywords[ident];
    }
}