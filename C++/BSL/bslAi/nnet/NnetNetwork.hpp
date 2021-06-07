#ifndef BSLAI_NNET_NETWORK_HPP
#define BSLAI_NNET_NETWORK_HPP

#include "NnetTypes.hpp"
#include "NnetCell.hpp"
#include "NnetTrainingData.hpp"

#include "bslcore/UtilLogger.hpp"

#include <vector>

namespace NNET
{
    //!
    //! \class Network
    //! \brief Build a cell network
    //!
    class Network {

    public:
        //!
        //! \class CorrectionMethod
        //! \brief Used to detemine which correction method to run in training
        //!
        enum class CorrectionMethod {
            BACK_PROPAGATION
            // Once nnet completed, a key expansion would be to add new correction methods
        };

        //!
        //! \brief Construct a builder
        //! \param logger The logger to use during the build process
        //! \param bias Default bias to instantiate cells with
        //!
        Network(UTIL::Logger &logger, double bias = 0.5);

        //!
        //! \brief Construct a builder
        //! \param logger The logger to use during the build process
        //! \param config Network configuration
        //! \post The network will be constructed as-per the configuration description
        //!
        Network(UTIL::Logger &logger, json config);

        //!
        //! \brief Deconstruct network
        //!
        ~Network();

        //!
        //! \brief Create a new cell layer
        //! \post  A new layer will be created 
        //!
        void newLayer();

        //!
        //! \brief Create a new cell
        //! \param layer The layer to insert the cell into
        //! \param cellId The id to give to the cell (Network does not check for unique, but it must be)
        //! \param cellVariant The type of cell to create
        //! \param transferFunc The transfer function to use with the cell
        //! \post  If the layer exists, the cell will be created. If layer does not exist, an exception will be thrown
        //!
        void newCell(unsigned layer, unsigned cellId, Cell::Variant cellVariant, TransferFunction transferFunction = TransferFunction::TANH);

        //!
        //! \brief Connect two cells together 
        //! \param cellIdFrom Where the connection starts
        //! \param cellIdTo Where the connection goes to
        //!
        void connectCells(unsigned cellIdFrom, unsigned cellIdTo);

        //!
        //! \brief Indicate to network that construction is complete 
        //! \post  Blocks calls for new layers, cells, and connections
        //!
        void finalizeConstruction();

        //!
        //! \brief Train the neural network
        //! \pre   The network must have been marked as finalized before training can take place
        //! \param data The training data
        //! \param epochs Number of epochs to run
        //! \param correctionMethod The method of correction to use in training (defaults to BACK_PROPAGATION)
        //!
        void train(TrainingData data, unsigned epochs, CorrectionMethod correctionMethod=CorrectionMethod::BACK_PROPAGATION);

        //!
        //! \brief Input data into the neural network
        //! \pre   The network must have been marked as finalized before input is allowed
        //! \param inputVals The input data
        //!
        void input(const std::vector<double> &inputVals);

        //!
        //! \brief Get the results from an input
        //! \pre   The network must have been marked as finalized before results will come out
        //! \returns Vector of results of a size dictated by the constructed topology
        //!
        std::vector<double> getResults() const;

        //!
        //! \brief Build json representation of the network
        //! \retval json information representing the network
        //!
        json get_json();

        friend bool  operator == (const Network &lhs, const Network &rhs);

    private:
        UTIL::Logger &logger;
        std::vector< std::vector< Cell > > layers;
        std::vector< CellToCell > connectedCells;
        bool constructionFinalized;
        double bias;
        json networkJson;

        void feedForward(const std::vector<double> &input);
        void backPropagation(const std::vector<double> &expected);
        Cell * findCell(unsigned id);
    };

    inline bool operator== (const Network &lhs, const Network &rhs) {
        return (
            lhs.layers == rhs.layers &&
            lhs.constructionFinalized == rhs.constructionFinalized &&
            lhs.bias == rhs.bias &&
            lhs.connectedCells == rhs.connectedCells
        );
    }
}


#endif