#ifndef WORLD_H_GUARD
#define WORLD_H_GUARD

// World limits
#define MAX_X 255
#define MAX_Y 255
#define MAX_P 1000
#define MAX_O MAX_X * MAX_Y + MAX_X

// Grid unit probing
#define QUERY_OBJECT 0
#define QUERY_PATRON 1

// Map loading variables
#define QSKIP "[skip]"
#define QWALL "[wall]"
#define QGOAL "[goal]"
#define QPATR "[patron]"
#define QEND  "[END]"

#define LOAD_BLANK_MAP 0
#define LOAD_BASIC_MAP 1
#define LOAD_CMPLX_MAP 2


// id is the index in object list
// objectType is obvious, and hp so is hp
struct gridObject
{
	unsigned id, X, Y;
	int hp, color, objectType;
};
typedef struct gridObject _gridObject;

//obvious
struct gridPatron
{
	int age;
	double health;
	double intelligence;
	unsigned color, X, Y, id;

};
typedef struct gridPatron _gridPatron;

// occupantId is the location of the object in it's stored list
// val is a number indicating which kind of object it is. Wall, Blank, Patron, etc
// height is used for computing score -10, 10
struct gridNode
{

	float Z;
	unsigned X, Y, occupantId;
	bool free;
	int val, g_score, f_score, h_score;

	gridNode& operator =(const gridNode& a)
	{
		X = a.X;
		Y = a.Y;
		val = a.val;
		free = a.free;
		Z = a.Z;
		occupantId = a.occupantId;
		return *this;
	}

	bool operator ==(const gridNode& a)
	{
		return( X == a.X && Y == a.Y );
	}

};
typedef struct gridNode _gridNode;

struct tuple
{
	unsigned x, y;

	tuple& operator=(const tuple& a)
	{
		x = a.x;
		y = a.y;
		return *this;
	}

	bool operator ==(const tuple& a)
	{
		return( x == a.x && y == a.y);
	}
};
typedef struct tuple _tuple;


class world
{
private:

	unsigned gridX, gridY;
	int populationCount, objectCount, path_index;
	_gridNode worldGrid[MAX_X][MAX_Y];
	_gridPatron population[MAX_P];
	_gridObject objectList[MAX_O];

	// Handles drawing maps from encoded file
	_tuple pathDraw[ MAX_X + MAX_Y ];

	char loadMap( unsigned map_arg, unsigned inX, unsigned inY);
	void buildMapFromFile( char *filename );

	void findDrawPath( unsigned sx, unsigned sy, unsigned gx, unsigned gy );

public:

	world( unsigned gridX, unsigned gridY, int initialPopulation, unsigned map_arg );

	int getGridNodeVal( unsigned x, unsigned y);

	void spawnGridPatron( unsigned x, unsigned y );

	void spawnGridObject( unsigned x, unsigned y, float z, int objectType);

	int *queryGridUnit( unsigned x, unsigned y, char query);

	void displayGrid();

	/*		NEED TO CODE		*/

	void resetGridUnit( unsigned x, unsigned y, char );

};

#endif
