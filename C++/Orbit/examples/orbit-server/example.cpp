//
// Created by bosley on 3/28/20.
//

#include <orbit/Server.hpp>
#include <orbit/RequestHandler.hpp>

int main() {

    ORBIT::RequestHandler handler;

    // Add something on the 'puter that can take in some json and hand back whatever was asked to be retrieved
    handler.addRetrieveAcceptor("/logical/unit/one" ,"/var/orbit/resource/some_retrieve_acceptor.(py | sh | rb)");

    // Add something on the 'puter that takes in some json that will handle the uploaded information
    handler.addPushAcceptor("/logical/unit/two" ,"/var/orbit/resource/some_push_acceptor.(py | sh | rb)");

    // Allow the retrieval of file.txt by asking for /logical/unit/three
    handler.addRetrieveDestination("/logical/unit/three", "/var/orbit/destinations/retrieve/file.txt");

    // Allow the uploading of a file to the push directory by uploading to /logical/unit/four
    handler.addPushDestination("/logical/unit/four", "/var/orbit/destinations/push/");

    // Create a server with the request handler
    ORBIT::Server server(ORBIT::HostPort("127.0.0.1", 4096), handler);

    while(1) {

    }

    return 0;
}