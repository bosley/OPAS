#include "err.h"
#include "util.h"
#include "world.h"
#include "screen.h"
#include <string.h>

// Grid Images
#define OPEN_SPACE " # "
#define WALL_SPACE "[=]"
#define PATRON_SPACE " * "

// Grid Values
#define OPEN_VAL 0
#define PATRON_VAL 1
#define WALL_VAL 2
#define GOAL_VAL 3

// Color Codes
#define COLOR_ID_BLACK	0
#define COLOR_ID_RED	1
#define COLOR_ID_GREEN	2
#define COLOR_ID_BLUE	3
#define COLOR_ID_CYAN	4
#define COLOR_ID_PURPLE	5
#define COLOR_ID_GRAY	6
#define COLOR_ID_DGRAY	7
#define COLOR_ID_LBLUE	8
#define COLOR_ID_LGREEN	9
#define COLOR_ID_LCYAN	10
#define COLOR_ID_LRED	11
#define COLOR_ID_LPURP  12
#define COLOR_ID_YELLOW	13
#define COLOR_ID_WHITE	14

// Map Building
#define MAP_Q1 "@Q1"
#define MAP_Q2 "@Q2"
#define MAP_Q3 "@Q3"
#define MAP_Q4 "@Q4"

// Directions for path finding
#define MOVE_UP     0
#define MOVE_UP_L   0
#define MOVE_UP_R   0
#define MOVE_LEFT   0
#define MOVE_RIGHT  0
#define MOVE_DOWN   0
#define MOVE_DOWN_L 0
#define MOVE_DOWN_R 0

world::world( unsigned inX, unsigned inY, int initialPopulation, unsigned map_arg )
{
	gridX = inX;
	gridY = inY;

	objectCount = 0;
	populationCount = initialPopulation;

	// ARG DETERMINES MAP BUILD TYPE
	if( EXIT_GOOD == loadMap( map_arg, inX, inY ) )
	{
		if ( initialPopulation != 0 )
		{
			for(int i = 0; i < initialPopulation; i++)
			{
				spawnGridPatron( 0, i );
			}
		}
	}
}

int world::getGridNodeVal( unsigned y, unsigned x )
{
	return worldGrid[x][y].val;
}

void world::spawnGridPatron( unsigned y, unsigned x )
{
	populationCount += 1;
	population[ populationCount ].intelligence = randDouble();
	population[ populationCount ].id = populationCount;
	population[ populationCount ].color = COLOR_ID_RED;
	population[ populationCount ].health = 100.00;
	population[ populationCount ].age = 1;
	population[ populationCount ].X = x;
	population[ populationCount ].Y = y;

	worldGrid[x][y].val = PATRON_VAL;
	worldGrid[x][y].free = false;
	worldGrid[x][y].occupantId = populationCount;
}

void world::spawnGridObject( unsigned y, unsigned x, float z, int objType )
{

	switch( objType )
	{
		case OPEN_VAL:
			objectList[ objectCount ].color = COLOR_ID_BLACK;
			objectList[ objectCount ].hp = 0;
			worldGrid[x][y].free = true;
			break;

		case WALL_VAL:
			objectList[ objectCount ].color = COLOR_ID_DGRAY;
			objectList[ objectCount ].hp = 100;
			worldGrid[x][y].free = false;
			break;
	}

	objectList[ objectCount ].objectType = objType;
	objectList[ objectCount ].id = objectCount;
	objectList[ objectCount ].X = x;
	objectList[ objectCount ].Y = y;

	// Link the grid unit to the object
        worldGrid[x][y].occupantId = objectCount;

	worldGrid[x][y].val = objType;
	worldGrid[x][y].X = x;
	worldGrid[x][y].Y = y;
	worldGrid[x][y].Z = z;

	objectCount += 1;
}


void resetGridUnit(unsigned x, unsigned y, char )
{
	// CODE THIS TO HANDLE KILLING THE PATRON / OBJECT AND SETTING TO BLANK SQUARE AT ITS ORIGINAL HEIGHT

}


