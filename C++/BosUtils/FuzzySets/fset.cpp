#include "fset.h"

namespace bosley
{

int fset::size()
{
    return setData.size();
}

bool fset::isEmpty()
{
    return setData.empty();
}

void fset::erase( std::string k)
{
    setData.erase(k);
}

void fset::clear()
{
    setData.clear();
}

void fset::insert( std::string k, bool probability)
{
    setData.insert( std::pair<std::string,fbool>(k,probability) );
}

void fset::insert( std::string k, double probability)
{
    setData.insert( std::pair<std::string,fbool>(k,probability) );
}

/*

   Operators

*/

fset fset::operator&&(fset x)
{
    fset  temp;
    fbool temp2;

    std::map<std::string,fbool>::iterator i;
    std::map<std::string,fbool>::iterator j;

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
    std::map<std::string,fbool>::iterator i;
    std::map<std::string,fbool>::iterator j;

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
    std::map<std::string,fbool>::iterator i;

    for (i = this->setData.begin(); i != setData.end(); ++i)
    {
        temp.insert(i->first,(!(i->second)).get());
    }
    return temp;
}


fbool& fset::operator[](std::string k)
{
    return setData[k];
}


}






