#ifndef MSG_FORMATTER_HPP
#define MSG_FORMATTER_HPP

#include <nlohmann/json.hpp>

//!
//! \file Formatter.hpp
//! \brief Message formatter tool
//!
namespace OBELUS
{
    //!
    //! \class Formatter
    //! \brief Formats json data to get prepare for network transport
    //!
    class Formatter
    {
    public:
        //!
        //! \brief Construct Formatter
        //!
        Formatter();

        //!
        //! \brief Deconstruct Formatter
        //!
        ~Formatter();

        //!
        //! \brief Convert a json object to a string 
        //! \param data The json data
        //! \param result The string of the resulting conversion
        //! \retval true result is populated correctly
        //! \retval false result was unable to be populated
        //!
        bool jsonToString(nlohmann::json data, std::string &result);

        //!
        //! \brief Convert a string into a json object
        //! \param data The string'd data
        //! \param result The string of the resulting conversion
        //! \retval true result is converted correctly
        //! \retval false result was unable to be converted
        //!
        bool stringToJson(std::string data, nlohmann::json &result);
    };
}

#endif