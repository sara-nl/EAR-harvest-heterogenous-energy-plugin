/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/states.h>
#include <common/config.h>
#include <common/types/configuration/cluster_conf.h>

#include <mysql/mysql.h>
#define SUM_QUERY   "SELECT SUM(dc_energy)/? FROM Periodic_metrics WHERE start_time" \
                    ">= ? AND end_time <= ?"

#define MET_TIME    "SELECT MIN(start_time), MAX(end_time) FROM Periodic_metrics WHERE start_time" \
                    ">= ? AND end_time <= ?"

#define AGGR_TIME   "SELECT MIN(start_time), MAX(end_time) FROM Periodic_aggregations WHERE start_time" \
                    ">= ? AND end_time <= ?"

#define AGGR_QUERY  "SELECT SUM(dc_energy)/? FROM Periodic_aggregations WHERE start_time" \
                    ">= ? AND end_time <= ?"

#define USER_QUERY  "SELECT SUM(DC_power*time)/? FROM Power_signatures WHERE id IN " \
                    "(SELECT Applications.power_signature_id FROM Applications JOIN Jobs " \
                    "ON job_id = id AND Applications.step_id = Jobs.step_id WHERE "\
                    "Jobs.user_id = '%s' AND start_time >= ? AND end_time <= ?)"

#define ETAG_QUERY  "SELECT SUM(DC_power*time)/? FROM Power_signatures WHERE id IN " \
                    "(SELECT Applications.power_signature_id FROM Applications JOIN Jobs " \
                    "ON job_id = id AND Applications.step_id = Jobs.step_id WHERE "\
                    "Jobs.e_tag = '%s' AND start_time >= ? AND end_time <= ?)"


char *node_name = NULL;
char *user_name = NULL;
char *etag = NULL;
int EAR_VERBOSE_LEVEL = 0;
int verbose = 0;
int avg_pow = -1;

void usage(char *app)
{
	printf("Usage: %s [options]\n", app);
    printf("Options are as follows:\n"\
            "\t-s start_time\t indicates the start of the period from which the energy consumed will be computed. Format: YYYY-MM-DD. Default 1970-01-01.\n"
            "\t-e ent_time  \t indicates the end of the period from which the energy consumed will be computed. Format: YYYY-MM-DD. Default: current time.\n"
            "\t-n node_name \t indicates from which node the energy will be computed. Default: none (all nodes computed)\n"
            "\t-u user_name \t requests the energy consumed by a user in the selected period of time. Default: none (all users computed)\n"
            "\t-h           \t shows this message.\n");
	exit(1);
}


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
    
    char query[256];

    if (node_name != NULL)
    {
        strcpy(query, SUM_QUERY);
        strcat(query, " AND node_id='");
        strcat(query, node_name);
        strcat(query, "'");
    }
    else if (user_name != NULL)
    {
        sprintf(query, USER_QUERY, user_name);
    }
    else if (etag != NULL)
    {
        sprintf(query, ETAG_QUERY, etag);
    }
    else strcpy(query, AGGR_QUERY);

    if (verbose) printf("QUERY: %s\n", query);
    if (mysql_stmt_prepare(statement, query, strlen(query)))
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



    if (user_name == NULL && etag == NULL)
    {
        MYSQL_STMT *statement2 = mysql_stmt_init(connection);
        if (!statement2)
        {
            fprintf(stderr, "Error creating statement (%d): %s\n", mysql_errno(connection), 
                    mysql_error(connection));
            return result;
        }

        if (node_name != NULL)
        {
            strcpy(query, MET_TIME);
            strcat(query, " AND node_id='");
            strcat(query, node_name);
            strcat(query, "'");
        }
        else
            strcpy(query, AGGR_TIME);

        if (verbose) printf("QUERY: %s\n", query);
        if (mysql_stmt_prepare(statement2, query, strlen(query)))
        {
            avg_pow = stmt_error(statement2);
            return result;
        }
        int start, end; 
        MYSQL_BIND sec_bind[2];
        memset(sec_bind, 0, sizeof(sec_bind));
        sec_bind[0].buffer_type = sec_bind[1].buffer_type = MYSQL_TYPE_LONG;
        sec_bind[0].buffer = (char *)&start_time;
        sec_bind[1].buffer = (char *)&end_time;

        MYSQL_BIND sec_res[2];
        memset(sec_res, 0, sizeof(sec_res));
        sec_res[0].buffer_type = sec_res[1].buffer_type = MYSQL_TYPE_LONGLONG;
        sec_res[0].buffer = &start;
        sec_res[1].buffer = &end;
        
        if (mysql_stmt_bind_param(statement2, sec_bind))
        {
            avg_pow = stmt_error(statement2);
            return result;
        }
        if (mysql_stmt_bind_result(statement2, sec_res))
        {
            avg_pow = stmt_error(statement2);
            return result;
        }
        if (mysql_stmt_execute(statement2))
        {
            avg_pow = stmt_error(statement2);
            return result;
        }
        if (mysql_stmt_store_result(statement2))
        {
            avg_pow = stmt_error(statement2);
            return result;
        }
        //status = mysql_stmt_fetch(statement2);
        /*if (status != 0 && status != MYSQL_DATA_TRUNCATED)
            avg_pow = -2;*/

        if (verbose)
            printf("current avg_pow value: %d\n", avg_pow);
        if (verbose)
            printf("\nresult: %d\t end: %d\t start: %d\n\n", result, end, start);
        if (verbose)
            printf("start time computed: %d\t end time computed: %d\n\n", start_time, end_time);
        if (start != end)
            avg_pow = result / (end-start);
    
        mysql_stmt_close(statement2);
    
    }
    return result;


}


