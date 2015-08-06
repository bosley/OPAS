#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include  <stdio.h>
#include   <time.h>

#include "jpath_get.h"

/*	SETTINGS	*/

#define DOT		88
#define MAXDOT 		20
#define MOPN		10
#define MAPBND 		30
#define MUP		 0
#define MLEFT		 1
#define MRIGHT		 2
#define MDOWN		 3
#define MAPSZE MAPBND*MAPBND

typedef struct Dotian
{
	int x;
	int y;
	int goalX;
	int goalY;
	int intent;
	int health;

} Dotian;

typedef struct World
{
	int map[MAPSZE];
	int popnum;

} World;

/*		DOTION OPERATIONS			*/

void DotSpawn    ( struct World *world,   struct Dotian *dot);
void DestroyDot  ( struct Dotian *dot);
 int TestMovement( struct World *world,   struct Dotian *dot, int dir);
void UpdateDotPos( struct Dotian *dot,    int dir);

/*		MAP OPERATIONS				*/

void DisplayMap  ( struct World *world);
void UpdateMap   ( struct World *world,   struct Dotian *dot);
void DisplayInfo ( struct World *world,	  struct Dotian *dot, int n);

/*		DECISION MAKING				*/

void Dengine    (  struct World *world,   struct Dotian *dot);

int main (void)
{

	/*	List of &dots[n] | Create the world	*/

	struct Dotian *dots  = malloc(MAXDOT * sizeof(Dotian));
	struct World  *world = malloc(sizeof ( struct World ));

	memset(world->map, 0, MAPBND * MAPBND);
	world->popnum = 0;

	DotSpawn(world, (struct Dotian *)&dots[0]);

	int n = 1;
	while(1)
	{
		printf("\033[2J");

		UpdateMap(world, &dots[0]);

//		DisplayInfo(world, &dots[0], n);

		DisplayMap(world);

		Dengine(world, &dots[0]);

		sleep(1);

		++n;
		}

	return 0;
}

/*		DECISION MAKING				*/

void Dengine(struct World *world, struct Dotian *dot)
{
	if(dot->intent == 0)
	{
		int d = rand() % 9;
		if(TestMovement(world, dot, d))
		{
			UpdateDotPos(dot, d);
		}
	}

	if(dot->intent == 1)
	{
		/*	Return dir of movement eg. 01234567	*/
		if ( retrievePath(dot->x, dot->goalX, dot->y, dot->goalY) == -1 )
		{
			dot->intent = 0;
			dot->goalX  = 0;
			dot->goalY  = 0;
		}
		else if( TestMovement(world, dot, retrievePath(dot->x, dot->goalX, dot->y, dot->goalY) ) )
		{
			UpdateDotPos(dot, retrievePath(dot->x, dot->goalX, dot->y, dot->goalY) );
		}
	}
}

/*		DOTION OPERATIONS			*/

void DotSpawn(struct World *world, struct Dotian *dot)
{
	dot->x		= rand() % MAPBND-2;
	dot->y		= rand() % MAPBND-2;
	dot->health	= 100;
	dot->goalX 	= 0;
	dot->goalY	= 0;
	dot->intent	= 0;
	world->popnum++;
}

void DestroyDot(struct Dotian *soul)
{
	free(soul);
}

void UpdateDotPos( struct Dotian *dot, int dir)
{
	switch(dir)
	{
		case 0: /* RIGHT */
			dot->x = (dot->x) + 1;
			break;
		case 1: /* LEFT */
			dot->x = (dot->x) - 1;
			break;
		case 2: /* UP */
			dot->y = (dot->y) - 1;
			break;
		case 3: /* DOWN */
			dot->y = (dot->y) + 1;
			break;
		case 4: /* UP+R */
			dot->y = (dot->y) - 1;
			dot->x = (dot->x) + 1;
			break;
		case 5: /* UL+L */
			dot->y = (dot->y) - 1;
			dot->x = (dot->x) - 1;
			break;
		case 6: /* DOWN+R */
			dot->y = (dot->y) + 1;
			dot->x = (dot->x) + 1;
			break;
		case 7: /* DOWN+L */
			dot->y = (dot->y) + 1;
			dot->x = (dot->x) - 1;
			break;
		default:
			break;
	}

}

