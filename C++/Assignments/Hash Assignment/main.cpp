#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <ctype.h>

#define MAX_WORD    32
#define FILTER_SIZE 41
#define MAX_HASH    1024
#define EMPTY       "NULL"
#define INPUT_FILE  "speech.txt"

using namespace std;

// HashTable Structure
struct HashTable
{
     unsigned TableSize;
     unsigned Comparisons;
     unsigned Collisions;                   // Counter
     char     Name [10];                    // For output
     char     Table[ MAX_HASH ][ MAX_WORD ];// Table
     double   MeanKeyComparisons;           // Collisions
     double   MeanSearch;                   // Searching
};
typedef struct HashTable HashTable;

// Prototypes
unsigned Hash                   ( char *   );           // Hash Function
HashTable NewHashTable          ( unsigned, char* );     // Gen HashTable
bool isFull                     ( struct HashTable * ); // Test if full
void Finalize                   ( struct HashTable * ); // Calc & Print
bool ExistInTable               ( char * , struct HashTable *  );
void LinearProbe                ( char * , struct HashTable *, unsigned, char );
void QuadraticProbe             ( char * , struct HashTable *, unsigned, char );

// Main Body
int main(int argc, char *argv[])
{
    // Create hash table set 1
    HashTable LinearTable_one       = NewHashTable( 563, "Linear"    );
    HashTable QuadraticTable_one    = NewHashTable( 563, "Quadratic" ); 
    // Create hash table set 2
    HashTable LinearTable_two       = NewHashTable( 719, "Linear"    );
    HashTable QuadraticTable_two    = NewHashTable( 719, "Quadratic" );
    // Create hash table set 3
    HashTable LinearTable_three     = NewHashTable( 911, "Linear"    );
    HashTable QuadraticTable_three  = NewHashTable( 911, "Quadratic" );
    
    // Start getting info from file, and inserting to tables
    ifstream in;
    in.open(INPUT_FILE);            
    unsigned unique_words   = 0;  // Counters and key
    unsigned word_count     = 0; 
    unsigned key            = 0;
    char s [MAX_WORD];
    const char filter[ FILTER_SIZE ] = 
    ",.<\">;':{}[]-=_+@#$%^&*()~!`1234567890? ";
 
    while(!in.eof())  // Remove unwanted tid bits
    {
        in >> s;
        word_count++;
		for (unsigned j=0; s[j] != '\0'; j++)      
        {
            for(unsigned k=0; k<FILTER_SIZE; k++)
            {
				if (s[j] == filter[k])
                {
					for( unsigned xt = j; xt < strlen(s) ; xt++ )
						s[xt] = s[xt+1];
					if( j == strlen(s) )
						s[j] = 0;
				}
			}
			s[j] = toupper( s[j] );
        }
        // Begin inserting / Searching. 
        if ( !ExistInTable( s, &LinearTable_one ) )
        {
            unique_words++;
            key =  Hash   ( s ) ;           // This is %= in functions
            /*                  Set 1                          */
            // Insertions
            LinearProbe   ( s, &LinearTable_one   , key, 'i' );
            QuadraticProbe( s, &QuadraticTable_one, key, 'i' );
            // Searches
            LinearProbe   ( s, &LinearTable_one   , key, 's' );
            QuadraticProbe( s, &QuadraticTable_one, key, 's' );
            /*                  Set 2                          */
            // Insertions
            LinearProbe   ( s, &LinearTable_two   , key, 'i' );
            QuadraticProbe( s, &QuadraticTable_two, key, 'i' );
            // Searches
            LinearProbe   ( s, &LinearTable_two   , key, 's' );
            QuadraticProbe( s, &QuadraticTable_two, key, 's' );
            /*                  Set 3                          */
            // Insertions
            LinearProbe   ( s, &LinearTable_three   , key, 'i' );
            QuadraticProbe( s, &QuadraticTable_three, key, 'i' );
            // Searches
            LinearProbe   ( s, &LinearTable_three   , key, 's' );
            QuadraticProbe( s, &QuadraticTable_three, key, 's' );
        }
    }
	in.close();
	
    printf("\n Total words  : %i \n", word_count   );
    printf(" Unique words : %i \n", unique_words );
    
    // Calculate collision averages on set 1
    Finalize( &LinearTable_one      );
    Finalize( &QuadraticTable_one   );
    // Calculate collision averages on set 2
    Finalize( &LinearTable_two      );
    Finalize( &QuadraticTable_two   );
    // Calculate collision averages on set 3
    Finalize( &LinearTable_three    );
    Finalize( &QuadraticTable_three );
    
    printf( " < RETURN >" );
    getchar();
    return 0;
}

