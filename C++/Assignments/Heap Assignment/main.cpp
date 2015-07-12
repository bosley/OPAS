#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define MAX_DATA  32
#define HEAP_SIZE 10
#define YEAR_SEED 42
#define NAME_NUM   9

/*	HEAP OPERATIONS		*/

class heapItem
{
	private:
		int years;
		char name[MAX_DATA];
	public:
		heapItem() 			{ years = 0; strcpy(name,"null"); }
		heapItem(int y, char * s) 	{ years = y; strcpy(name, s) 	; }
		int     getYears() 		{ return years ; }
		char *  getName ()		{ return name  ; }
		void    showData() 		{ printf(" %s with %i years experience \n", name, years); }

		friend bool operator>  ( heapItem lhs, const heapItem &rhs )
		{
			return ( lhs.years > rhs.years ) ;
		}
		friend bool operator<  ( heapItem lhs, const heapItem &rhs )
		{
			return ( lhs.years < rhs.years ) ;
		}
		friend bool operator!= ( heapItem lhs, const heapItem &rhs )
		{
			return !( !strcmp(lhs.name, rhs.name) && lhs.years == rhs.years ) ;
		}
};

class theHeap
{
	private:
		heapItem heap[HEAP_SIZE]	;
		heapItem temp			;
		int heap_index;
	public:
		theHeap() { heap_index = 0;	}
		void   showData  (  	    )	;
		void   swapData  ( int, int ) 	;
		void    enqueue  ( heapItem ) 	;
		void    dequeue  (          ) 	;
};

// Used in testing
void theHeap::showData( )
{
	int x = 0;

	printf("\n\n");

	for(x; x<heap_index;x++)
	{
		printf(" index= %i ", x);
		heap[x].showData();
		printf("\n");

	}
}

void theHeap::swapData( int from, int to )
{
	temp = heap[ to ];
	heap[ to ]   = heap[ from ];
	heap[ from ] = temp;
}

void theHeap::enqueue( heapItem hel )
{
	if ( heap_index != HEAP_SIZE )
	{
		heap[ heap_index ] = hel;
	}

	if ( heap_index > 0 )
	{
		int el_index = heap_index;

		while( el_index > 0 && heap[ el_index ] > heap[ ((el_index -1)/2) ] )
		{
			swapData(el_index, (el_index-1)/2);
			el_index = ((el_index-1)/2);
		}
	}
	if( heap_index < HEAP_SIZE-1)
		heap_index++;
}

void theHeap::dequeue( )
{
	heap[0].showData();

	int el_index = 0;

	swapData( el_index, heap_index-1 );

	if (heap_index >0)
		heap_index--;

	while ( heap[ el_index ] < heap[ (( 2 * el_index ) +1 ) ] || heap[ el_index ] < heap[ (( 2 * el_index ) +2 ) ] )
	{
		// Left child > Right Child
		if ( heap[ ( 2 * el_index) +1 ] > heap[ ( 2 * el_index) +2 ] )
		{
			swapData(el_index, (( 2 * el_index) +1) );

			if ( el_index = (( 2 * el_index) +1)  > 0 )
				el_index = (( 2 * el_index) +1) ;
		}
		// Right child > Left Child
		if ( heap[ ( 2 * el_index) +2 ] > heap[ ( 2 * el_index) +1 ] )
		{
			swapData(el_index, (( 2 * el_index) +2) );

			if ( el_index = (( 2 * el_index) +2) > 0 )
				el_index = (( 2 * el_index) +2) ;
		}
	}
}

/*	MAIN OPERATIONS		*/

const int 	generateYears();
char * 		generateName ();

int main(void)
{
	int index = 0;

	srand(time(NULL));

	theHeap myHeap;

	printf("\n\n");

	for(index; index < HEAP_SIZE; index++)
	{
		heapItem tempel  ( generateYears(), generateName() );
		printf(" Enqueue : ");
		tempel.showData();
		myHeap.enqueue   ( tempel  );
	}

	for(index = 0; index < 5; index++)
	{
		printf(" Dequeue : ");
		myHeap.dequeue();
	}

	return 0;
}

const int generateYears()
{
	return rand() % YEAR_SEED;
}

char * generateName()
{
	const char * names[ NAME_NUM ] = { " Josh Bosley "," Joseph Smith ",
		" Trenton Boocher ", " Donald Duck ", " Goofy ", " Tom Boger ",
		" Mickey Mouse ", " Zoltan ", " Tony Danza " };
	return (char *)names[ ( rand() % NAME_NUM ) ] ;
}
