/* err.h
 Ethan Laur
 3/25/14
 */
#ifndef err_h
#define err_h

#define ERROR_JCONFIG_FILE_OPEN 10
#define ERROR_JOBLIST_FILE_OPEN 11
#define ERROR_JLIST_FILE_UPDATE 12
#define ERROR_LOADTIME_CONVERT  20
#define ERROR_CURRTIME_CONVERT  21
#define ERROR_RECURSING_JOB_DIR 30
#define ERROR_SETTING_TIME_BUFF 35

#ifndef jb_err_no
/*      For the errors      */
extern char jberrno;
#endif

const char *jbetoa( int a = -1 );

#endif
