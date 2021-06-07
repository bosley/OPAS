// Node
#include "hpic.hpp"
#include "lua.hpp"

// Common
#include "Constants.hpp"
#include "Formatter.hpp"
#include "Wrapper.hpp"
#include "Callbacks.hpp"

// Submodules
#include <argagg/argagg.hpp>
#include <butane/NETWriter.hpp>
#include <butane/NETTcpServer.hpp>

// std
#include <iostream>
#include <thread>
#include <csignal>
#include <atomic>

namespace 
{
    // Default HPIC
    int HpicPort = 8000;
    std::string HpicAddress = "127.0.0.1";

    // Porgram running flag
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

        { "hpica", {"-b", "--hpicAddress"},
        "IP address of the HPIC server ", 1},
        
        { "hpicp", {"-q", "--hpicPort"},
        "Port of the HPIC server ", 1},
        
        { "script", {"-s", "--script"},
        "Lua script path ", 1},
    }};
}

// -----------------------------------------------------------
// run
// -----------------------------------------------------------

void run(std::string ip, int port)
{
    isRunning = true;
    std::signal(SIGINT, interrupt);

    //
    //  Start HPIC serving thread
    //
    std::thread hpicThread([]()
    {
        BUTANE::NET::TcpServer hpicServer(APP::HpicHandler, OBELUS::ErrorCallback, HpicPort, HpicAddress);
    
        while(isRunning)
        {
            hpicServer.tick();
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }

    });

    hpicThread.detach();

    //
    //  Start LUA serving thread
    //
    std::thread luaThread([](std::string ip, int port)
    {
        BUTANE::NET::TcpServer luaServer(APP::LuaHandler,   OBELUS::ErrorCallback, port, ip);

        while(isRunning)
        {
            luaServer.tick();
            APP::LuaTickOutbound();

            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }

    }, ip, port);

    luaThread.detach();

    //
    //  Keep the main thread alive
    //
    while(isRunning)
    {
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }
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

    if (args["help"]) 
    {
        argagg::fmt_ostream fmt(std::cerr);
        fmt << "Usage: program [options] ARG1 ARG2" << std::endl
            << parser;
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

    if(!args["script"])
    {
        std::cerr << "Error: -s or --script not given. Use -h for help." << std::endl;
        return 1;
    }

    std::string script = args["script"];

    // -------------------------------------------------

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

    // -------------------------------------------------

    if(args["hpica"])
    {
        std::string hpica = args["hpica"];
        HpicAddress = hpica;
    }

    if(args["hpicp"])
    {
        try
        {
            HpicPort = args["hpicp"];
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: Could not parse --hpicPort, an integer is required" << std::endl;
            return 1;
        }
    }

    // -------------------------------------------------

    if(address == HpicAddress && port == HpicPort)
    {
        std::cerr << "Can not serve with the settings (ip=" << HpicAddress << ", port=" << HpicPort << ")" << std::endl;
        std::cerr << "These settings are used for the HPIC service" << std::endl;
        return 1;
    }

    if(!APP::LuaPreProcess(address, port, script))
    {
        std::cerr << "Error preprocessing lua script" << std::endl;
        return 1;
    }
    
    // -------------------------------------------------

    APP::HpicInformation hpInfo;
    hpInfo.HpicIp   = HpicAddress;
    hpInfo.HpicPort = HpicPort;
    hpInfo.LuaIP    = address;
    hpInfo.LuaPort  = port;
    hpInfo.serverDescription = "Default server description that one day needs to be updated!";
    hpInfo.serverVersion = "0.0.1";

    // Set HPIC Information
    APP::HpicSetInfo(hpInfo);

    // Run the node
    run(address, port);

    return 0;
}