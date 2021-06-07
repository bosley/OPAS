//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_LANG_SOURCETREE_HPP
#define DEVIK_LANG_SOURCETREE_HPP

#include "lang/Ast.hpp"
#include "lang/Page.hpp"

#include <string>
#include <unordered_map>

namespace LANG
{
    //!
    //! \brief The source code for a given program
    //!
    class SourceTree
    {
    public:
        //!
        //! \brief Construct the source tree
        //!
        SourceTree() = default;

        //!
        //! \brief Destroy the source tree and all ASTs
        //!
        ~SourceTree();

        //!
        //! \param file The file to start reading in source code from
        //! \return true iff the source file was able to be loaded
        //!
        bool load_source(const std::string& file);

        //!
        //! \returns requested tree iff a source has been loaded for it,
        //!          otherwise a nullptr is returned
        //!
        LANG::AST::Program* get_tree(const std::string& tree);

    private:
        std::unordered_map<std::string, LANG::AST::Program*> _trees;
    };
}

#endif //DEVIK_LANG_SOURCETREE_HPP
