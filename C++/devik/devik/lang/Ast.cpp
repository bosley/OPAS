//
// Created by Josh Bosley on 6/3/21.
//

#include "Ast.hpp"

namespace LANG
{
    namespace AST
    {
        // #########################################################
        //                       PROGRAM
        // #########################################################

        Program::Program() : Node(NodeClassification::ROOT)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        Program::~Program()
        {
            for(auto stmt : _statements)
            {
                delete stmt;
            }
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string Program::token_literal() const
        {
            if(_statements.size())
            {
                return _statements[0]->token_literal();
            }
            else
            {
                return "";
            }
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string Program::as_string() const
        {
            std::string res;
            for(auto stmt : _statements)
            {
                res += stmt->as_string() + "\n";
            }
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType Program::get_type() const
        {
            return NodeType::PROGRAM;
        }

        // #########################################################
        //                       BLOCK
        // #########################################################

        BlockStatement::BlockStatement() : Node(NodeClassification::STATEMENT)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        BlockStatement::~BlockStatement()
        {
            for(auto stmt : _statements)
            {
                delete stmt;
            }
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string BlockStatement::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string BlockStatement::as_string() const
        {
            std::string res = "{";
            for(auto stmt : _statements)
            {
                res += stmt->as_string() + " ";
            }
            res += "}";
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType BlockStatement::get_type() const
        {
            return NodeType::BLOCK;
        }

        // #########################################################
        //                       IDENTIFIER
        // #########################################################

        Identifier::Identifier() : Node(NodeClassification::EXPRESSION)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string Identifier::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string Identifier::as_string() const
        {
            return _value;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType Identifier::get_type() const
        {
            return NodeType::ID;
        }

        // #########################################################
        //                       INT LITERAL
        // #########################################################

        IntegerLiteral::IntegerLiteral() : Node(NodeClassification::EXPRESSION)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string IntegerLiteral::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string IntegerLiteral::as_string() const
        {
            return std::to_string(_value);
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType IntegerLiteral::get_type() const
        {
            return NodeType::INTEGER;
        }

        // #########################################################
        //                       BOOL LITERAL
        // #########################################################

        BooleanLiteral::BooleanLiteral() : Node(NodeClassification::EXPRESSION)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string BooleanLiteral::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string BooleanLiteral::as_string() const
        {
            return _value ? "True" : "False";
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType BooleanLiteral::get_type() const
        {
            return NodeType::BOOLEAN;
        }

        // #########################################################
        //                       STRING LITERAL
        // #########################################################

        StringLiteral::StringLiteral() : Node(NodeClassification::EXPRESSION)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string StringLiteral::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string StringLiteral::as_string() const
        {
            return _value;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType StringLiteral::get_type() const
        {
            return NodeType::STRING;
        }

        // #########################################################
        //                       FUNCTION
        // #########################################################

        FunctionLiteral::FunctionLiteral() : Node(NodeClassification::EXPRESSION), _body(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        FunctionLiteral::~FunctionLiteral()
        {
            for(auto p : _parameters)
            {
                delete p;
            }
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string FunctionLiteral::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string FunctionLiteral::as_string() const
        {
            std::string res = token_literal() + " (";
            for(auto param : _parameters)
            {
                res += param->as_string() + ", ";
            }

            res += ")";
            res += _body->as_string();
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType FunctionLiteral::get_type() const
        {
            return NodeType::FN;
        }

        // #########################################################
        //                       ARRAY
        // #########################################################

        ArrayLiteral::ArrayLiteral() : Node(NodeClassification::EXPRESSION)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        ArrayLiteral::~ArrayLiteral()
        {
            for(auto e : _elements)
            {
                delete e;
            }
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string ArrayLiteral::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string ArrayLiteral::as_string() const
        {
            std::string res = "[";

            for(auto e : _elements)
            {
                res += e->as_string() + ", ";
            }

            res += "]";
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType ArrayLiteral::get_type() const
        {
            return NodeType::ARRAY;
        }

        // #########################################################
        //                       CALL
        // #########################################################

        CallExpression::CallExpression() : Node(NodeClassification::EXPRESSION), _function(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        CallExpression::~CallExpression()
        {
            for(auto a : _arguments)
            {
                delete a;
            }

            delete _function;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string CallExpression::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string CallExpression::as_string() const
        {
            std::string res = _function->as_string() + "(";

            for(auto arg : _arguments)
            {
                res += arg->as_string() + ", ";
            }

            res += ")";
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType CallExpression::get_type() const
        {
            return NodeType::CALL;
        }

        // #########################################################
        //                       INDEX EXPR
        // #########################################################

        IndexExpression::IndexExpression() : Node(NodeClassification::EXPRESSION), _array(nullptr), _index(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string IndexExpression::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string IndexExpression::as_string() const
        {
            return "(" + _array->as_string() + "[" + _index->as_string() + "])";
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType IndexExpression::get_type() const
        {
            return NodeType::INDEX_EXPR;
        }

        // #########################################################
        //                       PREFIX EXPR
        // #########################################################

        PrefixExpression::PrefixExpression() : Node(NodeClassification::EXPRESSION), _right(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string PrefixExpression::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string PrefixExpression::as_string() const
        {
            return "(" + _op + _right->as_string() + ")";
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType PrefixExpression::get_type() const
        {
            return NodeType::PREFIX;
        }

        // #########################################################
        //                       INFIX EXPR
        // #########################################################

        InfixExpression::InfixExpression() : Node(NodeClassification::EXPRESSION), _left(nullptr), _right(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string InfixExpression::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string InfixExpression::as_string() const
        {
            return "(" + _left->as_string() + _op + _right->as_string() + ")";
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType InfixExpression::get_type() const
        {
            return NodeType::INFIX;
        }

        // #########################################################
        //                       IF EXPR
        // #########################################################

        IfExpression::IfExpression() : Node(NodeClassification::EXPRESSION),
                                       _condition(nullptr),
                                       _consequence(nullptr),
                                       _alternative(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string IfExpression::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string IfExpression::as_string() const
        {
            std::string res = "if " + _condition->as_string() + " " + _consequence->as_string();
            if (_alternative != nullptr)
            {
                res += " else  " + _alternative->as_string();
            }
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType IfExpression::get_type() const
        {
            return NodeType::IF;
        }

        // #########################################################
        //                       WHILE EXPR
        // #########################################################

        WhileExpression::WhileExpression() : Node(NodeClassification::EXPRESSION),
                                             _condition(nullptr),
                                             _consequence(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string WhileExpression::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string WhileExpression::as_string() const
        {
            return "while " + _condition->as_string() + " " + _consequence->as_string();
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType WhileExpression::get_type() const
        {
            return NodeType::WHILE;
        }

        // #########################################################
        //                       LET STATEMENT
        // #########################################################

        LetStatement::LetStatement() : Node(NodeClassification::STATEMENT), _value_expr(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        LetStatement::~LetStatement()
        {
            delete _value_expr;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string LetStatement::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string LetStatement::as_string() const
        {
            std::string res = token_literal() + " " + _name.as_string() + " = ";

            if(_value_expr != nullptr)
            {
                res += _value_expr->as_string();
            }
            else
            {
                res += "<< UNSET VALUE EXPRESSION >>";
            }
            res += ";";
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType LetStatement::get_type() const
        {
            return NodeType::LET;
        }

        // #########################################################
        //                       REF STATEMENT
        // #########################################################

        RefStatement::RefStatement() : Node(NodeClassification::STATEMENT), _value_expr(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        RefStatement::~RefStatement()
        {
            delete _value_expr;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string RefStatement::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string RefStatement::as_string() const
        {
            std::string res = token_literal() + " " + _name.as_string() + " = ";

            if(_value_expr != nullptr)
            {
                res += _value_expr->as_string();
            }
            else
            {
                res += "<< UNSET VALUE EXPRESSION >>";
            }
            res += ";";
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType RefStatement::get_type() const
        {
            return NodeType::REF;
        }

        // #########################################################
        //                       RETURN STATEMENT
        // #########################################################

        ReturnStatement::ReturnStatement() : Node(NodeClassification::STATEMENT), _return_expr(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        ReturnStatement::~ReturnStatement()
        {
            delete _return_expr;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string ReturnStatement::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string ReturnStatement::as_string() const
        {
            std::string res = token_literal();

            if(_return_expr != nullptr)
            {
                res += " ";
                res += _return_expr->as_string();
            }

            res += ";";
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType ReturnStatement::get_type() const
        {
            return NodeType::RETURN;
        }

        // #########################################################
        //                   EXPRESSION STATEMENT
        // #########################################################

        ExpressionStatement::ExpressionStatement() : Node(NodeClassification::STATEMENT), _expression(nullptr)
        {

        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        ExpressionStatement::~ExpressionStatement()
        {
            delete _expression;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string ExpressionStatement::token_literal() const
        {
            return _token._literal;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        std::string ExpressionStatement::as_string() const
        {
            std::string res = "";

            if(_expression != nullptr)
            {
                res += _expression->as_string();
            }

            res += ";";
            return res;
        }

        // ---------------------------------------------------------
        //
        // ---------------------------------------------------------

        NodeType ExpressionStatement::get_type() const
        {
            return NodeType::EXPR;
        }
    }
}