#include "NnetCell.hpp"

#include <stdlib.h>
#include <time.h>  
#include<cmath>

#include <iostream>

namespace {

    inline double sigmoid(double x) {
        return (1.0 / (1.0 + std::pow(NNET::Cell::EULER, -x)));
    }

}

namespace NNET 
{
    // -------------------------------------------------
    // Cell
    // -------------------------------------------------
    Cell::Cell(unsigned id, unsigned layer, Variant variant, TransferFunction transfer, double eta, double alpha) : 
                                               id(id), 
                                               variant(variant),
                                               selectedTransferFunc(transfer),
                                               eta(eta),
                                               alpha(alpha),
                                               prevOutput(0.0),
                                               layer(layer) {
        
        srand (time(NULL));
    }

    // -------------------------------------------------
    // Cell
    // -------------------------------------------------
    Cell::Cell(json cellJson) {
        
        srand (time(NULL));

        buildFromJson(cellJson);
    }

    // -------------------------------------------------
    // Cell
    // -------------------------------------------------
    Cell::Cell() : 
                    id(0), 
                    variant(Cell::Variant::NONE),
                    selectedTransferFunc(TransferFunction::TANH),
                    eta(DEFAULT_ETA),
                    alpha(DEFAULT_ALPHA),
                    prevOutput(0.0),
                    layer(0) {
        srand (time(NULL));
    }

    // -------------------------------------------------
    // getId
    // -------------------------------------------------
    unsigned Cell::getId() const{
        return id;
    }

    // -------------------------------------------------
    // addInboundCell
    // -------------------------------------------------
    void Cell::addInboundCell(Cell *cell) {

        if(cell == nullptr) {
            std::__throw_runtime_error("Attempted to add nullptr as an inbound cell");
        }
        inbound.push_back(cell);
    }

    // -------------------------------------------------
    // addOutboundCell
    // -------------------------------------------------
    void Cell::addOutboundCell(Cell *cell, Connection connection) {

        if(cell == nullptr) {
            std::__throw_runtime_error("Attempted to add nullptr as an outbound cell");
        }
        outbound.push_back(cell);
        outputWeights[cell->getId()] = connection;
    }

    // -------------------------------------------------
    // setOutputValue
    // -------------------------------------------------
    void Cell::setOutputValue(double value){

        prevOutput = outputVal;
        outputVal = value;
    }

    // -------------------------------------------------
    // getOutputValue
    // -------------------------------------------------
    double Cell::getOutputValue() const{

        return outputVal;
    }

    // -------------------------------------------------
    // getPrevOutputValue
    // -------------------------------------------------
    double Cell::getPrevOutputValue() const{

        return prevOutput;
    }
    // -------------------------------------------------
    // setGradient
    // -------------------------------------------------
    void Cell::setGradient(double grad) {
        gradient = grad;
    }

    // -------------------------------------------------
    // getGradient
    // -------------------------------------------------
    double Cell::getGradient() const {
        return gradient;
    }

    // -------------------------------------------------
    // getInbound
    // -------------------------------------------------
    std::vector<Cell*> Cell::getInbound() const {

        return inbound;
    }

    // -------------------------------------------------
    // getOutbound
    // -------------------------------------------------
    std::vector<Cell*> Cell::getOutbound() const {

        return inbound;
    }

    // -------------------------------------------------
    // getConnectionInformation
    // -------------------------------------------------
    Connection Cell::getOuputWeightByCellId(unsigned cellId) const {


        for(auto &element : outputWeights) {

            if(element.first == cellId) {
                return element.second;
            }
        }

        std::string str = "Error finding cell for getOutputWeigtByCellId("  + std::to_string(cellId) + ") - Defaulting to new weight";
        std::__throw_runtime_error(str.c_str());

        return { 0.0, 0.0 };
    }

    // -------------------------------------------------
    // setConnectionInformation
    // -------------------------------------------------
    void Cell::setConnectionInformation(unsigned cellId, Connection connection) {

        for(auto &element : outputWeights) {

            if(element.first == cellId) {
                element.second = connection;
                return;
            }
        }

        std::string str = "Error finding cell for setConnectionInformation("  + std::to_string(cellId) + ")";
        std::__throw_runtime_error(str.c_str());
    }

    // -------------------------------------------------
    // calculateOutputGradients
    // -------------------------------------------------
    void Cell::calculateOutputGradients(double target){

        double delta = target - outputVal;
        gradient = delta * transferDerivative(outputVal, selectedTransferFunc);
    }

