#ifndef DEVIK_LEXER_HPP
#define DEVIK_LEXER_HPP

#include "lang/Keywords.hpp"
#include "lang/Page.hpp"
#include "lang/Token.hpp"

#include <string>

namespace LANG
{
    //!
    //! \brief Lexer
    //!
    class Lexer
    {
    public:
        //!
        //! \param page The source input for the _lexer
        //!
        Lexer(Page& page);

        //!
        //! \brief Receive the next available _token
        //! \returns Next available _token
        //!
        TOKEN::Token next_token();

    private:
        void read_char();
        char peek_char();
        std::string read_identifier();
        std::string read_number();
        std::string read_string();
        void skip_whitespace();

        Page&    _page;
        uint32_t _line_no;
        size_t   _read_position;
        char     _ch;
        Keywords _keywords;
    };
}

#endif