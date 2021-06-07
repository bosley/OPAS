//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_AST_HPP
#define DEVIK_AST_HPP

#include "Token.hpp"

#include <string>
#include <vector>

namespace LANG
{
    namespace AST
    {
        //!
        //! \brief Classification of nodes in the AST
        //!
        enum class NodeClassification
        {
            ROOT,
            STATEMENT,
            EXPRESSION
        };

        //!
        //! \brief Node types
        //!
        enum class NodeType
        {
            PROGRAM,
            INTEGER,
            BOOLEAN,
            STRING,
            ID,
            FN,
            CALL,
            INDEX_EXPR,
            ARRAY,
            PREFIX,
            INFIX,
            IF,
            WHILE,
            EXPR,
            BLOCK,
            RETURN,
            LET,
            REF
        };

        //!
        //! \brief An AST node
        //!
        class Node
        {
        public:
            Node(NodeClassification nc) : _classification(nc) { }
            virtual ~Node() {} ;
            virtual std::string token_literal() const = 0;
            virtual std::string as_string() const = 0;
            virtual NodeType get_type() const = 0;

            NodeClassification _classification;
        };

        //!
        //! \brief Root of the AST
        //!
        class Program : public Node
        {
        public:
            Program();
            ~Program();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;
            std::vector<Node*> _statements;
        };

        //!
        //! \brief A block of _statements
        //!
        class BlockStatement : public Node
        {
        public:
            BlockStatement();
            ~BlockStatement();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token       _token;
            std::vector<Node*> _statements;
        };

        //!
        //! \brief Identifier
        //!
        class Identifier : public Node
        {
        public:
            Identifier();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            std::string  _value;
        };

        //!
        //! \brief Integer Literal
        //!
        class IntegerLiteral : public Node
        {
        public:
            IntegerLiteral();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            int          _value;
        };

        //!
        //! \brief Boolean Literal
        //!
        class BooleanLiteral : public Node
        {
        public:
            BooleanLiteral();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            bool         _value;
        };

        //!
        //! \brief String Literal
        //!
        class StringLiteral : public Node
        {
        public:
            StringLiteral();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            std::string  _value;
        };

        //!
        //! \brief Function Literal
        //!
        class FunctionLiteral : public Node
        {
        public:
            FunctionLiteral();
            ~FunctionLiteral();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token             _token;
            std::vector<Identifier*> _parameters;
            BlockStatement*          _body;
        };

        //!
        //! \brief Array Literal
        //!
        class ArrayLiteral : public Node
        {
        public:
            ArrayLiteral();
            ~ArrayLiteral();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token       _token;
            std::vector<Node*> _elements;
        };

        //!
        //! \brief Call Expression
        //!
        class CallExpression : public Node
        {
        public:
            CallExpression();
            ~CallExpression();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token       _token;
            Node*              _function;
            std::vector<Node*> _arguments;
        };

        //!
        //! \brief Index Expression
        //!
        class IndexExpression : public Node
        {
        public:
            IndexExpression();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            Node*        _array;
            Node*        _index;
        };

        //!
        //! \brief Prefix Expression
        //!
        class PrefixExpression : public Node
        {
        public:
            PrefixExpression();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            std::string  _op;
            Node*        _right;
        };

        //!
        //! \brief Infix Expression
        //!
        class InfixExpression : public Node
        {
        public:
            InfixExpression();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            Node*        _left;
            std::string  _op;
            Node*        _right;
        };

        //!
        //! \brief If Expression
        //!
        class IfExpression : public Node
        {
        public:
            IfExpression();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token    _token;
            Node*           _condition;
            BlockStatement* _consequence;
            BlockStatement* _alternative;
        };

        //!
        //! \brief While Expression
        //!
        class WhileExpression : public Node
        {
        public:
            WhileExpression();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token    _token;
            Node*           _condition;
            BlockStatement* _consequence;
        };

        //!
        //! \brief Let Statement
        //!
        class LetStatement : public Node
        {
        public:
            LetStatement();
            ~LetStatement();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            Identifier   _name;
            Node*        _value_expr;
        };

        //!
        //! \brief Ref Statement
        //!
        class RefStatement : public Node
        {
        public:
            RefStatement();
            ~RefStatement();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            Identifier   _name;
            Node*        _value_expr;
        };

        //!
        //! \brief Return Statement
        //!
        class ReturnStatement : public Node
        {
        public:
            ReturnStatement();
            ~ReturnStatement();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            Node*        _return_expr;
        };

        //!
        //! \brief Expression Statement
        //!
        class ExpressionStatement : public Node
        {
        public:
            ExpressionStatement();
            ~ExpressionStatement();
            std::string token_literal() const override;
            std::string as_string() const override;
            NodeType get_type() const override;

            TOKEN::Token _token;
            Node*        _expression;
        };
    }
}


#endif //DEVIK_AST_HPP
