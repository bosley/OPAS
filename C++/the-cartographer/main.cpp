#include "err.h"
#include "world.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUGS 1

#define NUM_CYCLES 2


void showUsage()
{
	ERROUT("\n a.out [X-Y Size] [World Type] \n");
	ERROUT("\n Example : \n\t a.out 20 1 \n\n X-Y size must be < 40 \n");
	ERROUT("\n World Types :\n\t 1 - Simple | 2 - Complex \n");
}

int main(int argc, char **argv)
{
	if( 3 != argc )
	{
		showUsage();
		return 1;
	}
	else
	{
		if ( !atoi(argv[1]) || !atoi(argv[2]) )
		{
			showUsage();
			return 1;
		}
		if ( atoi(argv[1]) > 40 )
		{
			showUsage();
			return 1;
		}
	}

	// General information output
	INFOUT("\n Grid-Boundaries : ");	INFOUT(argv[1]); printf("\n");
	INFOUT("\n Map type : ");		INFOUT(argv[2]); printf("\n");

	//		Start doing things.

	srand(time(NULL));

	// For now initial population will be handled by patron placement in map file
	world World( atoi( argv[1] ), atoi( argv[1] ), -1, atoi( argv[2]) );

	World.displayGrid();

	return 0;

}
/*
Temp Graveyard

	int currentCycle = 0;

	for( currentCycle; currentCycle < NUM_CYCLES; currentCycle++ )
	{

	}
*/
