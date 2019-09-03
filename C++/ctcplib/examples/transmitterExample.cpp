#include "../CTCPTransmitter.hpp"

#include <iostream>

int main()
{
    std::cout << "Transmitter test... " << std::endl;

    CTCP::Transmitter basicTransmitter("127.0.0.1", 8888);

    basicTransmitter.init();

    std::string testData(
        "This is a test of the transmitter system. This giant block of text should be"
        "broken up into however many chunks defined by CTCPCommon.h and then sent off"
        "to the server. If all goes well the whole thing will go through rather nicely"
        ". The issue here is that things are never nice. I will no doubt have to grab"
        "my hammer and beat this code into submission before I can shine it and stick"
        "it on the shelf. =]"
            );

    std::string testData0 = "Look mom, its working! Hahahahahah";

    if(!basicTransmitter.transmit(testData))
    {
        std::cout << "main::basicTransmitter.transmit() failed" << std::endl;
    }
    else
    {
        std::cout << "main::basicTransmitter.transmit() success" << std::endl;
    }

    return 0;
}