int * world::queryGridUnit( unsigned y, unsigned x, char query)
{
	int *dp;
	int data[10];
	dp = data;

	switch( query )
	{
		case QUERY_OBJECT:
			data[0] = objectList[ worldGrid[x][y].occupantId ].X  		;
			data[1] = objectList[ worldGrid[x][y].occupantId ].Y  		;
			data[2] = objectList[ worldGrid[x][y].occupantId ].hp 		;
			data[3] = objectList[ worldGrid[x][y].occupantId ].color 	;
			data[4] = objectList[ worldGrid[x][y].occupantId ].objectType 	;
			data[5] = -1;
			break;
		case QUERY_PATRON:
			data[0] = population[ worldGrid[x][y].occupantId ].X 		;
			data[1] = population[ worldGrid[x][y].occupantId ].Y 		;
			data[2] = population[ worldGrid[x][y].occupantId ].age 		;
			data[3] = population[ worldGrid[x][y].occupantId ].color 	;
			data[4] = population[ worldGrid[x][y].occupantId ].health 	;
			data[5] = population[ worldGrid[x][y].occupantId ].intelligence ;
			data[6] = -1;
			break;
	}
	return dp;
}

void world::displayGrid()
{
	CLEAR_SCREEN();

	int currVal = 0;

	for(int y=0; y < gridY; y++ )
	{
		for(int x=0; x < gridX; x++)
		{
			switch( worldGrid[x][y].val ) 						// Determine what is on the grid
			{
				case OPEN_VAL:
					OUT_BLACK(OPEN_SPACE);
					break;

				case PATRON_VAL:

					if( COLOR_ID_RED == population[ worldGrid[x][y].occupantId ].color )
					{
						OUT_RED(PATRON_SPACE);
					}
					if( COLOR_ID_BLUE == population[ worldGrid[x][y].occupantId ].color )
					{
						OUT_BLUE(PATRON_SPACE);
					}
					break;

				case WALL_VAL:

					if( 50 > objectList[ worldGrid[x][y].occupantId ].hp )
					{
						OUT_GRAY(WALL_SPACE);
					}
					if( 50 <= objectList[ worldGrid[x][y].occupantId ].hp )
					{
						OUT_DGRAY(WALL_SPACE);
					}

					break;
			}
        	}
        	printf("\n");
	}
}

char world::loadMap(unsigned arg, unsigned inX, unsigned inY)
{
	if( LOAD_BLANK_MAP == arg )
	{
        	for(int y=0; y < inY; y++ )
        	{
        	        for(int x=0; x < inX; x++)
        	        {
				spawnGridObject( x, y, 0.0, OPEN_VAL );
 	                        worldGrid[x][y].Z = 0.0;
        	        }
        	}
	}

	else if( LOAD_BASIC_MAP == arg )
	{
		buildMapFromFile((char *)"maps/basic_map.enmap");
	}

	else if( LOAD_CMPLX_MAP == arg )
	{
		buildMapFromFile((char *)"maps/complex_map.enmap");
	}

	else
	{
		return EXIT_NOT_GOOD;
	}

	return EXIT_GOOD;
}

