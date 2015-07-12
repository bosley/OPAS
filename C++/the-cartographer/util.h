#ifndef JUTIL_H
#define JUTIL_H

#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

inline int randInt( int a, int b )
{
	return rand()%(a-b);
}

inline bool randBool()
{
	if( 5 >= (rand()%10) )
		return true;
	else
		return false;
}

inline double randDouble()
{
	return (rand())/(100+1.0);
}

void explode( char *data, double (&_array)[6] )
{


/*
	int c = 0;
	char *toke;
	char *p = data;

	while ( ( toke = strtok(p, ",:") ) != NULL )
	{
		_array[c] = atof( toke );

		printf("%f", array[c]);

		p = NULL;
	}
*/
}


/*
        for( int i = 0, c = 0; i < len; i++ )
	{
		if( data[i] == ',' )
		{
			c_index[c] = i;
			c++;
		}
		else if ( data[i] == ':' )
			sep = i;
	}

	for( int i = 0, s = 0, c = 0, ci = 0; i < 4; i++ )
	{
		while( s < c_index[i] )
		{
			cbuff[c] = data[s];
			s++;
		}
		coords[ci++] = atof(cbuff);
		s = c_index[i]+1; // Set to position after comma
		c = 0;
	}

	for(int i = 0; i < 6; i++ )
	{
		printf("\n %f ", coords[i]);
	}

0-----------0000---------------------------------------------------------------------------


        for( int i = 0, b = 0, c = 0; i < len; i++ )
        {
        }




		char buff[5];

		if( data[i] != ',' && data[i] != ':' )
		{
			buff[b] = data[i];
		}
		else
			b = 0;

		if( data[i] == ',' )
		{
			coords[c] = atof(buff);
			c++;
		}

//                printf("coords: %c\n", data[i]);


*/


#endif
