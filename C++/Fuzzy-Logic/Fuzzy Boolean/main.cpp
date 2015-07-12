#include "fbool.h"
#include <stdio.h>

void testValues( char*, double, fbool&);

int main()
{
    /*
        Default value is false, and will return 0 true

        Initialized as testBool(true), will return all true
    */

   fbool testBool;

   testValues( (char*)"0.25"  , 0.25 ,   testBool);
   testValues( (char*)"0.45"  , 0.45 ,   testBool);
   testValues( (char*)"0.75"  , 0.75 ,   testBool);
   testValues( (char*)"1.25"  , 1.25 ,   testBool);
   testValues( (char*)"-1.25" , -1.25,   testBool);

   /*

        Testing of operators

    */

    testBool.set(0.75);
    fbool testBool2(0.25);

    printf("testBool : %.2f\n", testBool.get());
    printf("testBool2: %.2f\n", testBool2.get());
    printf("testbool&&testBool2 : %.2f\n", (testBool&&testBool2).get());
    printf("testbool||testBool2 : %.2f\n", (testBool||testBool2).get());
    printf("testbool&&true : %.2f\n", (testBool&&true).get());
    printf("testbool&&false : %.2f\n", (testBool&&false).get());
    printf("testbool||true : %.2f\n", (testBool||true).get());
    printf("testbool||false : %.2f\n", (testBool||false).get());
    printf("!testbool : %.2f \n", (!testBool).get());
    printf("!testbool2 : %.2f\n", (!testBool2).get());

    return 0;
}


void testValues( char* x, double value, fbool& fuzzyBool)
{
    fuzzyBool.set(value);
    printf("\t %s value :  %.2f \n", x, fuzzyBool.get());
    int i = 1, j = 0, k = 0;
    for(;i<=100;i++)
    {
        k++;
        if(fuzzyBool.toBool())
            j++ ;
    }
    printf("\t %.2d out of %.2d were true.\n\n", j, k);
}
