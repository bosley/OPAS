//
// Created by bosley on 3/30/20.
//

#include "RequestHandler.hpp"

#include <iostream>

namespace ORBIT
{
    RequestHandler::RequestHandler() {

    }

    void RequestHandler::addPushAcceptor(std::string callPath, std::string path) {

#warning Need to add a path to the push acceptor to forward the push information to
        std::cerr << "addPushAcceptor(str, str) not yet implemented by the RequestHandler" << std::endl;
    }

    void RequestHandler::addRetrieveAcceptor(std::string callPath, std::string path) {

#warning Need to add a path to the retrieve acceptor to forward the retrieve information to
        std::cerr << "addRetrieveAcceptor(str, str) not yet implemented by the RequestHandler" << std::endl;
    }

    void RequestHandler::addPushDestination(std::string callPath, std::string path) {

#warning Need to add a path to the push destination to push information to
        std::cerr << "addPushDestination(str, str) not yet implemented by the RequestHandler" << std::endl;
    }

    void RequestHandler::addRetrieveDestination(std::string callpath, std::string path) {

#warning Need to add a path to the retrieve destination to retrieve information from
        std::cerr << "addRetrieveDestination(str, str) not yet implemented by the RequestHandler" << std::endl;
    }


    Response RequestHandler::handleRequest(Request &request) {

        switch(request.getType()) {

            case Request::Type::INFO:       return infoRequest(request);
            case Request::Type::PUSH:       return infoRequest(request);
            case Request::Type::RETRIEVE:   return infoRequest(request);
        }

        // This should never be reached. Its to shut the compiler up
        return Response(Response::Code::UNKNOWN, "Unhandled request type");
    }

    Response RequestHandler::infoRequest(Request &request) {

#warning Need to make an InfoResponse object that is json-able to construct and stuff into the responses data field
        return Response(Response::Code::OKAY, "Information Request has not yet been implemented by the RequestHandler");
    }

    Response RequestHandler::pushRequest(Request &request) {

#warning Need to make a PushResponse object that is json-able to construct and stuff into the responses data field
        return Response(Response::Code::UNKNOWN, "Push Request has not yet been implemented by the RequestHandler");
    }

    Response RequestHandler::retrieveRequest(Request &request) {

#warning Need to make a RetrieveResponse object that is json-able to construct and stuff into the responses data field
        return Response(Response::Code::UNKNOWN, "Retrieve Request has not yet been implemented by the RequestHandler");
    }

}