/*
    Ethan Laur
*/

#include "directoryReader.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

directoryReader::directoryReader()
{}

directoryReader::directoryReader(char *d)
{
	setFileMode(S_IFREG);
	setDirectory(d);
}

directoryReader::directoryReader(char *d, mode_t m)
{
	setFileMode(m);
	setDirectory(d);
}

void directoryReader::setDirectory(char * newDir)
{
     dir = NULL;
	strcpy(dirName, newDir);
	reset();
}

void directoryReader::setFileMode(mode_t mode)
{
	fileMode = mode;
}

void directoryReader::reset()
{
	if (dir != NULL) closedir(dir);
	dir = opendir(dirName);
}

char * directoryReader::getNext()
{
	struct stat st;
	char        *buf = (char *)malloc(1024); //ihs
	while ((ent = readdir(dir)) != NULL)
	{
		sprintf(buf, "%s/%s", dirName, ent->d_name);
		stat(buf, &st);
		if (st.st_mode & fileMode)
			return strdup(ent->d_name);
	}
	return NULL;
}

char * directoryReader::getNextDir()
{
	struct stat st;
	char        buf[1024];
	while ((ent = readdir(dir)) != NULL)
	{
		if (strstr(ent->d_name, ".") != NULL) continue;
		sprintf(buf, "%s/%s", dirName, ent->d_name);
		stat(buf, &st);
		if (S_ISDIR(st.st_mode))
			return strdup(ent->d_name);
	}
	return NULL;
}
