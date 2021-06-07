//
// Created by bosley on 3/30/20.
//

#include "Response.hpp"

namespace ORBIT
{

    Response::Response() : resultCode(Response::Code::UNKNOWN),
                            description("") {

    }

    Response::Response(Code code) : resultCode(code),
                                    description("") {

    }

    Response::Response(Code code, std::string description) : resultCode(code),
                                                             description(description) {

    }

    Response::Response(Code code, std::string description, json data) : resultCode(code),
                                                                        description(description),
                                                                        resultData(data) {

    }

    Response::Code Response::getCode() const {

        return resultCode;
    }

    std::string Response::getDescription() const {

        return description;
    }

    json Response::getResultData() const {

        return resultData;
    }

}