int TestMovement(struct World *world, struct Dotian *dot, int dir)
{
	switch(dir)
	{
		case 0:
			if(world->map[((dot->x+1)*MAPBND)+dot->y+1] == MOPN && dot->x != MAPBND-2)
			{
			//	printf("\n\n\nR");
				return 1;
			}							/* RIGHT */
			break;
		case 1:
			if(world->map[((dot->x-1)*MAPBND)+dot->y+1] == MOPN && dot->x != 0)
			{
			//	printf("\n\n\nL");
				return 1;
			} 							/* LEFT  */
			break;
		case 2:
			if(world->map[(dot->x*MAPBND)+(dot->y-1)+1] == MOPN && dot->y != 0)
			{
			//	printf("\n\n\nU");
				return 1;					/* UP    */
			}
			break;
		case 3:
			if(world->map[(dot->x*MAPBND)+(dot->y+1)+1] == MOPN && dot->y != MAPBND-2)
			{
			//	printf("\n\n\nD");
				return 1;					/* DOWN  */
			}
			break;
		case 4:
			if(world->map[(dot->x*MAPBND)+(dot->y-1)+1] == MOPN && dot->y != 0
			&& world->map[((dot->x+1)*MAPBND)+dot->y+1] == MOPN && dot->x != MAPBND-2)
			{
			//	printf("\n\n\nU+R");
				return 1;					/* UP+R  */
			}
			break;
		case 5:
			if(world->map[(dot->x*MAPBND)+(dot->y-1)+1] == MOPN && dot->y != 0
			&& world->map[((dot->x-1)*MAPBND)+dot->y+1] == MOPN && dot->x != 0)
			{
			//	printf("\n\n\nU+L");
				return 1;					/* UP+L   */
			}
			break;
		case 6:
			if(world->map[(dot->x*MAPBND)+(dot->y+1)+1] == MOPN && dot->y != MAPBND-2
			&& world->map[((dot->x+1)*MAPBND)+dot->y+1] == MOPN && dot->x != MAPBND-2)
			{
			//	printf("\n\n\nD+R");
				return 1;					/*DOWN+R  */
			}
			break;
		case 7:
			if(world->map[(dot->x*MAPBND)+(dot->y+1)+1] == MOPN && dot->y != MAPBND-2
			&& world->map[((dot->x-1)*MAPBND)+dot->y+1] == MOPN && dot->x != 0)
			{
			//	printf("\n\n\nD+R");
				return 1;					/*DOWN+L  */
			}
			break;
		default:
			return -1;
	}
}

/*		MAP OPERATIONS				*/

void UpdateMap(struct World *world, struct Dotian *dot)
{
	register int y, x;
	for(y=0;y<MAPBND;++y)
	{
		for(x=0;x<MAPBND;++x)
		{
			if(y == dot->y && x == dot->x)
			{
				world->map[(x*MAPBND)+y+1] = DOT;
			} else {
				world->map[(x*MAPBND)+y+1] = MOPN;
			}
		}
	}
}

void DisplayMap(struct World *world)
{
	register int y, x;
	for(y=0;y<MAPBND;++y)
	{
		for(x=0;x<MAPBND;++x)
		{
			if (x+1 < MAPBND)
			{
				if (world->map[x*MAPBND+y+1] == MOPN)
					printf("   ");
				if (world->map[x*MAPBND+y+1] == DOT)
					printf(" * ");
			} else {
				printf("\n");
			}
		}
	}
}

void DisplayInfo (struct World *world, struct Dotian *dot, int n)
{
		printf(" Cycle : %d |", n);
		printf(" X : %d |", dot->x);
		printf(" Y : %d |", dot->y);
		printf(" Intent : %d \n",dot->intent);
}
