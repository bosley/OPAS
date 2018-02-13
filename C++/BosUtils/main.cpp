#include <iostream>

#include "DoublyLinkedList/jdll.h"
#include "FuzzyBoolean/fbool.h"
#include "FuzzySets/fset.h"
#include "Sorting/datasorter.h"

using namespace std;

void testDoublyLinkedList();
void testFuzzyBools();
void evaluateBool(double, bosley::fbool&);
void testFuzzySets();
void evaluateSet(std::string, bosley::fset);
void testSorting();

int main()
{
    cout << "Testing doubly linked list\n\n";
    testDoublyLinkedList();

    cout << "Testing fuzzy booleans\n\n";
    testFuzzyBools();

    cout << "Testing fuzzy sets\n\n";
    testFuzzySets();

    cout << "Testing the sorter\n\n";
    testSorting();

    return 0;
}

void testDoublyLinkedList()
{
    bosley::JDLL<int> jList;

    cout << "Adding Items" << endl;

    for(int i = 0; i < 10; i++)
        jList.pushBack((i * 5));

    cout << "Getting number of items\n\n";
    cout << "There are " << jList.size << " item(s)\n\n";
    cout << "Index of the number 10 = "
         << jList.retrieveIndex(10) << "\n\n";

    cout << "The items are : \n\n";
    jList.printItems();

    cout << "Removing data '25' \n\n";
    jList.removeItem(25);

    cout << "The items are : \n\n";
    jList.printItems();

    cout << "Deleteing item at index 4\n\n";
    jList.removeIndex(4);

    cout << "The items are : \n\n";
    jList.printItems();

    cout << "Retrieving item by the index 5 \n\n";
    cout << "Item is " << jList.retrieveDataByIndex(5) << "\n\n";
}

void testFuzzyBools()
{
    bosley::fbool test;
    evaluateBool(0.25,test);
    evaluateBool(0.75,test);
    evaluateBool(1.25,test);
    evaluateBool(-1.25,test);

    cout << "Testing fuzzy boolean operators \n\n";

    test.set(0.75);
    bosley::fbool test2(0.25);
    cout << "FBool 1 : " << test.get() << "\n";
    cout << "FBool 2 : " << test2.get() << "\n";
    cout << "testbool&&testBool2 = " << (test&&test2).get() << "\n\n";
    cout << "testbool||testBool2 = " << (test||test2).get() << "\n\n";
    cout << "testbool&&true = " << (test&&true).get() << "\n\n";
    cout << "testbool&&false = " << (test&&false).get() << "\n\n";
    cout << "testbool||true = " << (test||true).get() << "\n\n";
    cout << "testbool||false = " << (test||false).get() << "\n\n";
    cout << "!testbool = " << (!test).get() << "\n\n";
    cout << "!testbool2 = " << (!test2).get() << "\n\n";
}

void evaluateBool(double value, bosley::fbool& fuzzyBool)
{
    fuzzyBool.set(value);
    cout << "Testing value [" << value << "]\n" << fuzzyBool.get();
    int i = 1, j = 0, k = 0;
    for(;i<=100;i++)
    {
        k++;
        if(fuzzyBool.toBool())
            j++ ;
    }
    cout << j << " of " << k << " returned true \n\n";
}

void testFuzzySets()
{
    cout << "\n\t Testing set 1\n";

    bosley::fset testSet;
    testSet.insert("false", false);
    testSet.insert("true" , true);
    testSet.insert("third", 0.33);
    testSet.insert("half" , 0.5);
    evaluateSet("TestSet", testSet);

    cout << "\n\t Testing set 2\n";

    bosley::fset testSet2;
    testSet2.insert("false", false);
    testSet2.insert("true" , true);
    testSet2.insert("quarter", 0.2f);
    testSet2.insert("half" , 0.5);
    evaluateSet("TestSet2", testSet2);

    cout << "\n\t Creating, and setting operation sets \n\n";

    bosley::fset andset = testSet2&&testSet;
    bosley::fset orset  = testSet2||testSet;
    bosley::fset notset = !testSet;

    cout << "andset[\"false\"]\n" << andset["false"].get();
    cout << "orset[\"false\"]\n" << orset["false"].get();
    cout << "notset[\"false\"]\n" << notset["false"].get();
}

void evaluateSet(std::string n, bosley::fset s)
{
    cout << endl << n << ".isEmpty() " << s.isEmpty();
    cout << endl << n << ".size() " << s.size() << endl;
}

void testSorting()
{
    bosley::DataSorter ds;

    int loot[2000];
    int moot[9] = {1,2,3,4,5,6,7,8,9};

    for(int i = 0; i < 2000; i++)
        loot[i] = 2000 - i;

    cout << "Sorting small data set: " << endl;
    ds.sort(moot);

    cout << "Sorting large data set: " << endl;
    ds.sort(loot);

    cout << "Data sorted" << endl;
}
