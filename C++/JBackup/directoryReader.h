/*
    Ethan Laur
*/

#ifndef __appBetaServer__directoryReader__
#define __appBetaServer__directoryReader__

#include "dirent.h"
#include <sys/stat.h>

class directoryReader
{
protected:
	DIR           *dir;
	struct dirent *ent;
	mode_t         fileMode;
	char           dirName[64];
public:
	directoryReader();
	directoryReader(char *);
	directoryReader(char *, mode_t);
	void setDirectory(char *);
	void setFileMode(mode_t);
	void reset();
	char *getNext();
	char *getNextDir();
};

#endif /* defined(__appBetaServer__directoryReader__) */
