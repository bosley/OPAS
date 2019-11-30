


#include <iostream>

#include "ckey.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Create a key manager
    keyman::ckey kman("./test_dir");

    // Create a new key
    if(!kman.newKey("test_key"))
        std::cout << "Unable to make key!\n";

    std::string messageToSign = "A message to sign";

    /*
        Sign a message
    */
    uint8_t signature[hydro_sign_BYTES];
    if(!kman.signMessage(signature, messageToSign, "test_key"))
        std::cout << "Unable to sign!\n";
    else
        std::cout << "Signed! : " << signature << "\n";

    /*
        Get a public key that is stored
    */
    uint8_t pubkey[hydro_sign_BYTES];
    if(!kman.getPublicKey(pubkey, "test_key"))
        std::cout << "Unable to get key!\n";
    else
        std::cout << "Pubkey: " << pubkey << "\n";

    /*
        Get a private key that is stored
    */
    uint8_t privkey[hydro_sign_BYTES];
    if(!kman.getPrivateKey(privkey, "test_key"))
        std::cout << "Unable to get key!\n";
    else
        std::cout << "Privkey: " << privkey << "\n";

    /*
        Verify a signature with uint8_t
    */
    if(!kman.verifySig(signature, messageToSign, pubkey))
        std::cout << "Unable to verify signature based on uint8_t key!\n";
    else
        std::cout << "Signature verified with uint8_t!\n";

    return 0;
}