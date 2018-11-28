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
 * File:   DydnoDB.cpp
 * Author: Josh A. Bosley
 * 
 * Created on April 1, 2018, 11:12 AM
 */

#include "DydnoDB.h"

namespace dhost{

boost::mutex dbmutex;
    

DynosDB::DynosDB() {
}

DynosDB::DynosDB(const DynosDB& orig) {
}

DynosDB::~DynosDB() {
}

void DynosDB::putd(Db* pdb, std::string key, std::string val)
{
    boost::mutex::scoped_lock lock(dbmutex);
    Dbt k(const_cast<char*>(key.data()), key.size());
    Dbt v(const_cast<char*>(val.data()), val.size()+1);
    pdb->put(NULL, &k, &v, 0);
}

bool DynosDB::sget(Db* pdb, std::string key, char *requestBuffer, int bufferlen)
{
    boost::mutex::scoped_lock lock(dbmutex);
    Dbt data;
    data.set_data(requestBuffer);
    data.set_ulen(bufferlen);
    data.set_flags(DB_DBT_USERMEM);
    Dbt k(const_cast<char*>(key.data()), key.size());
    if (pdb->get(NULL, &k, &data, 0) == DB_NOTFOUND) 
    {
        std::cout << "NOT FOUND\n";
        return false;
    } else {
        return true;
    }
}

bool DynosDB::writePrimaryConfig(std::string ins_loc, std::string op_mode,
        std::string address)
{
    u_int32_t dflags = 0;
    DbEnv env(dflags);
    Db* pdb;
    try 
    {
        env.set_error_stream(&std::cerr);
        env.open("./local_config", DB_CREATE | DB_INIT_MPOOL, 0);

        pdb = new Db(&env, 0);
        pdb->open(NULL, DYNOS_LOCAL_CONFIG, NULL, DB_BTREE, DB_CREATE | DB_TRUNCATE, 0);
 
        putd(pdb, "install_location", ins_loc);
        putd(pdb, "operating_mode", op_mode);
        putd(pdb, "local_address", address);

        if(pdb != NULL)
        {
            pdb->close(0);
            delete pdb;
        }
        
        env.close(0);
    } catch (DbException& e) 
    {
        delete pdb;
        std::cerr << "DbException: " << e.what() << std::endl;
        return false;
    } catch (std::exception& e) 
    {
        delete pdb;
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool DynosDB::initDB(std::string dbname, std::string dbloc)
{
    u_int32_t dflags = 0;
    DbEnv env(dflags);
    Db* pdb;
    try 
    {
        env.set_error_stream(&std::cerr);
        env.open(dbloc.c_str(), DB_CREATE | DB_INIT_MPOOL, 0);
        pdb = new Db(&env, 0);
        pdb->open(NULL, dbname.c_str(), NULL, DB_BTREE, DB_CREATE | DB_TRUNCATE, 0);
        putd(pdb, "dynos_init_wipe_db", "DynosDB::initDB");
        
        if(pdb != NULL)
        {
            pdb->close(0);
            delete pdb;
        }
        env.close(0);
    } catch (DbException& e) 
    {
        std::cerr << "DbException: " << e.what() << std::endl;
        return false;
    } catch (std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

bool DynosDB::getLocalConfigData(std::string key, char *buffer)
{
    u_int32_t dflags = 0;
    DbEnv env(dflags);
    Db* pdb;
    bool found = true;
    try 
    {
        env.set_error_stream(&std::cerr);
        env.open("./local_config", DB_CREATE | DB_INIT_MPOOL, 0);
        pdb = new Db(&env, 0);
        pdb->open(NULL, DYNOS_LOCAL_CONFIG, NULL, DB_BTREE, 0, 0);
        
        if (!sget(pdb, key, buffer, DYNOS_STANDARD_BUFFER)) 
        {
            std::cout << "Not found\n";
            found = false;
        }
        if(pdb != NULL)
        {
            pdb->close(0);
            delete pdb;
        }
        env.close(0);
    } catch (DbException& e) 
    {
        std::cerr << "DbException: " << e.what() << std::endl;
        return false;
    } catch (std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return found;
}


bool DynosDB::addVerifiedBlock(std::string height, std::string blockData)
{
    char ccbuffer[DYNOS_STANDARD_BUFFER];

    if(!getLocalConfigData("install_location", ccbuffer))
    {
        std::cout << "DynosDB::addVerifiedBlock => Unable to load install location!";
        return false;
    }

    u_int32_t dflags = 0;
    DbEnv env(dflags);
    Db* pdb;
    try 
    {
        env.set_error_stream(&std::cerr);
        env.open(ccbuffer, DB_CREATE | DB_INIT_MPOOL, 0);
        pdb = new Db(&env, 0);
        pdb->open(NULL, "chain.db", NULL, DB_BTREE, DB_CREATE, 0);
        putd(pdb, height, blockData);
        
        if(pdb != NULL)
        {
            pdb->close(0);
            delete pdb;
        }
        env.close(0);
    } catch (DbException& e) 
    {
        std::cerr << "DbException: " << e.what() << std::endl;
        return false;
    } catch (std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

// Seeds

void DynosDB::flushSeeds()
{
    char ccbuffer[DYNOS_STANDARD_BUFFER];
    if(!getLocalConfigData("install_location", ccbuffer))
    {
        std::cout << "DynosDB::addVerifiedBlock => Unable to load install location!";
        return;
    }
    u_int32_t dflags = 0;
    DbEnv env(dflags);
    Db* pdb;
    try 
    {
        env.set_error_stream(&std::cerr);
        env.open(ccbuffer, DB_CREATE | DB_INIT_MPOOL, 0);
        pdb = new Db(&env, 0);
        pdb->open(NULL, "seeds.db", NULL, DB_BTREE, DB_CREATE | DB_TRUNCATE, 0);
        
        if(pdb != NULL)
        {
            pdb->close(0);
            delete pdb;
        }
        env.close(0);
    } catch (DbException& e) 
    {
        std::cerr << "DbException: " << e.what() << std::endl;
        return;
    } catch (std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return;
    }
    return;
}

bool DynosDB::addSeed(std::string number, std::string seedData)
{
    char ccbuffer[DYNOS_STANDARD_BUFFER];

    if(!getLocalConfigData("install_location", ccbuffer))
    {
        std::cout << "DynosDB::addVerifiedBlock => Unable to load install location!";
        return false;
    }

    u_int32_t dflags = 0;
    DbEnv env(dflags);
    Db* pdb;
    try 
    {
        env.set_error_stream(&std::cerr);
        env.open(ccbuffer, DB_CREATE | DB_INIT_MPOOL, 0);
        pdb = new Db(&env, 0);
        pdb->open(NULL, "seeds.db", NULL, DB_BTREE, DB_CREATE, 0);
        putd(pdb, number, seedData);
        
        if(pdb != NULL)
        {
            pdb->close(0);
            delete pdb;
        }
        env.close(0);
    } catch (DbException& e) 
    {
        std::cerr << "DbException: " << e.what() << std::endl;
        return false;
    } catch (std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

std::vector<std::string> DynosDB::readSeeds()
{
    std::vector<std::string> seeds;
    char ccbuffer[DYNOS_STANDARD_BUFFER];
    if(!getLocalConfigData("install_location", ccbuffer))
    {
        std::cout << "DynosDB::addVerifiedBlock => Unable to load install location!";
        return seeds;
    }
    
    u_int32_t dflags = 0;
    DbEnv env(dflags);
    Db* pdb;
    bool found = true;
    try 
    {
        env.set_error_stream(&std::cerr);
        env.open(ccbuffer, DB_CREATE | DB_INIT_MPOOL, 0);
        pdb = new Db(&env, 0);
        pdb->open(NULL, "seeds.db", NULL, DB_BTREE, 0, 0);
        
        int index = 0;
        while(found)
        {
            char *buffer = new char[4096];
            std::string key = std::to_string(index++);
            if (!sget(pdb, key, buffer, 4096)) 
                found = false;
            else
                seeds.push_back(buffer);
            delete []buffer;
        }
        
        if(pdb != NULL)
        {
            pdb->close(0);
            delete pdb;
        }
        env.close(0);
    } catch (DbException& e) 
    {
        std::cerr << "DbException: " << e.what() << std::endl;
        return seeds;
    } catch (std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return seeds;
    }
    return seeds;
}

}