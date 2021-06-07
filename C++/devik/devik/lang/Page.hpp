//
// Created by Josh Bosley on 6/3/21.
//

#ifndef DEVIK_LANG_PAGE_HPP
#define DEVIK_LANG_PAGE_HPP

#include <string>

namespace LANG
{
    //!
    //! \brief A representation of a source file
    //!
    class Page
    {
    public:
        //!
        //! \param file The file _name to load into the page
        //!
        bool load(const std::string& file);

        //!
        //! \returns length of the loaded page
        //!
        size_t get_len();

        //!
        //! \param pos The _position to check if within range of source read-in
        //! \returns true iff a char is present to be read in
        //!
        bool is_valid(size_t pos);

        //!
        //! \param pos Position to retrieve char from
        //! \returns The char at the requested _position
        //! \note check if _position is valid prior to calling
        //!
        char char_at(size_t pos);

    private:

        std::string _page;
    };
}

#endif //DEVIK_LANG_PAGE_HPP
