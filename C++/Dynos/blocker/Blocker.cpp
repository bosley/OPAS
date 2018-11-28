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
 * File:   Blocker.cpp
 * Author: Josh A. Bosley
 * 
 * Created on March 30, 2018, 11:27 PM
 */

#include "Blocker.h"

namespace blocks {
    
    
boost::mutex mutex;
    
bool CreateGenesis(std::string address)
{
    dhost::DynosDB db_man;
    char buffer[DYNOS_STANDARD_BUFFER];

    if(!db_man.getLocalConfigData("install_location", buffer))
    {
        std::cout << "Unable to load install location!";
        return false;
    }

    std::string install_loc(buffer);
    
    // Genesis data
    keyman::ckey key_man(install_loc);
    std::string prev_hash = dhost::SHA256("meteor!");
    std::string odata = "crash,bang,boom.life!";
    std::string pub = key_man.cgetPublicKey(address);
    std::string ssig = key_man.csignMessage(
            dhost::SHA256(address + odata), address);
    
    if(ssig.length() == 0)
    {
        std::cout << "\n\nUnable to sign genesis!\n";
        exit(1);
    }
    blocks::Block exogenesis(prev_hash, address, odata, pub, ssig);
    exogenesis.index = 0;
    if(!db_man.addVerifiedBlock("0", exogenesis.toJson()))
    {
        std::cout << "Error adding genesis!\n";
        return false;
    }
    return true;
}
    
Blocker::Blocker() 
{
    height = 0;
}
Blocker::~Blocker() {}

bool Blocker::validateBlock(Block block)
{
    // Validate the block data here
    return false;
}

bool Blocker::validateBlocks()
{
    /*
      
     
     Load 'N' blocks from chain.blk and validate by batches
     
     
     */
    
    
    return true;
}

// Callable by RPC
bool Blocker::addBlock(Block newBlock)
{
    // RAII mutex
    boost::mutex::scoped_lock lock(mutex);
    
    // Assume all blocks require validation
    newBlock.requires_validation = true;
    
    keyman::ckey key_man(DYNOS_NIL_FILE);
    
    
    
    //ensure string is same length as hydro signature bytes
    
    
    // Convert the signature to a uint8_t
    std::vector<uint8_t> sigv(
        newBlock.origin_sig.begin(), newBlock.origin_sig.end());
    uint8_t *psig = &sigv[0];
    
    // Verify the signature
    if(!key_man.cverifySig(newBlock.origin_sig, newBlock.getHash(), newBlock.origin_pubkey))
        return false;
    
    newBlock.requires_validation = false;
         
    // Set previous hash 
    Block pblock = blockCache.rbegin()[1];
    newBlock.prev_hash = pblock.hash;
    
    // Hash the block, and give index
    newBlock.fullHash();
    newBlock.index = height++;
    
    dhost::DynosDB db_man;
    if(!db_man.addVerifiedBlock(std::to_string(newBlock.index), newBlock.toJson()))
    {
        std::cout << "Error adding genesis!\n";
        return false;
    }
    
    // Store it in mem
    blockCache.push_back(newBlock);
    return true;
}


}


