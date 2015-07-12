#include "fbool.h"
#include <ctime>
#include <cstdlib>

#define FUZZY_TRUE  1.0
#define FUZZY_FALSE 0.0

bool fbool::randomized = 0;

/*

    Constructors

*/

fbool::fbool()
{
    if(!randomized)
    {
        srand(time(0));
        randomized = 1;
    }

    probability = FUZZY_FALSE;
}

fbool::fbool(bool x)
{
    if(!randomized)
    {
        srand(time(0));
        randomized = 1;
    }

    (x) ? probability = FUZZY_TRUE : probability = FUZZY_FALSE;
}

fbool::fbool(double x)
{
    if(!randomized)
    {
        srand(time(0));
        randomized = 1;
    }

    if ( x > FUZZY_TRUE )
    {
        probability = FUZZY_TRUE;
    }
    else if ( x < FUZZY_FALSE )
    {
        probability = FUZZY_FALSE;
    }
    else
    {
        probability = x;
    }
}

/*

        Set / Get / toBool

*/

void fbool::set(bool x)
{
    (x) ? probability = FUZZY_TRUE : probability = FUZZY_FALSE;
}

void fbool::set(double x)
{
    if ( x > FUZZY_TRUE )
    {
        probability = FUZZY_TRUE;
    }
    else if ( x < FUZZY_FALSE )
    {
        probability = FUZZY_FALSE;
    }
    else
    {
        probability = x;
    }
}

// Probrably not needed, but people tend to like getters
double fbool::get() const
{
    return probability;
}

bool fbool::toBool() const
{
    return double(rand())/RAND_MAX <= probability;
}

/*

    Operator Overloads

*/

fbool fbool::operator&&(fbool x)
{
    if (probability < x.probability)
    {
        return fbool(probability);
    }
    else
    {
        return fbool(x.probability);
    }
}

fbool fbool::operator&&(bool x)
{
    if(x)
    {
        return fbool(probability);
    }
    else
    {
        return fbool(false);
    }
}

fbool fbool::operator||(fbool x)
{
    if (probability > x.probability)
    {
        return fbool(probability);
    }
    else
    {
        return fbool(x.probability);
    }
}

fbool fbool::operator||(bool x)
{
    if(x)
    {
        return fbool(true);
    }
    else
    {
        return fbool(probability);
    }
}

fbool fbool::operator!()
{
    return fbool(FUZZY_TRUE - probability);
}

fbool operator&&(bool x1, fbool x2)
{
    return(x2 && x1);
}

fbool operator||(bool x1, fbool x2)
{
    return(x2 || x1);
}





