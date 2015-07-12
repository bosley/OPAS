/*
    Ethan Laur
*/

#include "copy.h"

int copy(char *f1, char *f2)
{
	FILE *f, *g;
	unsigned char c;
	if ((f = fopen(f1, "rb")) == NULL || (g = fopen(f2, "wb")) == NULL) ERROR_FILE_OPEN;
	while (fread(&c, 1, 1, f) > 0)
    {
         fwrite(&c, 1, 1, g);
        
    }
	fflush(g);
	fclose(f);
	fclose(g);
	return ERROR_NONE;
}

void createDirs(directoryReader *dirr, char *path, char *base, char *base2)
{
	char *file = NULL;
	char *buf = (char *)malloc(1024);
	char *buf2 = (char *)malloc(1024);
	if (path)
	{
		sprintf(buf, "%s\\%s", base, path);
		sprintf(buf2, "%s\\%s", base2, path);
		mkdir(buf);
		createDirs(new directoryReader(buf2, 0), NULL, buf, buf2);
	}
	while ((file = dirr->getNextDir()) != NULL)
		createDirs(dirr, file, base, base2);
}

int recursiveCopy(char *p1, char *p2, char create)
{
	struct stat st;
	char *buf = (char *)malloc(strlen(p2) * 4.576); //these paths get much larger as we go along
	char *buf2 = (char *)malloc(strlen(p2) * 4.576); //there is also a possible stack overflow bug here
	char *file = NULL;
	if (create)
	   mkdir(p2);
	directoryReader *dirr = new directoryReader(p1, S_IFREG | S_IFDIR);
	createDirs(new directoryReader(p1, S_IFREG), NULL, strdup(p2), strdup(p1));
	while ((file = dirr->getNext()) != NULL)
	{
		if (file[0] == '.') continue;
		sprintf(buf, "%s\\%s", p1, file);
		sprintf(buf2, "%s\\%s", p2, file);
		stat(buf, &st);
		if (st.st_mode & S_IFDIR == S_IFDIR)
		{
			stat(buf, &st);
			if (!(st.st_mode & S_IFREG) && !(st.st_mode & S_IFDIR))
				mkdir(buf2);
			recursiveCopy(strdup(buf), strdup(buf2));
		}
		else
		    copy(buf, buf2);
	}
	//if you really want to make this non-thread safe, go ahead and free buf, buf2, and file here
	return ERROR_NONE;
}