    // -------------------------------------------------
    // calculateHiddenGradients
    // -------------------------------------------------
    void Cell::calculateHiddenGradients(){

        double dow = sumDOW();

        if(variant == Variant::RECURRENT) {

            gradient = dow * transferDerivative(outputVal + prevOutput, selectedTransferFunc);

        } else if (variant == Variant::MEMORY) {

            double combine = outputVal + prevOutput;
            double gateAlpha = transferDerivative(combine, TransferFunction::SIGMOID);
            double gateBeta  = transferDerivative(combine, TransferFunction::TANH);
            double alphaMult = prevOutput * gateAlpha;
            double gamma = gateBeta * gateAlpha;
            double theta = alphaMult + gamma;
            double omega = transferDerivative(theta, TransferFunction::SIGMOID);
            double cappa = omega * gateBeta;

            gradient = dow * cappa;

        } else if (variant == Variant::GATED_MEMORY) {

            double combine = outputVal + prevOutput;
            double gateAlpha = transferDerivative(combine, TransferFunction::SIGMOID);
            double gateBeta  = transferDerivative(outputVal, TransferFunction::TANH);
            double alphaMult = gateAlpha * outputVal;
            double alphaCombine = alphaMult + prevOutput;
            double betaNeg = gateAlpha - 1;
            double beta = betaNeg * alphaCombine + alphaMult;
            double cappaMult = gateBeta * gateAlpha;
            double cappa = beta + cappaMult;

            gradient = dow * cappa;

        } else {

            gradient = dow * transferDerivative(outputVal, selectedTransferFunc);
        }
    }

    // -------------------------------------------------
    // updateInputWeights
    // -------------------------------------------------
    void Cell::updateInputWeights(){

        for(unsigned n = 0; n < inbound.size(); ++n) {

            Cell *cell = inbound[n];

            Connection cellConnectionInfo = cell->getOuputWeightByCellId(id);

            double oldDelta = cellConnectionInfo.deltaWeight;
            double newDelta = eta * cell->getOutputValue() * gradient + alpha * oldDelta;
            double newWeight = cellConnectionInfo.weight + newDelta;

            cell->setConnectionInformation(id, 
                { 
                    newWeight, 
                    newDelta 
                });
        }
    }
    
    // -------------------------------------------------
    // transferFunction
    // -------------------------------------------------
    double Cell::transferFunction(double x, TransferFunction func) {

        switch(func) {
            case TransferFunction::TANH:    return tanh(x);
            case TransferFunction::SIGMOID: return sin(x);
        };

        // Satisfy the compiler
        return tanh(x);
    }

    // -------------------------------------------------
    // transferDerivative
    // -------------------------------------------------
    double Cell::transferDerivative(double x, TransferFunction func) {

        switch(func) {
            case TransferFunction::TANH:    return (1.0 - x*x);
            case TransferFunction::SIGMOID: return sigmoid(x)*(1.0-sigmoid(x));
        };

        // Satisfy the compiler
        return (1.0 - x*x);
    }

    // -------------------------------------------------
    // sumDOW
    // -------------------------------------------------
    double Cell::sumDOW() {

        double sum = 0.0;

        for(int i = 0; i < outbound.size(); i++) 
        {
            
            unsigned cellId = outbound[i]->getId();

            sum += outputWeights[cellId].weight * outbound[i]->getGradient();
        }

        return sum;

    }

    // -------------------------------------------------
    // feedForward
    // -------------------------------------------------
    void Cell::feedForward() {

        // output = f(Sigma i sub-i * w sub-i)
        double sum = 0.0;

        // Sum previous layers output
        for(unsigned n = 0; n  < inbound.size(); ++n)
        {
            Connection cellConnectionInfo = inbound[n]->getOuputWeightByCellId(id);

            sum += inbound[n]->getOutputValue() * cellConnectionInfo.weight;
        }


        // Transfer function
        outputVal = transferFunction(sum, selectedTransferFunc);
    }

    void Cell::buildFromJson(const json& j) {
        
        int stf = 0;
        int var = 0;
        j.at("eta").get_to(this->eta);
        j.at("alpha").get_to(this->alpha);
        j.at("outputVal").get_to(this->outputVal);
        j.at("gradient").get_to(this->gradient);
        j.at("id").get_to(this->id);
        j.at("transferFunc").get_to(stf);
        j.at("variant").get_to(var);
        j.at("outputWeights").get_to(this->outputWeights);
        j.at("layer").get_to(this->layer);
        j.at("prevOutput").get_to(this->prevOutput);
        this->selectedTransferFunc = static_cast<TransferFunction>(stf);
        this->variant = static_cast<Variant>(var);
    }

}