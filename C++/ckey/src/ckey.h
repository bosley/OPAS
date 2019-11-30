#ifndef CKEY_HEADER_GUARD
#define CKEY_HEADER_GUARD
#define KEY_CONTEXT "key_man"

#include <hydrogen.h>

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace keyman
{

class ckey
{
public:
    ckey(std::string datastore="./");
    bool newKey(std::string title, bool replace=false);
    bool signMessage(uint8_t *signature, std::string message, std::string keyname);
    bool verifySig(uint8_t *signature, std::string message, uint8_t * public_key);
    bool verifySig(uint8_t *signature, std::string message, std::string public_key);
    bool getPublicKey(uint8_t *keydata, std::string keyname);
    bool getPrivateKey(uint8_t *keydata, std::string keyname);

private:
    boost::filesystem::path _config;
    bool fwrite(boost::filesystem::path path, char *data);
    bool fread(boost::filesystem::path path, char *data);
};

}

#endif