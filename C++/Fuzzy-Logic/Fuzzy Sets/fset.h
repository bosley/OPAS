#ifndef FSET_H
#define FSET_H

#include <map>
#include "fbool.h"

using namespace std;

typedef char* DATA_TYPE;

typedef struct fset
{
    // Data
    map<DATA_TYPE,fbool> setData;

    // Methods
    fset(){}
    int size();
    bool isEmpty();
    void erase(DATA_TYPE);
    void clear();
    void insert(DATA_TYPE, bool);
    void insert(DATA_TYPE, double);

    // Operators
    fset operator&&(fset);
    fset operator||(fset);
    fset operator!();
    fbool& operator[](DATA_TYPE);

}fset;

#endif // FSET_H
