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

bool ckey::fwrite(boost::filesystem::path paf, char *data)
{
    boost::filesystem::ofstream of;
    of.open(paf, std::fstream::out);
    if(!of.is_open())
        return false;
    of << data;
    of.close();
    return true;
}

bool ckey::fread(boost::filesystem::path paf, char *data)
{
    boost::filesystem::ifstream ip;
    ip.open(paf , std::ios::in | std::ios::binary );

    if(!ip.is_open())
        return false;

    ip.read(data, hydro_sign_PUBLICKEYBYTES);
    ip.close();
    return true;
}

bool ckey::newKey(std::string title, bool replace)
{
    hydro_sign_keypair key_pair;
    hydro_sign_keygen(&key_pair);
    std::string pubfile = title + ".pub";
    std::string privfile = title + ".priv";
    boost::filesystem::path pubf = _config;
    boost::filesystem::path privf = _config;
    pubf /= pubfile;
    privf /= privfile;
    if(boost::filesystem::is_regular_file(pubf) && !replace)
        return false;
    if(boost::filesystem::is_regular_file(privf) && !replace)
        return false;
    if(!fwrite(pubf, (char*)key_pair.pk))
        return false;
    if(!fwrite(privf, (char*)key_pair.sk))
        return false;
    return true;
}

bool ckey::signMessage(uint8_t *signature, std::string message, std::string keyname)
{
    std::string pubfile = keyname + ".pub";
    std::string privfile = keyname + ".priv";
    boost::filesystem::path pubf = _config;
    boost::filesystem::path privf = _config;
    pubf /= pubfile;
    privf /= privfile;
    if(!boost::filesystem::is_regular_file(pubf))
        return false;
    if(!boost::filesystem::is_regular_file(privf))
        return false;
    char pub_key[hydro_sign_PUBLICKEYBYTES];
    char priv_key[hydro_sign_PUBLICKEYBYTES];
    if(!fread(pubf, pub_key))
        return false;
    if(!fread(privf, priv_key))
        return false;

    hydro_sign_create(signature, 
        message.c_str(), sizeof(message.c_str()), 
        KEY_CONTEXT, (uint8_t *)priv_key);
    return true;
}

bool ckey::verifySig(uint8_t *signature, std::string message, uint8_t * public_key)
{
    if (hydro_sign_verify(signature, 
        message.c_str(), sizeof(message.c_str()), 
        KEY_CONTEXT, public_key) != 0) {
        return false;
    }
    return true;
}

bool ckey::verifySig(uint8_t *signature, std::string message, std::string public_key)
{
    if (hydro_sign_verify(signature, 
        message.c_str(), sizeof(message.c_str()), 
        KEY_CONTEXT, (uint8_t *)public_key.c_str()) != 0) {
        return false;
    }
    return true;
}

bool ckey::getPublicKey(uint8_t *keydata, std::string keyname)
{
    std::string pubfile = keyname + ".pub";
    boost::filesystem::path pubf = _config;
    pubf /= pubfile;
    if(!boost::filesystem::is_regular_file(pubf))
        return false;
    if(!fread(pubf, (char*)keydata))
        return false;
    return true;
}

bool ckey::getPrivateKey(uint8_t *keydata, std::string keyname)
{
    std::string privfile = keyname + ".priv";
    boost::filesystem::path privf = _config;
    privf /= privfile;
    if(!boost::filesystem::is_regular_file(privf))
        return false;
    if(!fread(privf, (char*)keydata))
        return false;
    return true;
}

}