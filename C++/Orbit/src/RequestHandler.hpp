//
// Created by bosley on 3/30/20.
//

#ifndef ORBIT_REQUESTHANDLER_HPP
#define ORBIT_REQUESTHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

#warning Reminder - This class can be called from multiple threads. Since the thing is accessing physical resources we need to ensure that nothing terrible happens

namespace ORBIT
{
    class Server;

    class RequestHandler {
    public:
        RequestHandler();

        // Acceptors are local scripts / programs that take in the json information and fullfull the request
        // Destinations are directories / files that can be uploaded to / downloaded from directly

        // callpath is the path that the api will call to get the thing, path is the physical path to the thing
        void addPushAcceptor(std::string callPath, std::string path);
        void addRetrieveAcceptor(std::string callPath, std::string path);

        // callpath is the path that the api will call to get the thing, path is the physical path to the thing
        void addPushDestination(std::string callPath, std::string path);
        void addRetrieveDestination(std::string callpath, std::string path);

        // Allow the server to access private methods
        friend class Server;

    private:
        Response handleRequest(Request &request);

        // Internal routing
        Response infoRequest(Request &request);
        Response pushRequest(Request &request);
        Response retrieveRequest(Request &request);
    };

}


#endif //ORBIT_REQUESTHANDLER_HPP