void main(int argc,char *argv[])
{
    char path_name[256];
    cluster_conf_t my_conf;
    time_t start_time = 0;
    time_t end_time = time(NULL);
    int divisor = 1000;
    int flags, opt;
    struct tm tinfo = {0};

    if (get_ear_conf_path(path_name) == EAR_ERROR)
    {
        fprintf(stderr, "Error getting ear.conf path.\n");
        exit(1);
    }

    if (read_cluster_conf(path_name, &my_conf) != EAR_SUCCESS) fprintf(stderr, "Error reading configuration");
    
    char *user = getlogin();
    if (user == NULL)
    {
        fprintf(stderr, "Error getting username, cannot verify identity of user executing the command. Exiting...\n");
        free_cluster_conf(&my_conf);
        exit(1);
    }

    MYSQL *connection = mysql_init(NULL);
    if (!connection)
    {
        fprintf(stderr, "Error creating MYSQL object\n");
        free_cluster_conf(&my_conf);
        exit(1);
    }

    if (!mysql_real_connect(connection, my_conf.database.ip, my_conf.database.user, "", my_conf.database.database, 0, NULL, 0))
    {
        fprintf(stderr, "Error connecting to the database (%d) :%s\n",
                mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        free_cluster_conf(&my_conf);
        exit(1);
    }

    while ((opt = getopt(argc, argv, "t:vhn:u:s:e:")) != -1)
    {
        switch(opt)
        {
            case 'h':
                usage(argv[0]);
                mysql_close(connection);
                free_cluster_conf(&my_conf);
                break;
            case 'v':
                verbose=1;
                break;
            case 'n':
                node_name = optarg;
                break;
            case 'u':
                user_name = optarg;
                break;
            case 't':
                etag = optarg;
                break;
            case 'e':
                if (strptime(optarg, "%Y-%m-%e", &tinfo) == NULL)
                {
                    fprintf(stderr, "Incorrect time format. Supported format is YYYY-MM-DD\n");
                    mysql_close(connection);
                    free_cluster_conf(&my_conf);
                    exit(1);
                    break;
                }
                end_time = mktime(&tinfo);
                break;
            case 's':
                if (strptime(optarg, "%Y-%m-%e", &tinfo) == NULL)
                {
                    fprintf(stderr, "Incorrect time format. Supported format is YYYY-MM-DD\n");
                    mysql_close(connection);
                    free_cluster_conf(&my_conf);
                    exit(1);
                    break;
                }
                start_time = mktime(&tinfo);
                break;
        }
    }


    long long result = get_sum(connection, start_time, end_time, divisor);

    mysql_close(connection);
    free_cluster_conf(&my_conf);
    
    if (!result) printf("No results in that period of time found\n");
    else if (result < 0) 
    {
        fprintf(stderr, "Error querying the database.\n");
        exit(1);
    }
    else
    {
        char sbuff[20], ebuff[20];
        strtok(ctime_r(&end_time, ebuff), "\n");
        strtok(ctime_r(&start_time, sbuff), "\n");
        printf("Total energy spent from %s to %s: %lli J\n", sbuff, ebuff, result);
        if (avg_pow < 0)
        {
            if (user_name == NULL && etag == NULL)
                fprintf(stderr, "Error when reading time info from database, could not compute average power.\n");
        }
        else
            printf("Average power during the period: %d W\n", avg_pow);
    }    


    free_cluster_conf(&my_conf);
    exit(1);
}




