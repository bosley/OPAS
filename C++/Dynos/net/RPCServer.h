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
 * File:   RPCServer.h
 * Author: Josh A. Bosley
 *
 * Created on March 31, 2018, 1:43 AM
 */

#ifndef RPCSERVER_H
#define RPCSERVER_H

#define DYNOS_RPC_SERVER_PORT 8080
#define DYNOS_RPC_ASYNC_WORKERS 1024

#include <map>
#include <utility>
#include <chrono>
#include <queue>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <rpc/server.h>
#include "../host/DydnoDB.h"
#include "../host/helpers.h"
#include "../blocker/Blocker.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace dnet
{
struct seed{
    unsigned port;
    std::string ip;
    std::string daddress;
    std::chrono::duration<double> speed;
    const seed& operator=(const seed other)
    {
        this->port = other.port; this->ip = other.ip;
        this->daddress = other.daddress; this->speed = other.speed;
        return *this;
    }
};

void SeedPrior();
void StartRPC();
void StopRPC();
}
#endif /* RPCSERVER_H */

