//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_TOKEN_HPP
#define DEVIK_TOKEN_HPP

#include <string>
#include <unordered_map>

namespace LANG
{
    namespace TOKEN
    {
        //!
        //! \brief Devik Token
        //!
        class Token
        {
        public:
            //!
            //! \brief Construct a blank _token
            //!
            Token() = default;

            //!
            //! \brief Construct a _token given a _type and _literal (string)
            //! \param type The _token _type
            //! \param literal The _literal to assign
            //!
            Token(std::string type, std::string literal);

            //!
            //! \brief Construct a _token given a _type and _literal (char)
            //! \param type The _token _type
            //! \param literal The _literal to assign
            //!
            Token(std::string type, char literal);

            std::string _type;       //! The _token _type
            std::string _literal;    //! The _token _value
        };

        const std::string ILLEGAL = "ILLEGAL";
        const std::string END     = "END";

        const std::string IDENT   = "IDENT";
        const std::string INT     = "INT";
        const std::string STRING  = "STRING";

        const std::string ASSIGN   = "=";
        const std::string PLUS     = "+";
        const std::string MINUS    = "-";
        const std::string BANG     = "!";
        const std::string ASTERISK = "*";
        const std::string SLASH    = "/";
        const std::string PERCENT  = "%";

        const std::string LT     = "<";
        const std::string GT     = ">";
        const std::string LE     = "<=";
        const std::string GE     = ">=";

        const std::string EQ     = "==";
        const std::string NE     = "!=";

        const std::string COMMA     = ",";
        const std::string SEMICOLON = ";";

        const std::string LPAREN   = "(";
        const std::string RPAREN   = ")";
        const std::string LBRACE   = "{";
        const std::string RBRACE   = "}";
        const std::string LBRACKET = "[";
        const std::string RBRACKET = "]";

        const std::string REF    = "&";

        // keywords
        const std::string FUNCTION = "FUNCTION";
        const std::string LET      = "LET";
        const std::string TRUE     = "TRUE";
        const std::string FALSE    = "FALSE";
        const std::string IF       = "IF";
        const std::string ELSE     = "ELSE";
        const std::string RETURN   = "RETURN";
        const std::string WHILE    = "WHILE";
    }
}

#endif //DEVIK_TOKEN_HPP
