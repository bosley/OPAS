//
// Created by Josh Bosley on 6/4/21.
//

#ifndef DEVIK_LANG_KEYWORDS_HPP
#define DEVIK_LANG_KEYWORDS_HPP

#include <map>

namespace LANG
{
    //!
    //! \brief Object that contains all of the Devik keywords
    //!
    class Keywords
    {
    public:
        //!
        //! \brief Construct the keywords object
        //!
        Keywords();

        //!
        //! \brief Attempt to locate an identifier within the keywords
        //! \returns TOKEN::IDENT if the item is _not_ a keyword,
        //!          and returns the keyword if ident _is_ a keyword
        //!
        std::string lookup_ident(const std::string& ident);

    private:
        std::map<std::string, std::string> _keywords;
    };
}

#endif //DEVIK_LANG_KEYWORDS_HPP
