#include "hpic.hpp"
#include "Constants.hpp"
#include "Formatter.hpp"
#include "Wrapper.hpp"

#include <nlohmann/json.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <map>
#include <iterator>

namespace APP
{
    namespace
    {
        /*
        
                HPIC Settings
        
        */
        std::string HpicAddress;
        int HpicPort;

        /*
        
                Map requests to responses

        */
        std::map<std::string, std::string> HpicRequestReponseMap;
    }

    // ---------------------------------------------
    // readyResponse
    // Format and wrap the response into something the 
    // receiver can handle (JSON IT)
    //
    // ---------------------------------------------

    std::string readyResponse(std::string ipTo, int portTo, std::string data)
    {
        OBELUS::TxInfo sender;
        sender.ip   = HpicAddress;
        sender.port = HpicPort;

        OBELUS::TxInfo recvr;
        recvr.ip    = ipTo;
        recvr.port  = portTo;

        // Format the data from lua into json - If it fails, quit
        OBELUS::Formatter formatter;
        nlohmann::json jsonData;
        if(!formatter.stringToJson(data, jsonData))
        {
            std::cerr << "Unable to ready a response" << std::endl;
            return "{ \"response\", \"Error formatting response to request\" }";
        }

        // Wrap the data together
        OBELUS::Wrapper wrapper;
        nlohmann::json result;
        if(!wrapper.wrapMessage(sender, recvr, OBELUS::TransportMethod::TCP, jsonData, result))
        {
            std::cerr << "Unable to wrap lua response" << std::endl;
            return "{ \"response\", \"Error wrapping response to request\" }";
        }

        // Get the network-ready string
        std::string formattedMessage;
        if(!formatter.jsonToString(result, formattedMessage))
        {
            std::cerr << "Unable to dump result to string" << std::endl;
            return "{ \"response\", \"Error formatting response to request\" }";
        }

        return formattedMessage;
    }

    // ---------------------------------------------
    // handleRequeest
    // Get the particular response to the request
    //
    // ---------------------------------------------

    std::string handleRequeest(std::string request)
    {
        /*
            TODO: 

                Need to add secondary handling to process requests to shutdown, or
                change server settings.
        
        */
        std::map<std::string, std::string>::iterator it = HpicRequestReponseMap.find(request);
        if(it == HpicRequestReponseMap.end())
        {
            return std::string("Unknown Request");
        }
        else
        {
            return HpicRequestReponseMap[request];
        }
    }

    // ---------------------------------------------
    // processRequest
    // Process a request from string->json->request details
    //
    // ---------------------------------------------

    std::string processRequest(std::string request)
    {
        std::string response;

        // Get sender information
        OBELUS::Formatter formatter;

        nlohmann::json jsonRequest;
        if(!formatter.stringToJson(request, jsonRequest))
        {
            std::cerr << "Unable to json the string fed into HPIC" << std::endl;
            return response;
        }
        
        //
        // Get sender information
        //
        std::string senderIp;
        int senderPort;
        std::string jsonRequestData;

        try
        {
            senderIp = jsonRequest["senderIp"];             // Ip of the sender
            senderPort = jsonRequest["senderPort"];         // Port of the sender
            jsonRequestData = jsonRequest["data"]["hpic"];  // Only take hpic data
        }
        catch (nlohmann::json::parse_error& e)
        {
            std::cerr << "HPIC Error: " << e.what() << std::endl;
            return response;
        }
        catch (nlohmann::json::other_error& e)
        {
            std::cerr << "HPIC Error: " << e.what() << std::endl;
            return response;
        }
        catch(nlohmann::json::type_error& e)
        {
            std::cerr << "HPIC Error: " << e.what() << std::endl;
            return response;
        }

        return readyResponse(senderIp, senderPort, handleRequeest(jsonRequestData));
    }

    // ---------------------------------------------
    // HpicSetInfo
    // Setup information for handling responses
    //
    // ---------------------------------------------

    void HpicSetInfo(HpicInformation hpInfo)
    {
        HpicAddress = hpInfo.HpicIp;
        HpicPort    = hpInfo.HpicPort;

        // Set the requestable information
        HpicRequestReponseMap["getDesc"] = 
            std::string("{ \"desc\": \"") + 
            hpInfo.serverDescription + 
            std::string("\" }");

        HpicRequestReponseMap["getVersion"] = 
            std::string("{ \"version\": \"") + 
            hpInfo.serverVersion + 
            std::string("\" }");

        HpicRequestReponseMap["getLuaInfo"] = 
            std::string("{ \"lua\": \"") + 
            hpInfo.HpicIp + "@" + std::to_string(hpInfo.LuaPort) +  
            std::string("\" }");
    }
    // ---------------------------------------------
    // HpicHandler
    // Handle a socket connection from TCP Server
    // 
    // ---------------------------------------------

    void HpicHandler(BUTANE::NET::Socket socket)
    {
        char buffer[OBELUS::CONSTANTS::MAX_BUFFER_VALIDATION];

        if (socket.socketReadIn(buffer, OBELUS::CONSTANTS::MAX_BUFFER_VALIDATION) <= 0) 
        {
            socket.socketClose();
            return;
        }

        int expectedSize;
        std::stringstream ss;
        ss << buffer;
        ss >> expectedSize;

        if(expectedSize < 0 || expectedSize > OBELUS::CONSTANTS::TCP_MAX_BUFFER_DATA)
        {
            socket.socketClose();
            return;
        }

        char * dataBuffer = (char*)malloc(sizeof(char) * expectedSize);

        if (socket.socketReadIn(dataBuffer, expectedSize) <= 0) 
        {
            socket.socketClose();
            return;
        }

        std::string dataString = dataBuffer;

        free(dataBuffer);

        std::string requestResponse = processRequest(dataString);

        if(requestResponse.empty())
        {
            socket.socketClose();
            return;
        }

        // Send back the length of the request so they know how long the response is
        std::string len = std::to_string(requestResponse.length());
        char expectBuffer[OBELUS::CONSTANTS::MAX_BUFFER_VALIDATION];
        strcpy(expectBuffer, len.c_str());

        socket.socketWriteOut(expectBuffer, sizeof(expectBuffer));
        
        // Send the response
        socket.socketWriteOut((char*)requestResponse.c_str(), requestResponse.length());

        socket.socketClose();

        return;
    }
}