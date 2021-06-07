//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_PARSER_HPP
#define DEVIK_PARSER_HPP

#include "Lexer.hpp"
#include "Ast.hpp"

#include <vector>
#include <string>
#include <unordered_map>

// TODO : The lexer now has access to line numbers (in theory) and can easily
//        get access to file name. We will need to make an actual error reporter
//        rather than just dump out a bunch of strings. This might be best done
//        later after the language starts to be fleshed out, and to have a dedicated
//        branch / issue for it

namespace LANG
{
    // FWD for the following declarations
    class Parser;

    // Aliases to simplify the definition and calls to methods
    typedef AST::Node* (Parser::*ParsePrefix)();
    typedef AST::Node* (Parser::*ParseInfix)(AST::Node* );

    //!
    //! \brief Operator precedence
    //!
    enum class Precedence
    {
        LOWEST,
        EQUALS,
        ALLIGATOR,
        SUM,
        PRODUCT,
        PREFIX,
        CALL,
        INDEX,
    };

    //!
    //! \brief Token precedences
    //!
    extern std::unordered_map<std::string, Precedence> precedences;

    //!
    //! \brief Program parser
    //!
    class Parser
    {
    public:
        //!
        //! \brief Set the _lexer that will be used
        //! \param[in] lexer The _lexer to draw tokens from
        //!
        explicit Parser(Lexer& lexer);

        //!
        //! \brief Retrieve any parser _errors that were generated
        //! \returns std::vector of _errors iff _errors occur
        //!
        std::vector<std::string> get_errors();

        //!
        //! \brief Parse the program from the input given to the _lexer
        //! \returns AST root of the given users program
        //! \note Ensure that no _errors are present using the _errors() call
        //!
        AST::Program* parse_program();

    private:
        void next_token();
        bool expect_peek(const std::string& t);
        Precedence peek_precedence();
        Precedence curr_precedence();

        AST::Node* parse_statement();
        AST::LetStatement* parse_let_statement();
        AST::RefStatement* parse_ref_statement();
        AST::ReturnStatement* parse_return_statement();
        AST::ExpressionStatement* parse_expression_statement();
        AST::BlockStatement* parse_block_statement();

        AST::Node* parse_expression(Precedence precedence);
        AST::Node* parse_identifier();
        AST::Node* parse_integer_literal();
        AST::Node* parse_string_literal();
        AST::Node* parse_prefix_expression();
        AST::Node* parse_boolean();
        std::vector<AST::Identifier*> parse_function_parameters();
        AST::Node* parse_function_literal();
        AST::Node* parse_array_literal();
        AST::Node* parse_grouped_expression();
        AST::Node* parse_if_expression();
        AST::Node* parse_while_expression();

        std::vector<AST::Node*> parse_expression_list();
        AST::Node* parse_call_expression(AST::Node* function);
        AST::Node* parse_index_expression(AST::Node* array);
        AST::Node* parse_infix_expression(AST::Node* left);

        void no_prefix_parse_fn_error(std::string type);

        void peek_error(const std::string& t);

        Lexer& _lexer;

        TOKEN::Token _curr_token;
        TOKEN::Token _peek_token;

        std::unordered_map<std::string, ParsePrefix> _prefix_parse_fns;
        std::unordered_map<std::string, ParseInfix>  _infix_parse_fns;

        std::vector<std::string> _errors;
    };

}


#endif //DEVIK_PARSER_HPP