void world::buildMapFromFile(char * map_file)
{
	printf("\n\n\t Current Map: %s\n\n", map_file);

        FILE * map_fp = fopen(map_file, "r");

        if ( NULL == map_fp )
        {
                printf("\n\n\t Error opening map file \n\n");
        }

        // Qnx, and Qny represet where that quadrant's 0,0 is located on world grid
        int maxQx = (gridX/2), maxQy = (gridY/2), Qnx, Qny;

	// Qn_index marks @Qn index in data[]. objType is type of object.
	int block = 1024, Qn_index[3], obj_index[MAX_O], data_itr = 0, objType, obj_itr = 0, Qn_itr = 0;

	char data[block][block];

	float sx,sy,sz,ex,ey,ez,hd;

	// Store data, and mark beginning of each quad in data, as well as objects in quads
	while( fgets( data[data_itr], block, map_fp ) )
	{
		char * strip = strstr(data[data_itr], "\n");
		if ( strip != NULL ) *strip = 0;

		// Quads
		if( 0 == strcmp( MAP_Q1, data[data_itr] ) )
			Qn_index[ 0 ] = data_itr;
		if( 0 == strcmp( MAP_Q2, data[data_itr] ) )
			Qn_index[ 1 ] = data_itr;
		if( 0 == strcmp( MAP_Q3, data[data_itr] ) )
			Qn_index[ 2 ] = data_itr;
		if( 0 == strcmp( MAP_Q4, data[data_itr] ) )
			Qn_index[ 3 ] = data_itr;

		// Objects
		if( 0 == strcmp( QWALL, data[data_itr] ) )
			obj_index[ obj_itr++ ] = data_itr;
		if( 0 == strcmp( QPATR, data[data_itr] ) )
			obj_index[ obj_itr++ ] = data_itr;
		if( 0 == strcmp( QGOAL, data[data_itr] ) )
			obj_index[ obj_itr++ ] = data_itr;
		if( 0 == strcmp( QEND, data[data_itr] ) )
			obj_index[ obj_itr ] = data_itr;

		data_itr++;
	}
	fclose(map_fp);

	// For each object
	for(int i = 0; i < obj_itr; i++)
	{
		// Get current obj type.
		if ( 0 == strcmp(data[ obj_index[i] ], QWALL) )
			objType = WALL_VAL;
		if ( 0 == strcmp(data[ obj_index[i] ], QPATR) )
			objType = PATRON_VAL;
		if ( 0 == strcmp(data[ obj_index[i] ], QGOAL) )
			objType = GOAL_VAL;

		if (BUGS && 0) printf("\n\t Object at : %d | Type : %d", obj_index[i], objType );

		// Get coords out of data by means of j for each object
		for( int j = obj_index[i]+1; j <= obj_index[i]+(obj_index[i+1] - obj_index[i] )-2; j++ )
		{
			if (BUGS && 0) printf("\n\n Data from inner loop : %s", data[ j ]);

			// Find current quadrant ( for calculating conversion from quad to grid )

			if ( j >= Qn_index[0] && j < Qn_index[1] )
			{
				Qn_itr = 1;
			}
			else if ( j >= Qn_index[1] && j < Qn_index[2] )
			{
				Qn_itr = 2;
			}
			else if ( j >= Qn_index[2] && j < Qn_index[3] )
			{
				Qn_itr = 3;
			}
			else if ( j >= Qn_index[3] )
			{
				Qn_itr = 4;
			}

			// get coords from line and create object
			sscanf( data[j] , "%f,%f,%f:%f,%f,%f", &sx, &sy, &sz, &ex, &ey, &ez );

			// To Calculate x/y location and draw.
			int s_scaled_x = -1, s_scaled_y = -1, e_scaled_x = -1, e_scaled_y = -1, cx, cy;

			s_scaled_x = sx * maxQx;
			s_scaled_y = sy * maxQy;
			e_scaled_x = ex * maxQx;
			e_scaled_y = ey * maxQy;

// If scaled value is 0 , then align to quadrant pos
// BEGIN 0 ERROR ALIGN

			if ( s_scaled_x == 0 )
			{
				switch( Qn_itr )
				{
					case 3:
						s_scaled_x = maxQy; // 0x in Q3 = 1/2gridY
						break;
					case 4:
						s_scaled_x = maxQy; // 0x in Q4 = 1/2gridY
						break;
				}
			}
			if ( s_scaled_y == 0 )
			{
				switch( Qn_itr )
				{
					case 2:
						s_scaled_y = maxQx; // 0y in Q2 = 1/2gridX
						break;
					case 3:
						s_scaled_y = maxQx; // 0y in Q3 = 1/2gridX
						break;
					case 4:
						s_scaled_y = maxQx; // 0y in Q3 = 1/2gridY
						break;
				}
			}

			// Allign end point xy

			if ( e_scaled_x == 0 )
			{
				switch( Qn_itr )
				{
					case 3:
						e_scaled_x = maxQy; // 0x in Q3 = 1/2gridY
						break;
					case 4:
						e_scaled_x = maxQy; // 0x in Q4 = 1/2gridY
						break;
				}
			}
			if ( e_scaled_y == 0 )
			{
				switch( Qn_itr )
				{
					case 2:
						e_scaled_y = maxQx; // 0y in Q2 = 1/2gridX
						break;
					case 3:
						e_scaled_y = maxQx; // 0y in Q3 = 1/2gridX
						break;
					case 4:
						e_scaled_y = maxQx; // 0y in Q3 = 1/2gridY
						break;
				}
			}
// END 0 ERROR ALIGN

// BEGIN QUADRANT SPECIFIC NON-0 ALIGNMENT

			switch( Qn_itr )
			{
				case 1 :
					// Assuming the map maker isn't an idiot, nothing needs to
					// Be here. Perhaps I will make parameters later to handle.
					break;

		// CASE 2 : NEED TO VERIFY, SHOULD BE FINE
				case 2 :
					// Q2 needs to compensate for x, but not y
					if ( s_scaled_x < maxQx ) s_scaled_x += maxQx;
					if ( e_scaled_x < maxQx ) e_scaled_x += maxQx;

					break;

		// Verified works
				case 3 :
					if ( s_scaled_y < maxQy ) s_scaled_y += maxQy;
					if ( e_scaled_y < maxQy ) e_scaled_y += maxQy;
					// Q3 does not need to compensate for x
					break;

		// Verified works
				case 4 :
					if ( s_scaled_y < maxQy ) s_scaled_y += maxQy;
					if ( e_scaled_y < maxQy ) e_scaled_y += maxQy;
					// Q4 needs to compensate for x, and y
					if ( s_scaled_x < maxQx ) s_scaled_x += maxQx;
					if ( e_scaled_x < maxQx ) e_scaled_x += maxQx;
					break;
			}

// END QUADRANT SPECIFIC NON-0 ALIGNMENT

			if (BUGS) printf("\n\n\t Q%d \n s_scaled_x = %d | s_scaled_y = %d \n e_scaled_x = %d | e_scaled_y = %d \n",
			Qn_itr, s_scaled_x, s_scaled_y, e_scaled_x, e_scaled_y);


			findDrawPath(s_scaled_x, s_scaled_y, e_scaled_x, e_scaled_y);


// ------ EVERYTHING IS KNOWN TO WORK TO HERE> DO NOT DELETE ABOVE CODE

			// Height Calculaton
			if ( sz > ez && sz != ez )
			{
				hd = sz - ez;
				hd = hd / ( path_index - 2 ); // -1 Because of the method of indexing.
			}
			else if ( sz < ez && sz != ez )
			{
				hd = ez - sz;
				hd = hd / ( path_index - 2 ); // -2 might be more best.
			}

			// DRAW OBJECTS IN PATH AT Z HEIGHT
			float hd_inc = 0.0, z = hd_inc;
			for( int i = 0; i < path_index; i++)
			{
				z = (sz + hd_inc);
				if (BUGS) printf("\n PD[%d] | x : %d | y : %d | z : %f\n", i, pathDraw[i].x, pathDraw[i].y, z );

				switch( objType )
				{
					case PATRON_VAL:
						spawnGridPatron( pathDraw[i].y, pathDraw[i].x );
						break;

					default :
						spawnGridObject(pathDraw[i].y, pathDraw[i].x, z, objType);
						break;
				}
				hd_inc += hd;
			}
		}
		if (BUGS && 0) printf("\n Lines of coords between objs : %d ", ( obj_index[i+1] - obj_index[i] )-2 );
	}
	printf("\n");
} // End load

