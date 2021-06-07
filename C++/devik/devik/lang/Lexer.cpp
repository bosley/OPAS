#include "Lexer.hpp"

#include <ctype.h>

namespace LANG
{
    namespace
    {
        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        bool isDigit(char ch)
        {
            return isdigit(ch);
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        bool is_letter(char ch)
        {
            return isalpha(ch) || ch == '_';
        }
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    Lexer::Lexer(Page& page) : _page(page), _line_no(1), _read_position(0)
    {

    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    TOKEN::Token Lexer::next_token()
    {
        TOKEN::Token tok;
        skip_whitespace();
        switch (_ch)
        {
            case '=':
            {
                if(peek_char() == '=')
                {
                    tok = TOKEN::Token(TOKEN::EQ, "==");
                    read_char();
                } else
                {
                    tok = TOKEN::Token(TOKEN::ASSIGN, _ch);
                }
                read_char();
                break;
            }

            case '!':
            {
                if(peek_char() == '=')
                {
                    tok = TOKEN::Token(TOKEN::NE, "!=");
                    read_char();
                }
                else
                {
                    tok = TOKEN::Token(TOKEN::BANG, _ch);
                }
                read_char();
                break;
            }

            case '<':
            {
                if(peek_char() == '=')
                {
                    tok = TOKEN::Token(TOKEN::LE, "<=");
                    read_char();
                }
                else
                {
                    tok = TOKEN::Token(TOKEN::LT, _ch);
                }
                read_char();
                break;
            }

            case '>':
            {
                if(peek_char() == '=')
                {
                    tok = TOKEN::Token(TOKEN::GE, ">=");
                    read_char();
                }
                else
                {
                    tok = TOKEN::Token(TOKEN::GT, _ch);
                }
                read_char();
                break;
            }

            case '+':
            {
                tok = TOKEN::Token(TOKEN::PLUS, _ch);
                read_char();
                break;
            }

            case '-':
            {
                tok = TOKEN::Token(TOKEN::MINUS, _ch);
                read_char();
                break;
            }

            case '*':
            {
                tok = TOKEN::Token(TOKEN::ASTERISK, _ch);
                read_char();
                break;
            }

            case '/':
            {
                tok = TOKEN::Token(TOKEN::SLASH, _ch);
                read_char();
                break;
            }

            case '%':
            {
                tok = TOKEN::Token(TOKEN::PERCENT, _ch);
                read_char();
                break;
            }

            case ',':
            {
                tok = TOKEN::Token(TOKEN::COMMA, _ch);
                read_char();
                break;
            }

            case ';':
            {
                tok = TOKEN::Token(TOKEN::SEMICOLON, _ch);
                read_char();
                break;
            }

            case '(':
            {
                tok = TOKEN::Token(TOKEN::LPAREN, _ch);
                read_char();
                break;
            }

            case ')':
            {
                tok = TOKEN::Token(TOKEN::RPAREN, _ch);
                read_char();
                break;
            }

            case '{':
            {
                tok = TOKEN::Token(TOKEN::LBRACE, _ch);
                read_char();
                break;
            }

            case '}':
            {
                tok = TOKEN::Token(TOKEN::RBRACE, _ch);
                read_char();
                break;
            }

            case '[':
            {
                tok = TOKEN::Token(TOKEN::LBRACKET, _ch);
                read_char();
                break;
            }

            case ']':
            {
                tok = TOKEN::Token(TOKEN::RBRACKET, _ch);
                read_char();
                break;
            }

            case '"':
            {
                tok = TOKEN::Token(TOKEN::STRING, read_string());
                read_char();
                break;
            }

            case '&':
            {
                tok = TOKEN::Token(TOKEN::REF, _ch);
                read_char();
                break;
            }

            case 0:
            {
                tok = TOKEN::Token(TOKEN::END, "");
                read_char();
                break;
            }

            default:
            {
                if(is_letter(_ch))
                {
                    tok._literal = read_identifier();
                    tok._type = _keywords.lookup_ident(tok._literal);
                }
                else if(isDigit(_ch))
                {
                    tok._type = TOKEN::INT;
                    tok._literal = read_number();
                }
                else
                {
                    tok = TOKEN::Token(TOKEN::ILLEGAL, _ch);
                    read_char();
                }
                break;
            }
        }
        return tok;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    void Lexer::read_char()
    {
        if(!_page.is_valid(_read_position))
        {
            _ch = 0;
        }
        else
        {
            _ch = _page.char_at(_read_position);
        }
        _read_position++;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    char Lexer::peek_char()
    {
        if(!_page.is_valid(_read_position))
        {
            return 0;
        }
        else
        {
            return _page.char_at(_read_position);
        }
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::string Lexer::read_identifier()
    {
        std::string id = "";
        while(is_letter(_ch))
        {
            id += _ch;
            read_char();
        }
        return id;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::string Lexer::read_number()
    {
        std::string num = "";
        while(isDigit(_ch))
        {
            num += _ch;
            read_char();
        }
        return num;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::string Lexer::read_string()
    {

        read_char();

        std::string str = "";

        while(_ch != '"' && _ch != 0)
        {
            str += _ch;
            read_char();
        }

        return str;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    void Lexer::skip_whitespace()
    {
        while(_ch == ' ' || _ch == '\t' || _ch == '\r' || _ch == '\n')
        {
            if( _ch == '\n')
            {
                _line_no += 1;
            }
            read_char();
        }
        if(_ch == '/' && peek_char() == '/')
        {
            while(_ch != '\n')
            {
                read_char();
            }
            _line_no += 1;
            skip_whitespace();
        }
    }

}