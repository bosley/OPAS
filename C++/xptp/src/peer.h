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
 * File:   peer.h
 * Author: Josh A. Bosley
 *
 * Created on April 2, 2018, 8:45 PM
 */

#ifndef PEER_H
#define PEER_H

#include <string>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <boost/filesystem.hpp>
#include <boost/atomic.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include "network.h"
#include "xperr.h"


namespace xptp
{
    typedef boost::signals2::signal<void(xperr)> xptpsig;
    typedef boost::signals2::signal<void(std::string)> xptpinfosig;
    
    typedef xptpsig::slot_type xslot;
    typedef xptpinfosig::slot_type xislot;
    
class peer
{
public:
    peer(std::string config="./config.json", bool create=true, bool verbose=true);
    ~peer();
    
    xptpsig ErrorSignal;
    xptpsig WarningSignal;
    xptpinfosig InfoSignal;
    
    // Start the peer
    bool activate();
    
    // Stop the peer
    bool stop();
    
    // Manually add (ensure) a seed exists
    void push_seed(std::string ipv4, unsigned port);
    
private:
    bool writeConfig();
    bool readConfig();
    boost::filesystem::path _config;
    xnet::network _network;
};
 
}
#endif /* PEER_H */