void world::findDrawPath(unsigned sx, unsigned sy, unsigned gx, unsigned gy)
{
	// Clear out pathDraw
	for ( int i = 0; i < (MAX_X + MAX_Y )-1; i++)
	{
		pathDraw[i].x = 0;
		pathDraw[i].y = 0;
	}

	_tuple curr = {sx, sy}, goal = {gx, gy};

	path_index = 0;
	pathDraw[path_index].x = sx;
	pathDraw[path_index].y = sy;
	path_index++;

	// Direction determination
	bool compute = 1;

	while( compute )
	{
		if (BUGS) printf("\n Computing Path | x= %d | y= %d \n", curr.x, curr.y);

		if ( curr.y < goal.y && curr.x < goal.x ) // Down right
		{
			curr.y++;
			curr.x++;
			pathDraw[path_index++] = curr;
//			printf("DR");
		}
		if ( curr.y < goal.y && curr.x > goal.x ) // Down left
		{
			curr.y++;
			curr.x--;
			pathDraw[path_index++] = curr;
//			printf("DL");
		}
		if ( curr.y < goal.y && curr.x == goal.x ) // Down
		{
			curr.y++;
			pathDraw[path_index++] = curr;
//			printf("D");
		}
		if ( curr.y > goal.y && curr.x > goal.x ) // Up Left
		{
			curr.y--;
			curr.x--;
			pathDraw[path_index++] = curr;
//			printf("UL");
		}
		if ( curr.y > goal.y && curr.x < goal.x ) // Up Right
		{
			curr.y--;
			curr.x++;
			pathDraw[path_index++] = curr;
//			printf("UR");
		}
		if ( curr.y > goal.y && curr.x == goal.x ) // Up
		{
			curr.y--;
			pathDraw[path_index++] = curr;
//			printf("U");
		}
		if ( curr.y == goal.y && curr.x > goal.x ) // Left
		{
			curr.x--;
			pathDraw[path_index++] = curr;
//			printf("L");
		}
		if ( curr.y == goal.y && curr.x < goal.x ) // RIght
		{
			curr.x++;
			pathDraw[path_index++] = curr;
//			printf("R");
		}

		if ( curr.x == goal.x && curr.y == goal.y )
		{
			compute = false;
		}
	}
}
