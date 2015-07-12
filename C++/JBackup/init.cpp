#include "init.h"
#include  "err.h"
#include "copy.h"

/*              Main Body                   */
int main(int argc, char *argv[])
{
    /*          PRINTF OVERRIDE                */
    if (LOG_MODE)
    {
        FILE *log_file;
        if((log_file=freopen("JBUP_LOG_FILE", "a" ,stdout))==NULL)
        {
            printf("Cannot open LOG.\n");
            exit(1);
        }
    } // END IF LOG_MODE
    
    jobs jlist [MAX_JOBS];
    
    int result = 0;
    int number_of_jobs = 0;
    
    number_of_jobs = loadJobs( jlist ); 
    
    result = doJobs( jlist, number_of_jobs );

    if(BUGS) printf("\n\n\tNumber of jobs loaded : %i \n\n", number_of_jobs);

    /*       DEV C++     */
    system("PAUSE");
    return EXIT_SUCCESS;
}

/*              Build Job List               */
int loadJobs ( jobs jlist[] )
{
     int x           = 0     ;
     int line_count  = 0     ;
     char job_file[MAX_NAME] ;
     char    temp[MAX_FPATH] ;
     jberrno = 0;
     
     //   Goes through JConfig for a list of jobs by location
     FILE *jlist_fp = fopen( JOB_LIST , "r" );     

     if (jlist_fp == NULL)
     {
            x = MAX_JOBS;
            jberrno = ERROR_JCONFIG_FILE_OPEN ;
            
            if(BUGS) printf("\n%s\n", jbetoa() );
     }

     //   Go Through each job, and load. | x Ensures < MAX_JOBS and exits if error
     while ( fgets( job_file, sizeof(job_file), jlist_fp) && x < MAX_JOBS )
     {
           char *blarg1 = strstr(job_file, "\n");
           if (blarg1 != NULL) *blarg1 = 0;
           // Open the file pulled from JConfig
            FILE *job_fp = fopen( job_file , "r" );
            
            if (job_fp == NULL)
            {
                         line_count = MAX_LINES;
                         jberrno = ERROR_JOBLIST_FILE_OPEN ;
                         if(BUGS) printf("\n%s : %s\n", jbetoa(), job_file);
            }
            else 
            {
            // Store the job file for later in struct
                  strcpy(jlist[ x ].jobfile, job_file);              
            }
            // line_count Handles the placement of data into the structure
            while ( fgets( temp, sizeof(temp), job_fp) && line_count <= MAX_LINES)
            {
                  char *blarg = strstr(temp, "\n");
                  if (blarg != NULL) *blarg = 0;
                  if ( line_count == 0 ){ strcpy( jlist[ x ].title,     temp ) ; }
                  if ( line_count == 1 ){ strcpy( jlist[ x ].from,      temp ) ; }
                  if ( line_count == 2 ){ strcpy( jlist[ x ].to,        temp ) ; }
                  if ( line_count == 3 ){ strcpy( jlist[ x ].dateTime,  temp ) ; }
                  line_count++;
            }
            fclose(job_fp);
            fflush(job_fp);
            line_count = 0;
            
            if(BUGS) printf("\nTitle: %s\nFrom: %s\nTo: %s\nDateTime: %s\n",
                            jlist[x].title,jlist[x].from,jlist[x].to,jlist[x].dateTime);
            x++;
     }
     fclose(jlist_fp);
     fflush(jlist_fp);
     return x;
}

/*          Execute Jobs in List               */

#define TST_SC "C:\\TESTING\\SOURCE"
#define TST_DS "C:\\TESTING\\DESTINATION"

