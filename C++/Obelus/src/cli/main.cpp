
#include "Formatter.hpp"
#include "Wrapper.hpp"
#include "Callbacks.hpp"
#include "Constants.hpp"
#include <butane/NETTcpServer.hpp>

#include <argagg/argagg.hpp>
#include <nlohmann/json.hpp>
#include <butane/NETWriter.hpp>

#include <iostream>
#include <thread>
#include <sstream>
#include <atomic>
#include <csignal>
#include <iomanip>

namespace
{
    int outboundPort;
    std::string outboundAddress;
    std::string outboundMessage;

    int localPort;
    std::string localAddress;

    std::string localStype;

    std::atomic<bool> isRunning;
}

// -----------------------------------------------------------
// interrupt
// -----------------------------------------------------------

void interrupt(int sig)
{
    std::cout << "Server received signal [" << sig << "] : Shutting down" << std::endl;
    isRunning = false;
}

// -----------------------------------------------------------
// displayResult
// -----------------------------------------------------------

void displayResult(std::string result)
{
    nlohmann::json jsonResult;
    OBELUS::Formatter formatter;

    if(!formatter.stringToJson(result, jsonResult))
    {
        std::cerr << "Unable to json display result: " << result << std::endl;;
        return;
    }
    
    std::cout << std::setw(4) << jsonResult << std::endl;
}


// -----------------------------------------------------------
// lusServerCallback (Required to get lua data back)
// -----------------------------------------------------------

void luaServerCallback(BUTANE::NET::Socket socket)
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

    displayResult(dataString);

    socket.socketClose();

    isRunning = false;

    return;
}

// -----------------------------------------------------------
// formatOutbound 
// -----------------------------------------------------------

std::string formatOutbound(std::string data)
{
    OBELUS::TxInfo sender;
    sender.ip   = localAddress;
    sender.port = localPort;

    OBELUS::TxInfo recvr;
    recvr.ip    = outboundAddress;
    recvr.port  = outboundPort;

    // Format the data from lua into json - If it fails, quit
    OBELUS::Formatter formatter;
    nlohmann::json jsonData;
    if(!formatter.stringToJson(data, jsonData))
    {
        std::cerr << "Unable to ready a response from : " << data << std::endl;
        exit(1);
    }

    // Wrap the data together
    OBELUS::Wrapper wrapper;
    nlohmann::json result;
    if(!wrapper.wrapMessage(sender, recvr, OBELUS::TransportMethod::TCP, jsonData, result))
    {
        std::cerr << "Unable to wrap lua response" << std::endl;
        exit(1);
    }

    // Get the network-ready string
    std::string formattedMessage;
    if(!formatter.jsonToString(result, formattedMessage))
    {
        std::cerr << "Unable to dump result to string" << std::endl;
        exit(1);
    }

    return formattedMessage;
}

// -----------------------------------------------------------
// writerReady
// -----------------------------------------------------------

void writerReady(BUTANE::NET::Socket socket)
{
    std::string message = formatOutbound(outboundMessage);
    
    std::string len = std::to_string(message.length());
    char expectBuffer[OBELUS::CONSTANTS::MAX_BUFFER_VALIDATION];
    strcpy(expectBuffer, len.c_str());

    socket.socketWriteOut(expectBuffer, sizeof(expectBuffer));
    
    socket.socketWriteOut((char*)message.c_str(), message.length());

    // If it is hpic it will send back a response immediatly so we can get the data and display
    if(localStype == "hpic")
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

        displayResult(dataString);
    }

    socket.socketClose();
}

// -----------------------------------------------------------
// getArgParser
// -----------------------------------------------------------

argagg::parser getArgParser()
{
    return argagg::parser 
    {{
        { "help", {"-h", "--help"},
        "shows this help message", 0},

        { "address", {"-a", "--address"},
        "IP address of the server ", 1},
        
        { "port", {"-p", "--port"},
        "Port of the server ", 1},

        { "localAddress", {"-l", "--localAddress"},
        "IP Address of the CLI ", 1},
        
        { "localPort", {"-n", "--localPort"},
        "Port of the CLI ", 1},

        { "stype", {"-t", "--serverType"},
        "Type of server [hpic, lua] ", 1},
        
        { "message", {"-m", "--message"},
        "Message to send", 1},
    }};
}

// -----------------------------------------------------------
// main
// -----------------------------------------------------------

int main(int argc, char **argv)
{
    argagg::parser parser = getArgParser();

    argagg::parser_results args;

    try 
    {
        args = parser.parse(argc, argv);
    } catch (const std::exception& e) 
    {
        std::cerr << "Error: Could not parse input arguments." << std::endl;
        return 1;
    }

    if(!args["address"])
    {
        std::cerr << "Error: -a or --address not given. Use -h for help." << std::endl;
        return 1;
    }

    if(!args["port"])
    {
        std::cerr << "Error: -p or --port not given. Use -h for help." << std::endl;
        return 1;
    }

    if(!args["localAddress"])
    {
        std::cerr << "Error: -l or --localAddress not given. Use -h for help." << std::endl;
        return 1;
    }

    if(!args["localPort"])
    {
        std::cerr << "Error: -n or --localPort not given. Use -h for help." << std::endl;
        return 1;
    }

    if(!args["stype"])
    {
        std::cerr << "Error: -t or --serverType not given. Use -h for help." << std::endl;
        return 1;
    }

    if(!args["message"])
    {
        std::cerr << "Error: -m or --message not given. Use -h for help." << std::endl;
        return 1;
    }

    std::string address = args["address"];

    int port;
    try
    {
        port = args["port"];
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: Could not parse --port, an integer is required" << std::endl;
        return 1;
    }

    std::string iLocalAddress = args["localAddress"];

    int iport;
    try
    {
        iport = args["localPort"];
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: Could not parse --localPort, an integer is required" << std::endl;
        return 1;
    }

    std::string stype = args["stype"];

    if(stype != "lua" && stype != "hpic")
    {
        std::cerr << "Error: -t / --serverType must be [lua, or hpic]" << std::endl;
        return 1;
    }

    std::string message = args["message"];

    outboundMessage = message;
    outboundAddress = address;
    outboundPort = port;
    localStype = stype;
    localAddress = iLocalAddress;
    localPort = iport;

    std::signal(SIGINT, interrupt);
    
    isRunning = true;

    // If LUA type, we can't get data from socket immediatly, so a server needs to run to intercept 
    // the responses
    if(localStype == "lua")
    {
        std::thread luaThread([](std::string ip, int port)
        {
            BUTANE::NET::TcpServer luaServer(luaServerCallback,   OBELUS::ErrorCallback, port, ip);

            while(isRunning)
            {
                luaServer.tick();

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100)
                );
            }

        }, localAddress, localPort);

        luaThread.detach();
    }

    // Write ther request
    BUTANE::NET::Writer writer(writerReady, OBELUS::ErrorCallback, BUTANE::NET::WriterType::TCP, address, port);
    writer.callWriter();

    // Keep alive so we can get lua info back
    if(localStype == "lua")
    {
        while(isRunning)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }
    }
    return 0;
}