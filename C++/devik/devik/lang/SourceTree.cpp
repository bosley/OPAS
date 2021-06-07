//
// Created by Josh Bosley on 6/3/21.
//

#include "lang/SourceTree.hpp"

#include "lang/Lexer.hpp"
#include "lang/Parser.hpp"

#include <iostream>

namespace LANG
{
    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    SourceTree::~SourceTree()
    {
        for( auto & [file, tree] : _trees )
        {
            delete tree;
        }

        _trees.clear();
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    bool SourceTree::load_source(const std::string& file)
    {
        // If the file was already loaded indicate that it is
        // loaded and ready to go
        if( _trees.find(file) != _trees.end() )
        {
            return true;
        }

        // Load the file into a page
        Page page;

        if( !page.load(file) )
        {
            std::cerr << "Failed to load file : " << file << std::endl;
            return false;
        }

        // Create the _lexer
        LANG::Lexer lexer(page);

        // Create the parser
        LANG::Parser parser(lexer);

        // Get the program that was parsed
        LANG::AST::Program * program = parser.parse_program();

        // Check for _errors
        if( !parser.get_errors().empty() )
        {
            std::cerr << "Error(s) parsing : " << std::endl;

            for (auto & error : parser.get_errors())
            {
                std::cout << error << std::endl;
            }

            return false;
        }

        return true;
    }

    // ---------------------------------------------------------
    //
    // ---------------------------------------------------------

    LANG::AST::Program* SourceTree::get_tree(const std::string& tree)
    {
        if ( _trees.empty() )
        {
            return nullptr;
        }

        if ( _trees.find(tree) == _trees.end() )
        {
            return nullptr;
        }

        return _trees[tree];
    }
}