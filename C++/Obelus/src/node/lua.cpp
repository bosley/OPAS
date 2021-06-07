#include "lua.hpp"
#include "Constants.hpp"
#include "Callbacks.hpp"
#include "Wrapper.hpp"
#include "Formatter.hpp"

#include <butane/NETWriter.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <cstring>

extern "C"
{
    #include <lua5.3/lua.h>
    #include <lua5.3/lualib.h>
    #include <lua5.3/lauxlib.h>
}

namespace
{
    // Check return from LUA to ensure that it is LUA_OK
    bool CheckLua(lua_State *L, int r)
    {
        if(r != LUA_OK)
        {
            std::string err = lua_tostring(L, -1);
            std::cerr << "Error: " << err << std::endl;
            return false;
        }
        return true;
    }

    // Structure for data to write out to the network
    struct Lwd
    {
        std::string address;
        int port;
        std::string data;
    };

    std::queue<Lwd> LuaOutboundQueue;   // Queue of data from lua meant to be written out to the network

    int LuaPort;            // Information set by the user to determine how to reach lua function
    std::string LuaAddress; // Information set by the user to determine how to reach lua function
    std::string LuaScript;  // Information set by the user to determine how to reach lua function
}

extern "C" int LuaWriteTCP(lua_State* L)
{
    int result = -3;
    int port = 0;
    int routeTo = 0;
    std::string ipAddressTo;
    std::string data;

    // Get address
    if(!lua_isstring(L, 1))
    {
        return -1;  
    }
    ipAddressTo = lua_tostring(L, 1);

    // Get port
    if(!lua_isnumber(L, 2))
    {
        return -2;  
    }
    port = lua_tonumber(L, 2);

    // Get data
    if(!lua_isstring(L, 3))
    {
        return -3;  
    }
    data = lua_tostring(L, 3);

    Lwd lwd;
    lwd.address = ipAddressTo;
    lwd.port = port;
    lwd.data = data;

    // Write data
    LuaOutboundQueue.push(lwd);

    result = 0;

    return result;
}

namespace APP
{
    // -----------------------------------------------------------
    // feedLua
    // -----------------------------------------------------------

    void feedLua(std::string data)
    {
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);
        lua_register(L, "ObelusWrite", LuaWriteTCP);

        // Load the file and check to see it it is okay
        if(!CheckLua(L, luaL_dofile(L, LuaScript.c_str())))
        {
            std::cerr << "Unable to load lua file " << std::endl;
            return;
        }

        lua_getglobal(L, "Request");
        if(lua_isfunction(L, -1))
        {
            lua_pushstring(L, (char*)data.c_str());

            lua_pcall(L, 1, 1, 0);
        }
        else
        {
            std::cerr << "Lua method 'Request' was not found" << std::endl;
        }
        
        lua_close(L);
        return;
    }

    // -----------------------------------------------------------
    // LuaHandler
    // -----------------------------------------------------------

    void LuaHandler(BUTANE::NET::Socket socket)
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

        feedLua(dataString);

        socket.socketClose();

        return;
    }

    // -----------------------------------------------------------
    // LuaPreProcess
    // -----------------------------------------------------------

    bool LuaPreProcess(std::string ip, int port, std::string script)
    {
        LuaScript = script;
        LuaPort = port;
        LuaAddress = ip;

        lua_State *L = luaL_newstate();

        luaL_openlibs(L);

        // Load the file and check to see it it is okay
        if(!CheckLua(L, luaL_dofile(L, LuaScript.c_str())))
        {
            return false;
        }

        //
        //  Check to make sure required function exist within the script
        //
        lua_getglobal(L, "Request");

        if(!lua_isfunction(L, -1))
        {
            return false;
        }

        lua_close(L);
        return true;
    }

    // -----------------------------------------------------------
    // writerReady
    // -----------------------------------------------------------
    void WriterReady(BUTANE::NET::Socket socket)
    {
        // Crate tx info for sender and receiver
        OBELUS::TxInfo sender;
        sender.ip   = LuaAddress;
        sender.port = LuaPort;

        OBELUS::TxInfo recvr;
        recvr.ip    = LuaOutboundQueue.front().address;
        recvr.port  = LuaOutboundQueue.front().port;

        // Format the data from lua into json - If it fails, quit
        OBELUS::Formatter formatter;
         nlohmann::json jsonData;
        if(!formatter.stringToJson(LuaOutboundQueue.front().data, jsonData))
        {
            socket.socketClose();
            std::cerr << "Unable to format lua response" << std::endl;
            LuaOutboundQueue.pop();
            return;
        }

        // Wrap the data together
        OBELUS::Wrapper wrapper;
        nlohmann::json result;
        if(!wrapper.wrapMessage(sender, recvr, OBELUS::TransportMethod::TCP, jsonData, result))
        {
            socket.socketClose();
            std::cerr << "Unable to wrap lua response" << std::endl;
            LuaOutboundQueue.pop();
            return;
        }

        // Get the network-ready string
        std::string formattedMessage;
        if(!formatter.jsonToString(result, formattedMessage))
        {
            socket.socketClose();
            std::cerr << "Unable to dump lua result to string" << std::endl;
            LuaOutboundQueue.pop();
            return;
        }

        // Send back the length of the request so they know how long the response is
        std::string len = std::to_string(formattedMessage.length());
        char expectBuffer[OBELUS::CONSTANTS::MAX_BUFFER_VALIDATION];
        strcpy(expectBuffer, len.c_str());

        socket.socketWriteOut(expectBuffer, sizeof(expectBuffer));
        
        // Send the response
        socket.socketWriteOut((char*)formattedMessage.c_str(), formattedMessage.length());

        socket.socketClose();

        // Pop one off the queue
        LuaOutboundQueue.pop();
    }

    // -----------------------------------------------------------
    // LuaTickOutbound
    // -----------------------------------------------------------

    void LuaTickOutbound()
    {
        if(LuaOutboundQueue.empty())
        {
            return;
        }

        int processLimiter = 0;

        while(!LuaOutboundQueue.empty() && processLimiter != OBELUS::CONSTANTS::MAX_LUA_WRITE_BURST)
        {
            BUTANE::NET::Writer tcpWriter(
                WriterReady, 
                OBELUS::ErrorCallback, 
                BUTANE::NET::WriterType::TCP, 
                LuaOutboundQueue.front().address, 
                LuaOutboundQueue.front().port);

            tcpWriter.callWriter();

            processLimiter++;
        }
    }
}