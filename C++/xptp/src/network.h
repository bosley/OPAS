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
 * File:   network.h
 * Author: Josh A. Bosley
 *
 * Created on April 2, 2018, 10:04 PM
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <set>
#include <list>
#include <string>
#include <iterator>

namespace xnet
{
struct active_peer{
    std::string ip;
    unsigned port;
};

class network
{
public:
    network();
    network(const network& orig);
    virtual ~network();
    
    void setNetwork(std::string name, std::set<std::string> seeds);
    
    std::string getName();
    std::set<std::string> getSeeds();
    
    void ensureSeed(std::string seed);
private:
    std::string _name;
    std::set<std::string> _seeds;
    std::list<active_peer> _peers;
};
}
#endif /* NETWORK_H */