unsigned Hash( char * data )
{
    unsigned temp = 0, temp2;
    for( int i = 0; i < strlen( data ); i++)
    {
        temp2 = data[ i ];                  // Get el num
        temp = temp << 1 ;                  // Left shift!
        temp = temp ^ 2;                    // XOR
    }
    return temp;
}

HashTable NewHashTable( unsigned size, char* name ) // Make a table structure!
{
    HashTable temp                  ;
    strcpy(temp.Name, name)         ;
    temp.Collisions  = 0            ;
    temp.Comparisons = 0            ;
    temp.TableSize  = size          ;
    temp.MeanKeyComparisons = 0     ;
    for ( unsigned i = 0; i < MAX_HASH; i++)
        strcpy( temp.Table[ i ], EMPTY );
    return temp;
}

void LinearProbe( char * data, struct HashTable* Hash, unsigned key, char what )
{
    bool cycle = true;                               // For cycling table
    key %= Hash->TableSize;
    
    // Inserting
    if ( strcmp( Hash->Table[ key ], EMPTY ) == 0 && what == 'i' )
    {
         strcpy( Hash->Table[ key ], data );        // If spot is empty, insert
         return;
    }
    // Searching
    else if ( strcmp( Hash->Table[ key ], data ) == 0 && what == 's' )
    {
        Hash->Comparisons++;
        return;
    }
    else
    {                       
        while ( !isFull( Hash ) )                   // Cycle table until found
        {
            key++;
            if ( key == Hash->TableSize -1 && cycle)
            {
                key = 0;                  // If reach end from start init key
                cycle = false;       // Reset and tell not to reset next time
            }
            // Inserting
            if ( strcmp( Hash->Table[ key ], EMPTY ) == 0 && what == 'i' )
            {
                strcpy( Hash->Table[ key ], data ); 
                return;
            }
            else
                Hash->Collisions++;
            // Searching
            if ( strcmp( Hash->Table[ key ], data ) == 0 && what == 's' )
            {
                Hash->Comparisons++;
                return;
            }
            else
                Hash->Comparisons++;
        }
    }
}

void QuadraticProbe( char * data, struct HashTable* Hash, unsigned key, char what )
{
    key %= Hash->TableSize;                         //  Keep key in range
    
    // Inserting
    if ( strcmp( Hash->Table[ key ], EMPTY ) == 0 && what == 'i' )
    {
         strcpy( Hash->Table[ key ], data );
         return;
    }
    // Searching
    else if ( strcmp( Hash->Table[ key ], data ) == 0 && what == 's' )
    {
        Hash->Comparisons++;
        return;
    }
    else
    {
        int i = 1;                                  // For Quadratic operation
        int probe = key;
        while ( !isFull( Hash ) )                   // While there is EMPTY
        {
            // Plus
            probe = key + ( i * i );                // p(i) = h(k) + i^2
            if ( probe > Hash->TableSize )
                 probe %= Hash->TableSize;          // Ensure <= TableSize
            // Inserting
            if ( strcmp( Hash->Table[ probe ], EMPTY ) == 0 && what == 'i' )
            {
                strcpy( Hash->Table[ probe ], data );
                return;                          
            }
            // Searching
            else if ( strcmp( Hash->Table[ probe ], data ) == 0 && what == 's' )
            {
                Hash->Comparisons++;
                return;
            }
            // Minus
            probe = key - ( i * i );               // p(i) = h(k) - i^2
            while ( probe < 0 )
                probe += Hash->TableSize;          // Make sure not < 0
            // Inserting
            if ( strcmp( Hash->Table[ probe ], EMPTY ) == 0 && what == 'i' )
            {
                strcpy( Hash->Table[ probe ], data );
                return;
            }
            // Searching
            else if ( strcmp( Hash->Table[ probe ], data ) == 0 && what == 's' )
            {
                Hash->Comparisons++;
                return;
            }
            // Collision
            i++;
            Hash->Collisions++;
        }
    }
}

bool ExistInTable( char * s,  struct HashTable* Hash )
{
     for ( int i = 0; i < Hash->TableSize; i++ )
          if ( strcmp( Hash->Table[ i ], s ) == 0 )
               return true;
    return false;   
}

bool isFull( struct HashTable* Hash )          // For testing if there is room
{
    for ( int i = 0; i < Hash->TableSize; i++ )
          if ( strcmp( Hash->Table[ i ], EMPTY ) == 0 )
               return false;
    return true;
}

void Finalize ( struct HashTable * Hash )
{   
    Hash->MeanKeyComparisons = ( double( Hash->Collisions ) 
                               / double( Hash->TableSize  ) );
                               
    Hash->MeanSearch = ( double( Hash->Comparisons ) 
                       / double( Hash->TableSize   ) );
                       
    printf( "\n %s [ %i ] Mean Search Comparison %2.2f \n",
            Hash->Name, Hash->TableSize, Hash->MeanSearch  );
}




