#ifndef IPF_SERIES
#define IPF_SERIES

#include "ProcessableIF.h"
#include "StepperIF.h"

#include <list>

namespace IPF
{
    typedef std::list<ProcessableIF *>::iterator PIT;

    class Series : public StepperIF
    {
    public:

        Series();
        ~Series();

        //
        //  Add a processable step
        //
        void addStep(ProcessableIF * step);

        //
        //  Process the next step
        //
        bool process();

        //
        //  From StepperIF
        //
        void prev();
        void replay();

    private:
        std::list<ProcessableIF *> _steps;
        unsigned _stepCounter;
        bool _stepBlock;
    };
}

#endif
