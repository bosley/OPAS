#include "../CTCPThreadedTcpServer.hpp"
#include "../CTCPReceiverIf.hpp"
#include <iostream>

// -------------------------------------------------------------
// BasicReceiver
// -------------------------------------------------------------
class BasicReceiver : public CTCP::ReceiverIf
{
    public:
        BasicReceiver(){}
        ~BasicReceiver(){}

        virtual void receive(CTCP::MessageData msg) override
        {
            std::cout << "BasicReceiver::receive()" << std::endl
                      << msg.ipFrom  << ":" << msg.portFrom << std::endl
                      << "Chunks: " << std::endl;
            for(int i = 0; i < msg.chunkData.size(); i++)
            {
                std::cout << "Chunk [" << i << "] : " << msg.chunkData[i] << std::endl;
            }
        }
};

// -------------------------------------------------------------
// main
// -------------------------------------------------------------
int main()
{
    std::cout << "ThreadedTcpServer starting..." << std::endl;
    CTCP::ThreadedTcpServer server(8888, 100);

    BasicReceiver receiver;
    server.registerMessageReceiver(&receiver);

    if(!server.serve())
    {
        std::cout << "main::server.serve() failed" << std::endl;
    }

    return 0;
}
