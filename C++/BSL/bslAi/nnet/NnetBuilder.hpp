#ifndef BSLAI_NET_BUILDER_HPP
#define BSLAI_NET_BUILDER_HPP

#include "NnetTypes.hpp"
#include "NnetNetwork.hpp"

#include "bslcore/UtilLogger.hpp"

namespace NNET
{

    //!
    //! \class Builder
    //! \brief Build common networks
    //!
    class Builder {

    public:
        //!
        //! \brief Construct a builder object
        //! \param logger The logger to use with the network to be built
        //!
        Builder(UTIL::Logger &logger);

        //!
        //! \brief Construct a deep feed forward network
        //! \param inputNodes The number of input nodes for the network
        //! \param innerLayers The number of hidden layers for the network to have
        //! \param outputNides The number of output nodes for the network
        //! \param transferFunc Transfer function to be used for all cells on the network
        //!
        Network buildDeepFeedForward(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes, TransferFunction transferFunc);
    
        //!
        //! \brief Construct a recurrent network
        //! \param inputNodes The number of input nodes for the network
        //! \param innerLayers The number of hidden layers for the network to have
        //! \param outputNides The number of output nodes for the network
        //! \param transferFunc Transfer function to be used for all cells on the network
        //!
        Network buildRecurrent(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes, TransferFunction transferFunc);

        //!
        //! \brief Construct a LSTM network
        //! \param inputNodes The number of input nodes for the network
        //! \param innerLayers The number of hidden layers for the network to have
        //! \param outputNides The number of output nodes for the network
        //!
        Network buildLstm(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes);

        //!
        //! \brief Construct a Gated LSTM network
        //! \param inputNodes The number of input nodes for the network
        //! \param innerLayers The number of hidden layers for the network to have
        //! \param outputNides The number of output nodes for the network
        //!
        Network buildGatedLstm(unsigned inputNodes, unsigned innerLayers, unsigned outputNodes);

    private:
        enum class NetworkType {
            DFF,
            RECURRENT,
            LSTM,
            GATED_LSTM
        };

        UTIL::Logger &logger;

        Network fullyConnected(NetworkType type, unsigned inputNodes, unsigned innerLayers, unsigned outputNodes, TransferFunction transferFunc=TransferFunction::TANH);

    };
}

#endif