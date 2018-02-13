#ifndef FSET_H
#define FSET_H

#include <map>
#include <string>
#include "../FuzzyBoolean/fbool.h"


namespace bosley
{

typedef struct fset
{
    // Data
    std::map<std::string,fbool> setData;

    // Methods
    fset(){}
    int size();
    bool isEmpty();
    void erase(std::string);
    void clear();
    void insert(std::string, bool);
    void insert(std::string, double);

    // Operators
    fset operator&&(fset);
    fset operator||(fset);
    fset operator!();
    fbool& operator[](std::string);

}fset;
}
#endif // FSET_H
