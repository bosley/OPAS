#include "fset.h"
#include "stdio.h"

using namespace std;

void showInfo( DATA_TYPE, fset);

int main()
{

    // First Test Set

    fset testSet;

    printf("\n\t Creating, and setting testSet\n\n");

    testSet.insert( (DATA_TYPE)"false", false);
    testSet.insert( (DATA_TYPE)"true" , true);
    testSet.insert( (DATA_TYPE)"third", 0.33);
    testSet.insert( (DATA_TYPE)"half" , 0.5);

    showInfo( (char*)"testSet",testSet);

    // Second Test Set

    printf("\n\t Creating, and setting testSet2\n\n");

    fset testSet2;

    testSet2.insert( (DATA_TYPE)"false", false);
    testSet2.insert( (DATA_TYPE)"true" , true);
    testSet2.insert( (DATA_TYPE)"quarter", 0.25);
    testSet2.insert( (DATA_TYPE)"third", 0.33);
    testSet2.insert( (DATA_TYPE)"half" , 0.5);

    showInfo( (DATA_TYPE)"testSet2",testSet2);

    // Set Operations

    printf("\n\t Creating, and setting operation sets \n\n");

    fset andset = testSet2&&testSet;
    fset orset  = testSet2||testSet;
    fset notset = !testSet;

    printf("andset[\"false\"] : %2.f \n", andset[(DATA_TYPE)"false"].get());
    printf("orset[\"false\"]  : %2.f \n", orset[(DATA_TYPE)"false"].get());
    printf("notset[\"false\"] : %2.f \n\n\n", notset[(DATA_TYPE)"false"].get());

    return 0;
}

void showInfo(DATA_TYPE name, fset x)
{
    printf("%s isEmpty : %d\n", name, x.isEmpty());
    printf("%s size : %d\n", name, x.size());
}
