#include "Formatter.hpp"

#include <sstream>

namespace OBELUS
{
    // -----------------------------------------------
    // Formatter
    // -----------------------------------------------

    Formatter::Formatter()
    {
    }

    // -----------------------------------------------
    // ~Formatter
    // -----------------------------------------------

    Formatter::~Formatter(){}

    // -----------------------------------------------
    // jsonToString
    // -----------------------------------------------

    bool Formatter::jsonToString(nlohmann::json data, std::string &result)
    {
        try
        {
            result = data.dump();
        }
        catch (nlohmann::json::parse_error& e)
        {
            return false;
        }
        catch (nlohmann::json::other_error& e)
        {
            return false;
        }
        catch(nlohmann::json::type_error& e)
        {
            return false;
        }
        return true;
    }

    // -----------------------------------------------
    // stringToJson
    // -----------------------------------------------

    bool Formatter::stringToJson(std::string data, nlohmann::json &result)
    {
        try
        {
            std::stringstream ss;
            ss << data;
            ss >> result;
        }
        catch (nlohmann::json::parse_error& e)
        {
            return false;
        }
        catch (nlohmann::json::other_error& e)
        {
            return false;
        }
        catch(nlohmann::json::type_error& e)
        {
            return false;
        }
        return true;
    }
}