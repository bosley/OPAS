/*
 * The MIT License
 *
 * Copyright 2018 Josh A. Bosley.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* 
 * File:   RPCServer.cpp
 * Author: Josh A. Bosley
 * 
 * Created on March 31, 2018, 1:43 AM
 */

#include "RPCServer.h"


namespace dnet
{
    
static std::string localhostname;
static rpc::server rpcserver(DYNOS_RPC_SERVER_PORT);
static blocks::Blocker blockman;

static std::vector<seed> seedCache;
static std::priority_queue<seed> seedQueue;
boost::atomic<bool> seedPriorRun(true);


/*   SEEDS      */
std::string seedToJson(seed sout)
{
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value object(rapidjson::kObjectType);
    object.AddMember("port", sout.port, allocator);
    object.AddMember("ip", 
            rapidjson::Value(sout.ip.c_str(), allocator).Move(), 
            allocator);
    object.AddMember("daddress", 
            rapidjson::Value(sout.daddress.c_str(), allocator).Move(), 
            allocator);
    document.AddMember("seed_definition", object, allocator);
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    document.Accept(writer);
    return strbuf.GetString();
}

seed seedFromJson(std::string jsonData)
{
    seed tseed;
    tseed.ip = "NOT_SET";
    rapidjson::Document d;
    d.Parse(jsonData.c_str());
    if(!d.HasMember("seed_definition"))
        return tseed;
    rapidjson::Value& definition = d["seed_definition"];
    if(!definition.HasMember("port"))
        return tseed;
    if(!definition.HasMember("ip"))
        return tseed;
    if(!definition.HasMember("daddress"))
        return tseed;
    rapidjson::Value& p = definition["port"];
    rapidjson::Value& i = definition["ip"];
    rapidjson::Value& de = definition["daddress"];
    tseed.port = p.GetUint();
    tseed.ip = i.GetString();
    tseed.daddress = de.GetString();
    return tseed;
}

// Write seed to db
void writeSeeds()
{
    int index = 0;
    dhost::DynosDB db_man;
    db_man.flushSeeds();
    for(auto i = seedCache.begin(); i != seedCache.end(); ++i)
    {
        db_man.addSeed(std::to_string(index++), seedToJson((*i)));
    }
}

// Read seeds from db into cache
void readSeeds()
{
    seedCache.clear();
    dhost::DynosDB db_man;
    std::vector<std::string> seedStrings = db_man.readSeeds();

    for(auto i = seedStrings.begin(); i != seedStrings.end(); ++i)
    {
        seed tseed = seedFromJson((*i));

        if(tseed.ip != "NOT_SET")
        {
            seedCache.push_back(tseed);
        } else {
            std::cout << "Unable to convert to seed\n";
        }
    }
}

 // Get a seed
std::pair<std::string, unsigned> getSeed()
{
    // Needs to be completed
    std::pair<std::string, unsigned> m;
    m.first = "0.0.0.0"; m.second = 0;
    return m;
}
    
// Set a new seed
bool setSeed(std::string address, std::string ip, unsigned port)
{
    return false;
}

/*  END SEED  */
   
/*  STANDARD RPC */
// Send away the dynos host name
std::string getAddress()
{
    return localhostname;
}

// Respond to a ping
std::string pong()
{
    return "pong";
}

/*  END STANDARD RPC */

/*  BLOCKS  */

// Send some block data
std::vector<blocks::Block> getBlocksSinceHeight(int32_t height)
{
    std::vector<blocks::Block> retBlocks;

    return retBlocks;
}
/*  END BLOCKS  */

void SeedPrior()
{
    while(seedPriorRun)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
}

// Start the RPC server
void StartRPC()
{
    std::cout << "Starting rpc server on port: " << 
            DYNOS_RPC_SERVER_PORT << std::endl;
    
    dhost::DynosDB db_man;
    char buff[256];
    db_man.getLocalConfigData("local_address", buff);
    localhostname = std::string(buff);

    
    // Blocks
    
    
    // Standard 
    rpcserver.bind("getAddress", &getAddress);
    rpcserver.bind("ping", &pong);
    
    
    rpcserver.async_run(DYNOS_RPC_ASYNC_WORKERS);
}

// Stop the RPC server
void StopRPC()
{
    std::cout << "Closing rpc sessions..\n";
    rpcserver.close_sessions();
    
    std::cout << "Complete\n" << "Stopping rpc server..\n";
    rpcserver.stop();
    
    seedPriorRun = false;
    std::cout << "Complete\n";
}

}
