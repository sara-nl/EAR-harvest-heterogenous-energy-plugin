/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, dynamic and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*   
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*   
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING  
*/

#define _XOPEN_SOURCE 700 //to get rid of the warning
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <common/config.h>

#if DB_MYSQL
#include <mysql/mysql.h>
#define SUM_QUERY   "SELECT SUM(dc_energy)/? FROM Report.Periodic_metrics WHERE start_time" \
                    ">= ? AND end_time <= ?"
#endif

void usage(char *app)
{
	printf("Usage: %s start_time end_time [options]\n", app);
	exit(1);
}

#if DB_MYSQL

long long stmt_error(MYSQL_STMT *statement)
{
    fprintf(stderr, "Error preparing statement (%d): %s\n", 
            mysql_stmt_errno(statement), mysql_stmt_error(statement));
    mysql_stmt_close(statement);
    return -1;
}

long long get_sum(MYSQL *connection, int start_time, int end_time, unsigned long long divisor)
{

    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        fprintf(stderr, "Error creating statement (%d): %s\n", mysql_errno(connection), 
                mysql_error(connection));
        return -1;
    }
    
    if (mysql_stmt_prepare(statement, SUM_QUERY, strlen(SUM_QUERY)))
                                                return stmt_error(statement);


    //Query parameters binding
    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].is_unsigned = 1;

    bind[1].buffer_type = bind[2].buffer_type = MYSQL_TYPE_LONG;

    bind[0].buffer = (char *)&divisor;
    bind[1].buffer = (char *)&start_time;
    bind[2].buffer = (char *)&end_time;


    //Result parameters
    MYSQL_BIND res_bind[1];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    long long result = 0;
    res_bind[0].buffer = &result;

    if (mysql_stmt_bind_param(statement, bind)) return stmt_error(statement);
    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(statement);
    if (mysql_stmt_execute(statement)) return stmt_error(statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
        result = -2;

    mysql_stmt_close(statement);
    return result;


}
#endif


void main(int argc,char *argv[])
{
    if (argc < 3) usage(argv[0]);

#if DB_MYSQL
    char *db_ip = "127.0.0.1";
    char *db_user = "root";
    char *db_pass = "";
    

    unsigned long long divisor = 1;
    int i;
    for (i = 3; i < argc -1; i++)
    {
        if (!strcmp(argv[i], "-d"))
            divisor = (atoi(argv[i+1]) > 0) ? atoi(argv[i+1]) : divisor;
        if (!strcmp(argv[i], "-u"))
            db_user = argv[i+1];
        if (!strcmp(argv[i], "-db"))
            db_ip = argv[i+1];
        if (!strcmp(argv[i], "-p"))
            db_pass = argv[i+1];
          
    }
    printf("User: %s\n", db_user);
    struct tm *start={0}, *end={0}; 
    time_t start_time, end_time;
    
    start = getdate(argv[1]);
    if (start == NULL)
    {
     //   printf("getdate error %d\n", getdate_err);
        if (getdate_err == 7) fprintf(stderr, "Usupported date input. Please check your $DATEMSK file to see your defined inputs. \
                                                If the environment variable is not set, please set it to a valid format file. \n");
        else fprintf(stderr, "Error with date format.\n");
        exit(1);
    }
    start_time = mktime(start);
   
    end = getdate(argv[2]);
    end_time = mktime(end);
    
    MYSQL *connection = mysql_init(NULL);
    if (!connection)
    {
        fprintf(stderr, "Error creating MYSQL object\n");
        exit(1);
    }

    if (!mysql_real_connect(connection, db_ip, db_user, db_pass, NULL, 0, NULL, 0))
    {
        fprintf(stderr, "Error connecting to the database (%d) :%s\n",
                mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        exit(1);
    }
    
    long long result = get_sum(connection, start_time, end_time, divisor);

    mysql_close(connection);
    
    if (!result) printf("No results in that period of time found\n");
    else if (result < 0) exit(1);
    else printf("Total energy spent: %lli\n", result);
    

#endif

    exit(1);
}




