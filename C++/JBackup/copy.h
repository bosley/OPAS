/*
    Ethan Laur
*/

#include "directoryReader.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define ERROR_FILE_OPEN 1
#define ERROR_NONE      0
#define ERROR_NONEXIST  2

int copy(char *f1, char *f2);
int recursiveCopy(char *p1, char *p2, char create = 0);
