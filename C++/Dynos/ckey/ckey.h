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
 */

#ifndef CKEY_HEADER_GUARD
#define CKEY_HEADER_GUARD

#ifdef _WIN32
    #define DYNOS_NIL_FILE "nul"
#else
    #define DYNOS_NIL_FILE "/dev/null"
#endif

#define KEY_CONTEXT "key_man"

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <cryptopp/sha.h>
#include <cryptopp/rsa.h>
#include <cryptopp/pssr.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/whrlpool.h>

#define KEYMAN_KEYSIZE 4096

namespace keyman
{

    
typedef CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::SHA256>::Signer csigner;
typedef CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::SHA256>::Verifier cverifier;
    
class ckey
{
public:
    ckey(std::string datastore="./");
    bool cnewKey(std::string title, bool replace=false);
    std::string csignMessage(std::string message, std::string keyname);
    std::string cgetPublicKey(std::string keyname);
    std::string cgetPrivateKey(std::string keyname);
    bool cverifySig(std::string signature, std::string message, std::string public_key);
    
private:
    boost::filesystem::path _config;
    bool fwrite(boost::filesystem::path path, char *data);
    bool fread(boost::filesystem::path path, char *data);
    bool cfwrite(boost::filesystem::path paf, std::string data);
    std::string cfread(boost::filesystem::path paf);
};

}

#endif