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
 * File:   Blocker.h
 * Author: Josh A. Bosley
 *
 * Created on March 30, 2018, 11:27 PM
 * 
 *      All functions RPC can bind to must be tagged, and
 *      given a scoped mutex. 
 * 
 *          - Validation should not be remotely triggered
 * 
 */

#ifndef BLOCKER_H
#define BLOCKER_H

#include <string>
#include <vector>
#include <iterator>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

#include "../host/helpers.h"
#include "../ckey/ckey.h"
#include "../host/DydnoDB.h"


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace blocks
{

// Spawn the genesis block
bool CreateGenesis(std::string address);

class Block;

// Block manager
class Blocker {
public:
    Blocker();
    virtual ~Blocker();
    
    bool validateBlocks();
    bool addBlock(Block newBlock);
    
private:
    std::vector<Block> blockCache;
 
    bool validateBlock(Block block);
    
    int64_t height;
};
  
// Left in the header because of how basic it is, 
// and to differentiate it from any callable entity
class Block {    
public:
    Block(){}
    
    // Should only be called by CreateGenesis
    Block(std::string prev_hash,
            std::string origin_address, std::string data, 
            std::string origin_pubkey, std::string origin_sig)
    {
        this->requires_validation = false;
        this->prev_hash = prev_hash;
        this->block_data = data;
        this->origin_address = origin_address;
        this->origin_pubkey = origin_pubkey;
        this->origin_sig = origin_sig;
        this->hash = getHash();
    }
    
    // All RPC constructed blocks must use this constructor
    Block(std::string origin_address, std::string origin_pubkey,
            std::string origin_sig)
    {
        this->origin_address = origin_address;
        this->origin_pubkey = origin_pubkey;
        this->origin_sig = origin_sig;
        this->requires_validation = true;
    }
   
    std::string toJson()
    {
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value object(rapidjson::kObjectType);
        
	object.AddMember("index", this->index, allocator);
	object.AddMember("hash", 
                rapidjson::Value(this->hash.c_str(), allocator).Move(), 
                allocator);
	object.AddMember("prev_hash", 
                rapidjson::Value(this->prev_hash.c_str(), allocator).Move(), 
                allocator);
	object.AddMember("origin_address", 
                rapidjson::Value(this->origin_address.c_str(), allocator).Move(), 
                allocator);
	object.AddMember("origin_pubkey", 
                rapidjson::Value(this->origin_pubkey.c_str(), allocator).Move(), 
                allocator);
	object.AddMember("block_data", 
                rapidjson::Value(this->block_data.c_str(), allocator).Move(), 
                allocator);
	object.AddMember("origin_sig", 
                rapidjson::Value(this->origin_sig.c_str(), allocator).Move(), 
                allocator);
	document.AddMember("block_definition", object, allocator);
 
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	document.Accept(writer);
	return strbuf.GetString();
    }
    
    bool fromJson(std::string jsonData)
    {
        rapidjson::Document d;
        d.Parse(jsonData.c_str());
        if(!d.HasMember("block_definition"))
            return false;
        rapidjson::Value& definition = d["block_definition"];
        if(!definition.HasMember("index"))
            return false;
        if(!definition.HasMember("hash"))
            return false;
        if(!definition.HasMember("prev_hash"))
            return false;
        if(!definition.HasMember("origin_address"))
            return false;
        if(!definition.HasMember("origin_pubkey"))
            return false;
        if(!definition.HasMember("block_data"))
            return false;
        if(!definition.HasMember("origin_sig"))
            return false;
        rapidjson::Value& i = definition["index"];
        rapidjson::Value& h = definition["hash"];
        rapidjson::Value& ph = definition["prev_hash"];
        rapidjson::Value& oa = definition["origin_address"];
        rapidjson::Value& op = definition["origin_pubkey"];
        rapidjson::Value& bd = definition["block_data"];
        rapidjson::Value& os = definition["origin_sig"];
        this->index = i.GetUint();
        this->hash = h.GetString();
        this->prev_hash = ph.GetString();
        this->origin_address = oa.GetString();
        this->origin_pubkey = op.GetString();
        this->block_data = bd.GetString();
        this->origin_sig = os.GetString();
        this->requires_validation = true;
        return true;
    }
    
    std::string getHash()
    {
        return dhost::SHA256( this->origin_address + this->block_data);
    }
    
    bool fullHash()
    {
        if(this->requires_validation)
            return false;
        this->hash = dhost::SHA256(
                this->prev_hash +
                this->origin_address + 
                this->origin_pubkey +
                this->block_data + 
                this->origin_sig);
        return true;
    }
    
    std::string hash;
    std::string prev_hash;
    std::string origin_address;
    std::string origin_pubkey;
    std::string origin_sig;
    std::string block_data;
    bool requires_validation;
    int64_t index;
};

}

#endif /* BLOCKER_H */

