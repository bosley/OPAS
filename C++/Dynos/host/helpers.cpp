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
 * File:   helpers.cpp
 * Author: Josh A. Bosley
 * 
 * Created on March 30, 2018, 5:30 PM
 */

#include "helpers.h"

namespace dhost {
    
std::string HexToString(const std::string hex)
{
    std::string retString;
    for(int i=0; i< hex.length(); i+=2)
        retString.push_back(
            (char) (int)strtol(
                hex.substr(i,2).c_str(), NULL, 16
        ));
    return retString;
}

std::string AsciiToHex(const std::string ascii, bool asmac)
{
    std::stringstream sstream;
    for(int i = 0 ; i < ascii.length() ; i++)
    {
        sstream << std::hex << (int)ascii[i];
        if( asmac && (i != ascii.length()-1))
            sstream <<  ":";
    }
    return sstream.str();
}

std::string SHA256(std::string data)
{
    std::string digest;
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource foo(data, true,
    new CryptoPP::HashFilter(hash,
      new CryptoPP::Base64Encoder (
         new CryptoPP::StringSink(digest))));
    digest.erase(std::remove(digest.begin(), digest.end(), '\n'), digest.end());
    return digest;
}

std::string GenAddress(std::string source)
{
    // For now we just hash the mac, will modify later
    // to make easier for humans <3
    return SHA256(source);
}


}