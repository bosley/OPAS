//
// Created by bosley on 3/30/20.
//

#ifndef ORBIT_RESPONSE_HPP
#define ORBIT_RESPONSE_HPP

#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace ORBIT
{
    class Response {
    public:

#warning need to extend the response codes offered here
        enum class Code
        {
            UNKNOWN,
            OKAY,
            REQUEST_MALFORMED,
            RESOURCE_NOT_FOUND,
            RESOURCE_NOT_PERMITTED,
            RESOURCE_BUSY,
            SERVER_BUSY,
            SERVER_RESPONSE_MALFORMED
            };

        Response();
        Response(Code code);
        Response(Code code, std::string description);
        Response(Code code, std::string description, json data);

        Code        getCode() const;
        std::string getDescription() const;
        json        getResultData() const;

        friend void to_json(json &j, const Response &s);
        friend void from_json(const json &j, Response &r);

    private:
        Code        resultCode;
        std::string description;
        json        resultData;
    };

    //json serialization
    inline void to_json(json &j, const Response &r)
    {
        j["code"] = r.resultCode;
        j["description"] = r.description;
        j["data"] = r.resultData;
    }

    inline void from_json(const json &j, Response &r)
    {
        r.resultCode = j.at("code").get<Response::Code>();
        r.description = j.at("description").get<std::string>();
        r.resultData = j.at("data").get<std::string>();
    }

}
#endif //ORBIT_RESPONSE_HPP
