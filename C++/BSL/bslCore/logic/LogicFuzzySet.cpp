#include "LogicFuzzySet.hpp"

namespace LOGIC
{
    // -----------------------------------------------------------
    // FuzzySet
    // -----------------------------------------------------------
    FuzzySet::FuzzySet() {
    }

    // -----------------------------------------------------------
    // size
    // -----------------------------------------------------------
    int FuzzySet::size() {

        return setData.size();
    }

    // -----------------------------------------------------------
    // isEmpty
    // -----------------------------------------------------------
    bool FuzzySet::isEmpty() {

        return setData.empty();
    }

    // -----------------------------------------------------------
    // erase
    // -----------------------------------------------------------
    void FuzzySet::erase( std::string k) {

        setData.erase(k);
    }

    // -----------------------------------------------------------
    // clear
    // -----------------------------------------------------------
    void FuzzySet::clear() {

        setData.clear();
    }

    // -----------------------------------------------------------
    // insert
    // -----------------------------------------------------------
    void FuzzySet::insert( std::string k, bool probability) {

        setData.insert( std::pair<std::string, FuzzyBool>(k,probability) );
    }

    // -----------------------------------------------------------
    // insert
    // -----------------------------------------------------------
    void FuzzySet::insert( std::string k, double probability) {

        setData.insert( std::pair<std::string, FuzzyBool>(k,probability) );
    }

    // -----------------------------------------------------------
    // operator&&
    // -----------------------------------------------------------
    FuzzySet FuzzySet::operator&&(FuzzySet x) {

        FuzzySet  temp;
        FuzzyBool temp2;

        std::map<std::string, FuzzyBool>::iterator i;
        std::map<std::string, FuzzyBool>::iterator j;

        for( i = setData.begin(), j = x.setData.begin(); \
            i != setData.end() && j != x.setData.end(); )
        {
            if( i->first < j->first )
            {
                temp2.set(false);
                temp.insert(i->first, temp2.get());
                ++i;
            }
            else if ( i->first > j->first )
            {
                temp2.set(false);
                temp.insert(j->first,temp2.get());
                ++j;
            }
            else
            {
                temp.insert(j->first,((i->second)&&(j->second)).get());
                ++i;
                ++j;
            }
        }
        return temp;
    }

    // -----------------------------------------------------------
    // operator||
    // -----------------------------------------------------------
    FuzzySet FuzzySet::operator||(FuzzySet x) {

        FuzzySet temp;
        std::map<std::string, FuzzyBool>::iterator i;
        std::map<std::string, FuzzyBool>::iterator j;

        for( i = setData.begin(), j = x.setData.begin(); \
            i != setData.end() && j != x.setData.end(); )
        {
            if (i->first < j->first)
            {
                temp.insert(i->first,(i->second).get());
                ++i;
            }
            else if (i->first > j->first)
            {
                temp.insert(j->first,(j->second).get());
                ++j;
            }
            else
            {
                temp.insert(j->first,((i->second)||(j->second)).get());
                ++i;
                ++j;
            }
        }
        return temp;
    }

    // -----------------------------------------------------------
    // operator!
    // -----------------------------------------------------------
    FuzzySet FuzzySet::operator!() {

        FuzzySet temp;
        std::map<std::string, FuzzyBool>::iterator i;

        for (i = this->setData.begin(); i != setData.end(); ++i)
        {
            temp.insert(i->first,(!(i->second)).get());
        }
        return temp;
    }

    // -----------------------------------------------------------
    // operator[]
    // -----------------------------------------------------------
    FuzzyBool& FuzzySet::operator[](std::string k) {
        return setData[k];
    }


}






