//
// Created by Josh Bosley on 6/3/21.
//

#include "Token.hpp"

namespace LANG
{

    namespace TOKEN
    {
        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        Token::Token(std::string type, std::string literal) : _type(type), _literal(literal)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        Token::Token(std::string type, char ch) : _type(type), _literal(std::string(1, ch))
        {

        }
    }
}