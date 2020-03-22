#include "UTILFuzzyBool.hpp"

#include <ctime>
#include <cstdlib>

namespace BUTANE
{
    namespace UTIL
    {
        bool FuzzyBool::randomized = 0;

        // -----------------------------------------------------------
        // FuzzyBool()
        // -----------------------------------------------------------

        FuzzyBool::FuzzyBool()
        {
            if(!randomized)
            {
                srand(time(0));
                randomized = 1;
            }

            probability = FUZZY_FALSE;
        }

        // -----------------------------------------------------------
        // FuzzyBool(bool x)
        // -----------------------------------------------------------
        
        FuzzyBool::FuzzyBool(bool x)
        {
            if(!randomized)
            {
                srand(time(0));
                randomized = 1;
            }

            (x) ? probability = FUZZY_TRUE : probability = FUZZY_FALSE;
        }

        // -----------------------------------------------------------
        // FuzzyBool(double x)
        // -----------------------------------------------------------
        
        FuzzyBool::FuzzyBool(double x)
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

        // -----------------------------------------------------------
        // set
        // -----------------------------------------------------------
        
        void FuzzyBool::set(bool x)
        {
            (x) ? probability = FUZZY_TRUE : probability = FUZZY_FALSE;
        }

        // -----------------------------------------------------------
        // set
        // -----------------------------------------------------------
        
        void FuzzyBool::set(double x)
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

        // -----------------------------------------------------------
        // get
        // -----------------------------------------------------------
        
        double FuzzyBool::get() const
        {
            return probability;
        }

        // -----------------------------------------------------------
        // toBool
        // -----------------------------------------------------------
        
        bool FuzzyBool::toBool() const
        {
            return double(rand())/RAND_MAX <= probability;
        }

        // -----------------------------------------------------------
        // operator&&
        // -----------------------------------------------------------
        
        FuzzyBool FuzzyBool::operator&&(FuzzyBool x)
        {
            if (probability < x.probability)
            {
                return FuzzyBool(probability);
            }
            else
            {
                return FuzzyBool(x.probability);
            }
        }

        // -----------------------------------------------------------
        // operator&&
        // -----------------------------------------------------------
        
        FuzzyBool FuzzyBool::operator&&(bool x)
        {
            if(x)
            {
                return FuzzyBool(probability);
            }
            else
            {
                return FuzzyBool(false);
            }
        }

        // -----------------------------------------------------------
        // operator||
        // -----------------------------------------------------------
        
        FuzzyBool FuzzyBool::operator||(FuzzyBool x)
        {
            if (probability > x.probability)
            {
                return FuzzyBool(probability);
            }
            else
            {
                return FuzzyBool(x.probability);
            }
        }

        // -----------------------------------------------------------
        // operator||
        // -----------------------------------------------------------
        
        FuzzyBool FuzzyBool::operator||(bool x)
        {
            if(x)
            {
                return FuzzyBool(true);
            }
            else
            {
                return FuzzyBool(probability);
            }
        }

        // -----------------------------------------------------------
        // operator!
        // -----------------------------------------------------------
        
        FuzzyBool FuzzyBool::operator!()
        {
            return FuzzyBool(FUZZY_TRUE - probability);
        }

        // -----------------------------------------------------------
        // operator&&
        // -----------------------------------------------------------
        
        FuzzyBool operator&&(bool x1, FuzzyBool x2)
        {
            return(x2 && x1);
        }

        // -----------------------------------------------------------
        // operator||
        // -----------------------------------------------------------
        
        FuzzyBool operator||(bool x1, FuzzyBool x2)
        {
            return(x2 || x1);
        }
    }
}