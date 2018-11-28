/*
    Step control interface to enable a processable to change 
    scheduled step behaviour
*/

#ifndef IPF_STEPPER_IF
#define IPF_STEPPER_IF

namespace IPF
{
    class StepperIF
    {
    public:
        virtual void prev()     = 0;    // Schedule next step to previous step in list
        virtual void replay()  = 0;     // Schedule next step to replay current step
    };
} // IPF
#endif