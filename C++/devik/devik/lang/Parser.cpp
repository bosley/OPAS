//
// Created by Josh Bosley on 6/3/21.
//

#include "lang/Parser.hpp"


namespace LANG
{

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::unordered_map<std::string, Precedence> precedences(
    {
            {TOKEN::EQ,       Precedence::EQUALS  },
            {TOKEN::NE,       Precedence::EQUALS  },
            {TOKEN::LT,       Precedence::ALLIGATOR},
            {TOKEN::GT,       Precedence::ALLIGATOR},
            {TOKEN::LE,       Precedence::ALLIGATOR},
            {TOKEN::GE,       Precedence::ALLIGATOR},
            {TOKEN::PLUS,     Precedence::SUM     },
            {TOKEN::MINUS,    Precedence::SUM     },
            {TOKEN::SLASH,    Precedence::PRODUCT },
            {TOKEN::ASTERISK, Precedence::PRODUCT },
            {TOKEN::PERCENT,  Precedence::PRODUCT },
            {TOKEN::LPAREN,   Precedence::CALL    },
            {TOKEN::LBRACKET, Precedence::INDEX   },
    });

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    Parser::Parser(Lexer& lexer) : _lexer(lexer)
    {
        _prefix_parse_fns[TOKEN::IDENT]  = &Parser::parse_identifier;
        _prefix_parse_fns[TOKEN::INT]    = &Parser::parse_integer_literal;
        _prefix_parse_fns[TOKEN::STRING] = &Parser::parse_string_literal;
        _prefix_parse_fns[TOKEN::BANG]   = &Parser::parse_prefix_expression;
        _prefix_parse_fns[TOKEN::MINUS]  = &Parser::parse_prefix_expression;
        _prefix_parse_fns[TOKEN::TRUE]   = &Parser::parse_boolean;
        _prefix_parse_fns[TOKEN::FALSE]  = &Parser::parse_boolean;
        _prefix_parse_fns[TOKEN::LPAREN]   = &Parser::parse_grouped_expression;
        _prefix_parse_fns[TOKEN::IF]       = &Parser::parse_if_expression;
        _prefix_parse_fns[TOKEN::WHILE]    = &Parser::parse_while_expression;
        _prefix_parse_fns[TOKEN::FUNCTION] = &Parser::parse_function_literal;
        _prefix_parse_fns[TOKEN::LBRACKET] = &Parser::parse_array_literal;
        _infix_parse_fns[TOKEN::PLUS]     = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::MINUS]    = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::SLASH]    = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::ASTERISK] = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::PERCENT]  = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::EQ]       = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::NE]       = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::LT]       = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::GT]       = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::LE]       = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::GE]       = &Parser::parse_infix_expression;
        _infix_parse_fns[TOKEN::LPAREN]   = &Parser::parse_call_expression;
        _infix_parse_fns[TOKEN::LBRACKET] = &Parser::parse_index_expression;

        next_token();
        next_token();
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::vector<std::string> Parser::get_errors()
    {
        return _errors;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    void Parser::next_token()
    {
        _curr_token = _peek_token;
        _peek_token = _lexer.next_token();
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    bool Parser::expect_peek(const std::string& t)
    {

        if(_peek_token._type == t)
        {
            next_token();
            return true;
        }
        else
        {
            peek_error(t);
            return false;
        }
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    Precedence Parser::peek_precedence()
    {
        if(precedences.find(_peek_token._type) != precedences.end())
        {
            return precedences[_peek_token._type];
        }
        return Precedence::LOWEST;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    Precedence Parser::curr_precedence()
    {
        if(precedences.find(_curr_token._type) != precedences.end())
        {
            return precedences[_curr_token._type];
        }
        return Precedence::LOWEST;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    void Parser::peek_error(const std::string& t)
    {
        _errors.push_back("expected :" + t + ", got " + _peek_token._type + " instead ");
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_statement()
    {
        if (_curr_token._type == TOKEN::LET)
        {
            return parse_let_statement();
        }
        else if (_curr_token._type == TOKEN::REF)
        {
            return parse_ref_statement();
        }
        else if (_curr_token._type == TOKEN::RETURN)
        {
            return parse_return_statement();
        }
        else
        {
            return parse_expression_statement();
        }
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::LetStatement* Parser::parse_let_statement()
    {
        AST::LetStatement* stmt = new AST::LetStatement();
        stmt->_token = _curr_token;

        if(!expect_peek(TOKEN::IDENT))
        {
            delete stmt;
            return nullptr;
        }

        stmt->_name._token = _curr_token;
        stmt->_name._value = _curr_token._literal;

        if(!expect_peek(TOKEN::ASSIGN))
        {
            delete stmt;
            return nullptr;
        }
        next_token();

        stmt->_value_expr = parse_expression(Precedence::LOWEST);

        if (_peek_token._type == TOKEN::SEMICOLON)
        {
            next_token();
        }
        return stmt;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::RefStatement* Parser::parse_ref_statement()
    {
        AST::RefStatement* stmt = new AST::RefStatement();
        stmt->_token = _curr_token;

        if(!expect_peek(TOKEN::IDENT))
        {
            delete stmt;
            return nullptr;
        }

        stmt->_name._token = _curr_token;
        stmt->_name._value = _curr_token._literal;

        if(!expect_peek(TOKEN::ASSIGN))
        {
            delete stmt;
            return nullptr;
        }

        next_token();

        stmt->_value_expr = parse_expression(Precedence::LOWEST);

        if (_peek_token._type == TOKEN::SEMICOLON)
        {
            next_token();
        }

        return stmt;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::ReturnStatement* Parser::parse_return_statement()
    {
        AST::ReturnStatement* stmt = new AST::ReturnStatement();
        stmt->_token = _curr_token;

        next_token();

        stmt->_return_expr = parse_expression(Precedence::LOWEST);

        if (_peek_token._type == TOKEN::SEMICOLON)
        {
            next_token();
        }

        return stmt;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::ExpressionStatement* Parser::parse_expression_statement()
    {
        AST::ExpressionStatement* stmt = new AST::ExpressionStatement();
        stmt->_token = _curr_token;
        stmt->_expression = parse_expression(Precedence::LOWEST);

        if (_peek_token._type == TOKEN::SEMICOLON)
        {
            next_token();
        }

        return stmt;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::BlockStatement* Parser::parse_block_statement()
    {
        AST::BlockStatement* block = new AST::BlockStatement();
        block->_token = _curr_token;

        next_token();

        while(_curr_token._type != TOKEN::RBRACE && _curr_token._type != TOKEN::END)
        {
            AST::Node* stmt = parse_statement();
            if (stmt != nullptr)
            {
                block->_statements.push_back(stmt);
            }
            next_token();
        }

        return block;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_expression(Precedence precedence)
    {
        if (_prefix_parse_fns.find(_curr_token._type) == _prefix_parse_fns.end())
        {
            no_prefix_parse_fn_error(_curr_token._type);
            return nullptr;
        }
        ParsePrefix prefix = _prefix_parse_fns[_curr_token._type];

        AST::Node* leftExp = (this->*prefix)();

        while(_peek_token._type != TOKEN::SEMICOLON && precedence < peek_precedence())
        {
            if (_infix_parse_fns.find(_peek_token._type) == _infix_parse_fns.end())
            {
                return leftExp;
            }

            ParseInfix infix = _infix_parse_fns[_peek_token._type];

            next_token();

            leftExp = (this->*infix)(leftExp);
        }

        return leftExp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::vector<AST::Node*> Parser::parse_expression_list()
    {
        std::vector<AST::Node*> arguments;

        next_token();
        arguments.push_back(parse_expression(Precedence::LOWEST));

        while(_peek_token._type == TOKEN::COMMA)
        {
            next_token();
            next_token();
            arguments.push_back(parse_expression(Precedence::LOWEST));
        }
        return arguments;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_identifier()
    {
        AST::Identifier* ident = new AST::Identifier();
        ident->_token = _curr_token;
        ident->_value = _curr_token._literal;
        return ident;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_boolean()
    {
        AST::BooleanLiteral* ident = new AST::BooleanLiteral();
        ident->_token = _curr_token;
        ident->_value = _curr_token._type == TOKEN::TRUE;
        return ident;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_integer_literal()
    {
        AST::IntegerLiteral* num = new AST::IntegerLiteral();
        num->_token = _curr_token;

        try
        {
            num->_value = std::stoi(_curr_token._literal);
        }
        catch (const std::invalid_argument e)
        {
            _errors.push_back("Cannot parse \"" + _curr_token._literal + "\" to _type \"Integer\"");
            return nullptr;

        }
        catch (const std::out_of_range e)
        {
            _errors.push_back("The _value \"" + _curr_token._literal + "\" is out of range for _type \"Integer\"");
            return nullptr;
        }

        return num;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_string_literal()
    {
        AST::StringLiteral* s = new AST::StringLiteral();
        s->_token = _curr_token;
        s->_value = _curr_token._literal;
        return s;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    std::vector<AST::Identifier*> Parser::parse_function_parameters()
    {
        std::vector<AST::Identifier*> parameters;

        if(_peek_token._type == TOKEN::RPAREN)
        {
            next_token();
            return parameters;
        }

        // Go past the next _token (,)
        next_token();

        AST::Identifier* ident = new AST::Identifier();

        ident->_token = _curr_token;
        ident->_value = _curr_token._literal;

        parameters.push_back(ident);

        while(_peek_token._type == TOKEN::COMMA)
        {
            next_token();
            next_token();

            AST::Identifier* ident = new AST::Identifier();

            ident->_token = _curr_token;
            ident->_value = _curr_token._literal;

            parameters.push_back(ident);
        }

        if(!expect_peek(TOKEN::RPAREN))
        {
            _errors.push_back("Missing expected _token \")\"");
            return std::vector<AST::Identifier*>();
        }

        return parameters;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_function_literal()
    {
        AST::FunctionLiteral* fn = new AST::FunctionLiteral();
        fn->_token = _curr_token;

        if(!expect_peek(TOKEN::LPAREN))
        {
            delete fn;
            return nullptr;
        }

        fn->_parameters = parse_function_parameters();

        if(!expect_peek(TOKEN::LBRACE))
        {
            delete fn;
            return nullptr;
        }

        fn->_body = parse_block_statement();
        return fn;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_array_literal()
    {
        AST::ArrayLiteral* exp = new AST::ArrayLiteral();
        exp->_token = _curr_token;

        if(_peek_token._type == TOKEN::RBRACKET)
        {
            next_token();
            return exp;
        }

        exp->_elements = parse_expression_list();

        if(!expect_peek(TOKEN::RBRACKET))
        {
            exp->_elements = std::vector<AST::Node*>();
        }
        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_prefix_expression()
    {
        AST::PrefixExpression* exp = new AST::PrefixExpression();
        exp->_token = _curr_token;
        exp->_op = _curr_token._literal;

        next_token();

        exp->_right = parse_expression(Precedence::PREFIX);
        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_grouped_expression()
    {
        next_token();

        AST::Node* exp = parse_expression(Precedence::LOWEST);

        if(!expect_peek(TOKEN::RPAREN))
        {
            delete exp;
            return nullptr;
        }

        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_if_expression()
    {
        AST::IfExpression* exp = new AST::IfExpression();
        exp->_token = _curr_token;

        // _condition
        if(!expect_peek(TOKEN::LPAREN))
        {
            delete exp;
            return nullptr;
        }

        next_token();

        exp->_condition = parse_expression(Precedence::LOWEST);

        if(!expect_peek(TOKEN::RPAREN))
        {
            delete exp;
            return nullptr;
        }

        if(!expect_peek(TOKEN::LBRACE))
        {
            delete exp;
            return nullptr;
        }

        exp->_consequence = parse_block_statement();

        if (_peek_token._type == TOKEN::ELSE)
        {
            next_token();

            if (!expect_peek(TOKEN::LBRACE))
            {
                delete exp;
                return nullptr;
            }

            exp->_alternative = parse_block_statement();
        }
        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_while_expression()
    {
        AST::WhileExpression* exp = new AST::WhileExpression();
        exp->_token = _curr_token;

        // _condition
        if(!expect_peek(TOKEN::LPAREN))
        {
            delete exp;
            return nullptr;
        }

        next_token();
        exp->_condition = parse_expression(Precedence::LOWEST);

        if(!expect_peek(TOKEN::RPAREN))
        {
            delete exp;
            return nullptr;
        }

        if(!expect_peek(TOKEN::LBRACE))
        {
            delete exp;
            return nullptr;
        }

        exp->_consequence = parse_block_statement();

        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_call_expression(AST::Node* function)
    {
        AST::CallExpression* exp = new AST::CallExpression();

        exp->_token = _curr_token;
        exp->_function = function;

        if(_peek_token._type == TOKEN::RPAREN)
        {
            next_token();
            return exp;
        }

        exp->_arguments = parse_expression_list();

        if(!expect_peek(TOKEN::RPAREN))
        {
            exp->_arguments = std::vector<AST::Node*>();
        }

        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_index_expression(AST::Node* array)
    {
        AST::IndexExpression* exp = new AST::IndexExpression();
        exp->_token = _curr_token;
        exp->_array = array;

        next_token();

        exp->_index = parse_expression(Precedence::LOWEST);

        if (!expect_peek(TOKEN::RBRACKET))
        {
            return nullptr;
        }

        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Node* Parser::parse_infix_expression(AST::Node* left)
    {
        AST::InfixExpression* exp = new AST::InfixExpression();

        exp->_token = _curr_token;
        exp->_op = _curr_token._literal;
        exp->_left = left;

        Precedence precedence = curr_precedence();

        next_token();
        exp->_right = parse_expression(precedence);

        return exp;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    void Parser::no_prefix_parse_fn_error(std::string type)
    {
        _errors.push_back("no prefix parse _function for " + type);
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    AST::Program* Parser::parse_program()
    {
        AST::Program *program = new AST::Program();

        while(_curr_token._type != TOKEN::END)
        {
            AST::Node* stmt = parse_statement();

            if(stmt != nullptr)
            {
                program->_statements.push_back(stmt);
            }
            next_token();
        }
        return program;
    }

}