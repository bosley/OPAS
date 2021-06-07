#ifndef BSLAI_NNET_CELL_HPP
#define BSLAI_NNET_CELL_HPP

#include "NnetTypes.hpp"

#include <vector>

namespace NNET 
{
    //!
    //! \class Cell
    //! \brief A single cell
    //!
    class Cell {
    public:

        static constexpr double DEFAULT_ETA   = 0.15; //! Default ETA value
        static constexpr double DEFAULT_ALPHA = 0.50; //! Default ALPHA value
        static constexpr double EULER         = 2.71828182845904523536; //! Eulers number (e)

        //!
        //! \class Variant
        //! \brief Possible variants a cell could be
        //!
        enum class Variant {
            NONE,
            INPUT,
            HIDDEN,
            RECURRENT,
            MEMORY,
            GATED_MEMORY,
            OUTPUT
        };

        //!
        //! \brief Construct a cell
        //! \param id The cell's id
        //! \param layer The layer that the cell resides in
        //! \param variant The cell variant
        //! \param transfer Transfer function to use
        //! \param eta Overall net training weighr (Defaults to DEFAULT_ETA)
        //! \param alpha Multiplier for last weight change i.e Momentum (Defaults to DEFAULT_ALPHA)
        //!
        Cell(unsigned id, unsigned layer, Variant variant, TransferFunction transfer=TransferFunction::TANH, double eta=DEFAULT_ETA, double alpha=DEFAULT_ALPHA);

        //!
        //! \brief Construct a cell from json
        //! \param cellJson the json representation of the cell
        //! \post The cell will be as the description dictates
        //!
        Cell(json cellJson);

        //!
        //! \brief Construct an empty
        //!
        Cell();

        //!
        //! \brief Get cell id
        //! \returns cell's id
        //!
        unsigned getId() const;

        //!
        //! \brief Set the output value of the cell
        //! \param value The value to set for the cell output
        //!
        void setOutputValue(double value);

        //!
        //! \brief Get cell's output value
        //! \returns value of the cell
        //!
        double getOutputValue() const;

        //!
        //! \brief Get cell's previous output value
        //! \returns value of the cell
        //!
        double getPrevOutputValue() const;

        //!
        //! \brief Set cell's gradient
        //! \param grad New gradient
        //!
        void setGradient(double grad);

        //!
        //! \brief Get cell's output value
        //! \returns Gradient of the cell
        //!
        double getGradient() const;

        //!
        //! \brief Get cell's inbound connections
        //!
        std::vector<Cell*> getInbound() const;

        //!
        //! \brief Get cell's outbound connections
        //!
        std::vector<Cell*> getOutbound() const;

        //!
        //! \brief Calculate the output gradients
        //! \param target Target value
        //!
        void calculateOutputGradients(double target);

        //!
        //! \brief Calculate hidden gradients
        //!
        void calculateHiddenGradients();

        //!
        //! \brief Update neuron input weights
        //!
        void updateInputWeights();

        //!
        //! \brief Get the cell's output weights information
        //! \param cellId The id of the cell to retrieve
        //!
        Connection getOuputWeightByCellId(unsigned cellId) const;

        //!
        //! \brief Set the weights of a conenction given a cell id
        //! \param cellId The id to update
        //! \param conn The connection weights to update to
        //! \post The cell's output wieght to the given cellId will be updated
        //!
        void setOutputWeightByCellId(unsigned cellId, Connection conn);

        //!
        //! \brief Set the connection information
        //! \param cellId The id of the connection to set
        //! \param connection The connection weights
        //!
        void setConnectionInformation(unsigned cellId, Connection connection);

        //!
        //! \brief Feed data forward
        //!
        void feedForward();


        friend class Network;
        friend bool  operator == (const Cell &lhs, const Cell &rhs);
        friend bool  doCellsMatch(const Cell &lhs, const Cell &rhs);
        friend void  to_json(json& j, const Cell& c);
        friend void  from_json(const json& j, Cell& c);

    private:
        unsigned id;
        Variant variant;
        double eta;
        double alpha;
        double outputVal;
        double prevOutput;
        double gradient;
        TransferFunction selectedTransferFunc;
        unsigned layer;

        std::vector<Cell*> inbound;
        std::vector<Cell*> outbound;

        std::map<unsigned, Connection> outputWeights;

        void addOutboundCell(Cell* cell, Connection connection);
        void addInboundCell(Cell *cell);

        double transferFunction(double x,  TransferFunction func);
        double transferDerivative(double x,TransferFunction func);
        double sumDOW();

        void buildFromJson(const json& j);
    };

    // Find cell by id - used in cell operations to find a cell, can't check other values
    // lest we end up with wrong behaviour in the network
    inline bool operator== (const Cell &lhs, const Cell &rhs) {
        return (
            lhs.id == rhs.id &&
            lhs.variant == rhs.variant
        );
    }

    inline bool doCellsMatch(const Cell &lhs, const Cell &rhs) {
        return (
            lhs.id == rhs.id &&
            lhs.gradient == rhs.gradient &&
            lhs.outputWeights == rhs.outputWeights &&
            lhs.eta == rhs.eta &&
            lhs.alpha == rhs.alpha &&
            lhs.variant == rhs.variant &&
            lhs.prevOutput == rhs.prevOutput
        );
    }

    inline void to_json(json& jsonCell, const Cell& c)
    {
        jsonCell["eta"]       = c.eta;
        jsonCell["alpha"]     = c.alpha;
        jsonCell["outputVal"] = c.outputVal;
        jsonCell["gradient"]  = c.gradient;
        jsonCell["id"]        = c.id;
        jsonCell["variant"]   = static_cast<int>(c.variant);
        jsonCell["transferFunc"] = static_cast<int>(c.selectedTransferFunc);
        jsonCell["outputWeights"] = c.outputWeights;
        jsonCell["layer"] = c.layer;
        jsonCell["prevOutput"] = c.prevOutput;
    }

    inline void from_json(const json& j, Cell& c) 
    {
        int stf = 0;
        int var = 0;
        j.at("eta").get_to(c.eta);
        j.at("alpha").get_to(c.alpha);
        j.at("outputVal").get_to(c.outputVal);
        j.at("gradient").get_to(c.gradient);
        j.at("id").get_to(c.id);
        j.at("transferFunc").get_to(stf);
        j.at("variant").get_to(var);
        j.at("outputWeights").get_to(c.outputWeights);
        j.at("layer").get_to(c.layer);
        j.at("prevOutput").get_to(c.prevOutput);
        c.selectedTransferFunc = static_cast<TransferFunction>(stf);
        c.variant = static_cast<Cell::Variant>(var);
    }
}

#endif