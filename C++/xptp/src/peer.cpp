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
 * File:   peer.cpp
 * Author: Josh A. Bosley
 * 
 * Created on April 2, 2018, 8:45 PM
 */

#include "peer.h"

namespace xptp
{
    boost::atomic<bool> peerRunning (false);
    
   
peer::peer(std::string config, bool create, bool verbose) 
{
    _config = boost::filesystem::path(config);

    if(boost::filesystem::is_regular_file(_config))
    {
        if(!readConfig())
        {
            if(verbose)
                std::cout << 
                        "Bad config file: " << config << "\n";
            exit(1);
        } else {
            if(verbose)
                std::cout << "Using network: " << _network.getName() << 
                        " with " << _network.getSeeds().size() << " seeds.\n\n";
        }
    } else if (create) {
        
        std::set<std::string> blankSeeds;
        _network.setNetwork("root", blankSeeds);
        if(!writeConfig())
        {
            if(verbose)
                std::cout << 
                        "Unable to write config file.\n";
            exit(1);
        }
        
    } else {
        std::cout << config << " was not found - and flag 'create' was not set!";
        exit(1);
    }
}

peer::~peer()
{
    if(!writeConfig())
    {
        InfoSignal("Unable to write config!");
    }
}

bool peer::writeConfig()
{
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value object(rapidjson::kObjectType);
    
    rapidjson::Value a(rapidjson::kArrayType);
    std::set<std::string> tseeds = _network.getSeeds();
    for(auto i = tseeds.begin(); i != tseeds.end(); ++i)
    {
        rapidjson::Value t;
        t.SetString((*i).c_str(), (*i).length(), allocator);
        a.PushBack(t, allocator);
        
    }
    std::string tnet = _network.getName();
    object.AddMember("network", 
            rapidjson::Value(tnet.c_str(), allocator).Move(),
            allocator);
    object.AddMember("seeds", a, allocator);
    document.AddMember("network_definition", object, allocator);

    boost::filesystem::ofstream ofs(_config);
    rapidjson::OStreamWrapper osw(ofs);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    document.Accept(writer);
    return true;
}

bool peer::readConfig()
{
    boost::filesystem::ifstream ifs(_config);
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document d;
    d.ParseStream(isw);
    if(!d.HasMember("network_definition"))
    {
        return false;;
    }
    rapidjson::Value& definition = d["network_definition"];
    if(!definition.HasMember("network"))
        return false;
    if(!definition.HasMember("seeds"))
        return false;

    rapidjson::Value& n = definition["network"];
    rapidjson::Value& s = definition["seeds"];
    
    std::string net = n.GetString();
    std::set<std::string> seeds;
    for (rapidjson::SizeType i = 0; i < s.Size(); i++) 
    {
        const rapidjson::Value& entry = s[i];
        seeds.insert(entry.GetString());
    }
    _network.setNetwork(net, seeds);
    return true;
}

bool peer::activate()
{
 //   running = true;
    
    ErrorSignal(xperr(XPERR_ERROR, 24, "Test errr"));
    
    
    return false;
}

bool peer::stop()
{
  //  running = false;
    return false;
}

void peer::push_seed(std::string ipv4, unsigned port)
{
    if(peerRunning)
    {
        ErrorSignal(xperr(XPERR_ERROR, DIABLED_WHILE_ACTIVE, 
                "Unable to push seed while running"));
        return;
    }
    std::string seed = ipv4 + ":" + std::to_string(port);
    _network.ensureSeed(seed);
}

}