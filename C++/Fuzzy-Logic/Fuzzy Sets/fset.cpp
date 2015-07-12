#include "fset.h"

int fset::size()
{
    return setData.size();
}

bool fset::isEmpty()
{
    return setData.empty();
}

void fset::erase( DATA_TYPE k)
{
    setData.erase(k);
}

void fset::clear()
{
    setData.clear();
}

void fset::insert( DATA_TYPE k, bool probability)
{
    setData.insert( pair< char*,fbool>(k,probability) );
}

void fset::insert( DATA_TYPE k, double probability)
{
    setData.insert( pair< char*,fbool>(k,probability) );
}

/*

   Operators

*/

fset fset::operator&&(fset x)
{
    fset  temp;
    fbool temp2;

    map<DATA_TYPE,fbool>::iterator i;
    map<DATA_TYPE,fbool>::iterator j;

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


fset fset::operator||(fset x)
{
    fset temp;
    map<DATA_TYPE,fbool>::iterator i;
    map<DATA_TYPE,fbool>::iterator j;

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


fset fset::operator!()
{
    fset temp;
    map<DATA_TYPE,fbool>::iterator i;

    for (i = this->setData.begin(); i != setData.end(); ++i)
    {
        temp.insert(i->first,(!(i->second)).get());
    }
    return temp;
}


fbool& fset::operator[](DATA_TYPE k)
{
    return setData[k];
}









