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

#include "ckey.h"

namespace keyman
{

ckey::ckey(std::string datastore)
{
    boost::filesystem::path p(datastore);

    if(boost::filesystem::is_regular_file(p))
    {
        _config = p.parent_path();
        _config /= "keyman";
        boost::filesystem::create_directory(_config);
    }
    
    if(boost::filesystem::is_directory(p))
    {
        _config = p;
    } else {
        boost::filesystem::create_directory("./keyman/");
        _config = boost::filesystem::path("./keyman/");
    }
}

bool ckey::cfwrite(boost::filesystem::path paf, std::string data)
{
    boost::filesystem::ofstream of;
    of.open(paf, std::fstream::out);
    if(!of.is_open())
        return false;
    of << data;
    of.close();
    return true;
}

std::string ckey::cfread(boost::filesystem::path paf)
{
    boost::filesystem::ifstream ip;
    ip.open(paf , std::ios::in);

    if(!ip.is_open())
        return std::string();

    
    std::stringstream buffer;
    buffer << ip.rdbuf();
    ip.close();
    
    return buffer.str();
}

bool ckey::cnewKey(std::string title, bool replace)
{
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::RSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, KEYMAN_KEYSIZE);
    CryptoPP::RSA::PublicKey publicKey(privateKey);

    std::string pubfile = title + ".pub";
    std::string privfile = title + ".priv";
    boost::filesystem::path pubf = _config;
    boost::filesystem::path privf = _config;
    pubf /= pubfile;
    privf /= privfile;
   
    std::string pub, priv;
    publicKey.Save( CryptoPP::HexEncoder(
                      new CryptoPP::StringSink(pub)).Ref());
    privateKey.Save(CryptoPP::HexEncoder(
                      new CryptoPP::StringSink(priv)).Ref());

    if(!cfwrite(pubf, pub))
        return false;
    if(!cfwrite(privf, priv))
        return false;
    return true;
}

std::string ckey::csignMessage(std::string message, std::string keyname)
{
    std::string privfile = keyname + ".priv";
    boost::filesystem::path privf = _config;
    privf /= privfile;
    if(!boost::filesystem::is_regular_file(privf))
        return std::string();
    std::string privkey = cfread(privf);
    if(privkey.length() == 0)
        return privkey;
  
    CryptoPP::RSA::PrivateKey privateKey;
    privateKey.Load(CryptoPP::StringSource(privkey, true,
                                           new CryptoPP::HexDecoder()).Ref());
    std::string signature;
    csigner signer(privateKey);
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::StringSource ss(message, true,
                              new CryptoPP::SignerFilter(rng, signer,
                                new CryptoPP::HexEncoder(
                                  new CryptoPP::StringSink(signature))));
    return signature;
}

std::string ckey::cgetPublicKey(std::string keyname)
{
     std::string pubfile = keyname + ".pub";
    boost::filesystem::path pubf = _config;
    pubf /= pubfile;
    if(!boost::filesystem::is_regular_file(pubf))
        return std::string();
    
    std::string pubkey = cfread(pubf);
    return pubkey;
}

std::string ckey::cgetPrivateKey(std::string keyname)
{
    std::string privfile = keyname + ".priv";
    boost::filesystem::path privf = _config;
    privf /= privfile;
    if(!boost::filesystem::is_regular_file(privf))
        return std::string();
    std::string privkey = cfread(privf);
    return privkey;
}

bool ckey::cverifySig(std::string sig, std::string message, std::string public_key)
{
    CryptoPP::RSA::PublicKey publicKey;
    publicKey.Load(CryptoPP::StringSource(public_key, true,
                                          new CryptoPP::HexDecoder()).Ref());

    // decode signature
    std::string decodedSignature;
    CryptoPP::StringSource ss(sig, true,
                              new CryptoPP::HexDecoder(
                                new CryptoPP::StringSink(decodedSignature)));

    bool result = false;
    cverifier verifier(publicKey);
    CryptoPP::StringSource ss2(decodedSignature + message, true,
                               new CryptoPP::SignatureVerificationFilter(verifier,
                                 new CryptoPP::ArraySink((CryptoPP::byte*)&result,
                                                         sizeof(result))));
    return result;
}


}