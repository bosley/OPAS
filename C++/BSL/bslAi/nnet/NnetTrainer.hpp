#ifndef BSLAI_NNET_TRAINER_HPP
#define BSLAI_NNET_TRAINER_HPP

#include "NnetNetwork.hpp"
#include "NnetTrainingData.hpp"

#include "bslcore/UtilLogger.hpp"
#include "bslcore/UtilTimer.hpp"

namespace NNET
{
    //!
    //! \class Trainer
    //! \brief Train a network
    //!
    class Trainer {
    public:
        //!
        //! \brief Construct a Trainer
        //! \param logger The logger to use during Training process
        //! \param maxTrainTimeSec The maximum amount of time to allow training
        //!
        Trainer(UTIL::Logger &logger, double maxTrainTimeSec=600);

        //!
        //! \brief Destroy Trainer
        //!
        ~Trainer();

        //!
        //! \brief Train the network
        //! \param network The network to train
        //! \param trainingdata The json data to use for training
        //! \param iterations Number of times to run the training data through the network 
        //! \param epochs Number of times per iteration the given data will go through the network
        //!
        void trainByItr(Network &network, json trainingdata, uint64_t iterations, uint64_t epochs);

        //!
        //! \brief Train the network until each input error differs by no more than an allowedDelta
        //! \param network The network to train
        //! \param trainingdata The json data to use for training
        //! \param allowedDelta Maximum delta allowed for any case in trainingdata 
        //! \param epoch The number of epochs per error check
        //! \post Once the method returns, the network will be trained to handle all cases in trainingdata
        //!       with an error of no  more than the allowedDelta
        //!
        void trainToComplete(Network &network, json trainingdata, double allowedDelta, unsigned epoch);

    private:
        UTIL::Logger &logger;
        TrainingData *data;
        UTIL::Timer  *timer;
    };
}

#endif