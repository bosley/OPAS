#ifndef IPF_PROCESSABLE_IF
#define IPF_PROCESSABLE_IF

namespace IPF
{
    // FWD
    class StepperIF;

    //!
    //! \class ProcessableIF
    //!        An interface to make a 'step' in processing an image
    //!        within a process series
    //!
    class ProcessableIF
    {
    public:

        //!
        //! \brief Callback to process stepper to change next step
        //!
        virtual void defineSeriesCallback(StepperIF *) = 0;

        //!
        //! \brief Perform processing specific to this class
        //!
        virtual void process() = 0;
    };
} // IPF

#endif
