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
 * File:   main.cpp
 * Author: Josh A. Bosley
 *
 * Created on April 2, 2018, 8:44 PM
 */

#include <iostream>

#include "src/peer.h"

#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <memory>
#include <boost/signals2.hpp>

using namespace std;
using namespace boost;

/*
    A handler to get info back from the peer class
    once up-and-running
 */
struct handler{
    void error(xptp::xperr e){
        cout << e << endl;
    }
    void warning(xptp::xperr e){
        cout << e << endl;
    }
    void info(std::string i){
        cout << "Got info: " << i << endl;
    }
} handle;

void setupSignals(xptp::peer * peer)
{
    peer->ErrorSignal.connect( xptp::xslot(&handler::error, handle, _1) );
    peer->WarningSignal.connect( xptp::xslot(&handler::warning, handle, _1) );
    peer->InfoSignal.connect( xptp::xislot(&handler::info, handle, _1) );
}

/*
 * 
 */
int main(int argc, char** argv) 
{
    xptp::peer peer;
    setupSignals(&peer);
    
    peer.push_seed("10.1.0.83", 9090);
    
    peer.activate();
    
    cout << "DONE\n";
    return 0;
    
    
}