char doJobs( jobs jlist[], int number_of_jobs )
{    
    int result, check;
    int index = 0;
    int mt, d, y, h, m, s, update_index;
    char time_update_buff [strlen(jlist[ index ].dateTime)];
    
    time_t epoch_time;
    struct tm *tm_p = NULL;
    epoch_time = time( NULL );
    tm_p = localtime( &epoch_time );
 
    jberrno = 0;
    
    for ( index; index < number_of_jobs; index++)
    {
        // If the job needs to be executed -> do it
        if( shouldExecute( jlist[ index ].dateTime ))
        {
            result = recursiveCopy( jlist[ index ].from, jlist[ index ].to, 1);
        }
        else
        {
            if (BUGS)
                printf("\nShould not be ran today.\n");
        }

        // If something went wrong with the backup, notify. 
        if ( result )
        {
            jberrno = ERROR_RECURSING_JOB_DIR;
            if(BUGS) printf("\n%s %s\n\n", jbetoa(),  jlist[ index ].title);
        }
        else
        {
            // UPDATE MONTH / DAY to next update period.
            // IF MONTH > -1 : ++month - leave day alone
            // IF MONTH == -1 && day > -1 ++day
            // THIS IS ALL ASSUMING THAT ITS NOT A YEARLY BACKUP, AND 
            // THAT HOUR/MIN/SEC DOESNT CHANGE
            // TURN ALL TO INTS FOR LATER IF NEEDS TO BE MODIFIED
            
            sscanf(jlist[ index ].dateTime, 
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR"%d",
            &y, &mt, &d, &h, &m, &s, &update_index);
            
            // NEED TO BETTER MANAGE THE INCRIMENTING OF DAYS
            if( mt > -1 )
            {
                if( mt+update_index > 12) // If its larger than 12
                {
                     mt = (mt+update_index)%12; //set num months larger.
                }
                else
                {
                    tm_p->tm_mon += update_index+1;     // Adds index to mon, Leave the +1
                    mt = tm_p->tm_mon;
                }
            }
            if( mt == -1 && d > -1 )                    // If mon not given, assume daily
            {
                if ( tm_p->tm_mday >= 28 )              // not quite monthly
                    tm_p->tm_mday = 0;
                tm_p->tm_mday += update_index;
                d = tm_p->tm_mday;
            }
            // Set time_update_buff before updating job file
            check = sprintf(time_update_buff,
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR"%d",
            y, mt, d, h, m, s, update_index);
                
            if ( check < 0 )
            {
                jberrno = ERROR_SETTING_TIME_BUFF;
                if(BUGS) printf("\n%s\n", jbetoa());
                return -1;   
            }
            else
            {
                strcpy(jlist[ index ].dateTime, time_update_buff);
                updateJobFile( jlist, index );
            }
        }   // END OF RESULT ELSE
    }   // END JOB LOOP
	return 0;
}

char updateJobFile(jobs jlist[], int index )
{
    /*      Update the file for specific job        */

    jberrno = 0;
    FILE *out = fopen( jlist[ index ].jobfile, "w+" );
    
    if( out == NULL )
    {
        jberrno = ERROR_JLIST_FILE_UPDATE;
        if(BUGS) printf("\n%s\n", jbetoa());
        return 0;    
    }

    fprintf(out,"%s\n%s\n%s\n%s\n",
    jlist[ index ].title,
    jlist[ index ].from,
    jlist[ index ].to,
    jlist[ index ].dateTime
    );

    fclose(out);
    
    if(BUGS) printf(" \n\nUpdated File Info : \nTitle: %s\nFrom: %s\nTo: %s\nDateTime: %s\n",
                    jlist[index].title,jlist[index].from,jlist[index].to,jlist[index].dateTime);
    return 1;
}

//format: yr mo da ho mi se
//should all be integers, ascii encoded, if all fields should be true
///within that range, specify -1 instead. ranges are disallowed

char shouldExecute(char *tspec)
{
        //make sure there are no comments on the following line
        //MUST be inside double quotes!
        time_t epoch_time;
        struct tm *tm_p = NULL;
        int mt, d, y, h, m, s;
        epoch_time = time( NULL );
        tm_p = localtime( &epoch_time );
        sscanf(tspec, "%d"TIME_SEPARATOR"%d"
        TIME_SEPARATOR"%d"TIME_SEPARATOR"%d"
        TIME_SEPARATOR"%d"TIME_SEPARATOR"%d",
                  &y, &mt, &d, &h, &m, &s);
        if (y == tm_p->tm_year || y == -1)
                if ((mt - 1) == (tm_p->tm_mon + 0) || mt == -1)
                        if (d == tm_p->tm_mday || d == -1)
                                if (h == tm_p->tm_hour || h == -1)
                                        if (m == tm_p->tm_min || m == -1)
                                                if (s == tm_p->tm_sec || s == -1) 
                                                       return 1;
        return 0;
}
