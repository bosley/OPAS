/*
	Terminal color output
		  Josh Bosley
*/

#ifndef SCREEN_HEADER_GUARD
#define SCREEN_HEADER_GUARD

#include <stdio.h>

#define END_COLOR "\33[0m"

/*	All point to one function for easy format changing.	*/
#define OUTFUNC(c, s) printf("%s%s%s",c,s,END_COLOR);

#define OUT_BLACK(s) OUTFUNC("\33[0;30m", s)
#define OUT_RED(s) OUTFUNC("\33[0;31m", s)
#define OUT_GREEN(s) OUTFUNC("\33[0;32m", s)
#define OUT_BLUE(s) OUTFUNC("\33[0;34m", s)
#define OUT_CYAN(s) OUTFUNC("\33[0;36m", s)
#define OUT_PURPLE(s) OUTFUNC("\33[0;35", s)
#define OUT_BROWN(s) OUTFUNC("\33[0;33m", s)
#define OUT_GRAY(s) OUTFUNC("\33[0;37m", s)
#define OUT_DGRAY(s) OUTFUNC("\33[1;30m", s)
#define OUT_LBLUE(s) OUTFUNC("\33[1;34m", s)
#define OUT_LGREEN(s) OUTFUNC("\33[1;32m", s)
#define OUT_LCYAN(s) OUTFUNC("\33[1;36m", s)
#define OUT_LRED(s) OUTFUNC("\33[1;31m", s)
#define OUT_LPURPLE(s) OUTFUNC("\33[1;35m", s)
#define OUT_YELLOW(s) OUTFUNC("\33[1;33m", s)
#define OUT_WHITE(s) OUTFUNC("\33[1;37m", s)

#define CLEAR_SCREEN() printf("\33[2J");

#endif
