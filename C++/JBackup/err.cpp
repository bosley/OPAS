#define jb_err_no
char jberrno; //this has to come before the include
#include "err.h"
#undef jb_err_no

/*               Error to ascii               */
const char *jbetoa(int a)
{
	if (a < 0)
	{
		if (jberrno == ERROR_JCONFIG_FILE_OPEN)
			return "Unable to open configuration file ( JConfig )."      ;
		if (jberrno == ERROR_JOBLIST_FILE_OPEN)
			return "Unable to open job list file. <include filename?>"   ;
		if (jberrno == ERROR_RECURSING_JOB_DIR)
		    return "A problem has occurred while performing backup. : "   ;
		if (jberrno == ERROR_SETTING_TIME_BUFF)
		    return "There was a problem updating the time buffer."       ;
		if (jberrno == ERROR_JLIST_FILE_UPDATE)
		    return "An error has occurred updating a job file."           ;
	}
     return "no error";
}

