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
 * File:   DydnoDB.h
 * Author: Josh A. Bosley
 *
 * Created on April 1, 2018, 11:12 AM
 */

#ifndef DYDNODB_H
#define DYDNODB_H

#define DYNOS_LOCAL_CONFIG "dynos.db"
#define DYNOS_STANDARD_BUFFER 1024

#include <vector>
#include <db_cxx.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>



namespace dhost{
    
class DynosDB 
{
public:
    DynosDB();
    DynosDB(const DynosDB& orig);
    virtual ~DynosDB();
    
    bool writePrimaryConfig(std::string install_location, 
        std::string op_mode, std::string address);
    
    bool initDB(std::string dbname, std::string dbloc);
    bool getLocalConfigData(std::string key, char *buffer);
    
    // Block-file functions
    bool addVerifiedBlock(std::string height, std::string blockData);
    
    // Seed-File functions
    void flushSeeds();
    bool addSeed(std::string number, std::string seedData);
    std::vector<std::string> readSeeds();
    
    
private:
    void putd(Db* pdb, std::string key, std::string val);
    bool sget(Db* pdb, std::string key, char *requestBuffer, int bufferlen);
};

}

#endif /* DYDNODB_H */

