#include "Series.h"

#include <iostream>
#include <iterator>

namespace IPF
{

Series::Series() : _stepCounter(0), _stepBlock(false)
{

}

Series::~Series()
{

}

void Series::addStep(ProcessableIF *step)
{
    _steps.push_back(step);
}

bool Series::process()
{
    _stepBlock = false;
    if(_steps.size() > _stepCounter)
    {
        PIT it = _steps.begin();
        std::advance(it, _stepCounter);
        (*it)->process();

        if(!_stepBlock)
            _stepCounter++;
        return true;
    }
    else
    {
        _stepCounter = 0;
        return false;
    }
}

// -------------------------------- STEPPER IF

void Series::prev()
{
    _stepBlock = true;

    if(_stepCounter > 0)
        _stepCounter--;
}

void Series::replay()
{
    _stepBlock = true;
}

} // IPF