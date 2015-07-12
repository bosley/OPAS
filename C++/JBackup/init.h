#ifndef init_h
#define init_h

#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <cstring>
#include <time.h>
#include <sys/stat.h>

#define MAX_LINES   3
#define MAX_FPATH 255
#define MAX_JFILE  50
#define  MAX_JOBS  25
#define  MAX_NAME  32
#define  MAX_DATE  20

#define LOG_MODE    1
#define BUGS        1

#define TIME_SEPARATOR " "

#define JOB_LIST "JConfig.txt"

using namespace std;

struct jobs
{
	char title            [MAX_NAME];
	char from            [MAX_FPATH];
	char to              [MAX_FPATH];
	char dateTime         [MAX_DATE];
	char jobfile         [MAX_JFILE];
};

typedef struct jobs jobs;

int loadJobs ( jobs jlist[] );

char doJobs( jobs jlist[], int number_of_jobs  );

char updateJobFile( jobs jlist[], int index );

char shouldExecute( char * loadedTime );


#endif
