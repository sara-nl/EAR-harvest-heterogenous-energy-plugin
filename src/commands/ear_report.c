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
#define AGGREGATED 1

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/user.h>
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

#define ALL_USERS   "SELECT TRUNCATE(SUM(DC_power*time), 0) as energy, Jobs.user_id FROM " \
                    "Power_signatures INNER JOIN Applications On id=Applications.power_signature_id " \
                    "INNER JOIN Jobs ON job_id = Jobs.id AND Applications.step_id = Jobs.step_id " \
                    "WHERE start_time >= %d AND end_time <= %d GROUP BY Jobs.user_id ORDER BY energy"

#define ALL_NODES   "select SUM(DC_energy), node_id FROM Periodic_metrics WHERE start_time >= %d " \
                    " AND end_time <= %d GROUP BY node_id "

#define ALL_TAGS    "SELECT TRUNCATE(SUM(DC_power*time), 0) as energy, Jobs.e_tag FROM " \
                    "Power_signatures INNER JOIN Applications ON id=Applications.power_signature_id " \
                    "INNER JOIN Jobs ON job_id = Jobs.id AND Applications.step_id = Jobs.step_id " \
                    "WHERE start_time >= %d AND end_time <= %d GROUP BY Jobs.e_tag ORDER BY energy"
//grouped by query:

//SELECT TRUNCATE(SUM(DC_power*time),0) as energy, Jobs.user_id FROM Power_signatures INNER JOIN Applications ON id=Applications.power_signature_id INNER JOIN Jobs ON job_id = Jobs.id AND Applications.step_id = Jobs.step_id WHERE start_time >= 0 AND end_time <= 5555555555555 GROUP BY Jobs.user_id ORDER BY energy;


char *node_name = NULL;
char *user_name = NULL;
char *etag = NULL;
int verbose = 0;
unsigned long long avg_pow = 0;
time_t global_start_time = 0;
time_t global_end_time = 0;

void usage(char *app)
{
    verbose(0, "%s is a tool that reports energy consumption data", app);
	verbose(0, "Usage: %s [options]", app);
    verbose(0, "Options are as follows:\n"\
            "\t-s start_time     \t indicates the start of the period from which the energy consumed will be computed. Format: YYYY-MM-DD. Default 1970-01-01.\n"
            "\t-e end_time       \t indicates the end of the period from which the energy consumed will be computed. Format: YYYY-MM-DD. Default: current time.\n"
            "\t-n node_name |all \t indicates from which node the energy will be computed. Default: none (all nodes computed) \n\t\t\t\t 'all' option shows all users individually, not aggregated.\n"
            "\t-u user_name |all \t requests the energy consumed by a user in the selected period of time. Default: none (all users computed). \n\t\t\t\t 'all' option shows all users individually, not aggregated.\n"
            "\t-t energy_tag|all \t requests the energy consumed by energy tag in the selected period of time. Default: none (all tags computed). \n\t\t\t\t 'all' option shows all tags individually, not aggregated.\n"
            "\t-h                \t shows this message.");
	exit(1);
}


long long stmt_error(MYSQL_STMT *statement)
{
    verbose(0, "Error preparing statement (%d): %s",
            mysql_stmt_errno(statement), mysql_stmt_error(statement));
    mysql_stmt_close(statement);
    return -1;
}

unsigned long long get_sum(MYSQL *connection, int start_time, int end_time, unsigned long long divisor)
{

    MYSQL_STMT *statement = mysql_stmt_init(connection);
    if (!statement)
    {
        verbose(0, "Error creating statement (%d): %s", mysql_errno(connection),
                mysql_error(connection)); //error
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
#if AGGREGATED
    else strcpy(query, AGGR_QUERY);
#else
    else strcpy(query, SUM_QUERY);
#endif

    if (verbose) {
        verbose(0, "QUERY: %s", query);
    }
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
    unsigned long long result = 0;
    MYSQL_BIND res_bind[1];
    memset(res_bind, 0, sizeof(res_bind));
    res_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    res_bind[0].is_unsigned = 1;
    res_bind[0].buffer = &result;

    if (mysql_stmt_bind_param(statement, bind)) return stmt_error(statement);
    if (mysql_stmt_bind_result(statement, res_bind)) return stmt_error(statement);
    if (mysql_stmt_execute(statement)) return stmt_error(statement);
    if (mysql_stmt_store_result(statement)) return stmt_error(statement);

    int status = mysql_stmt_fetch(statement);
    if (status != 0 && status != MYSQL_DATA_TRUNCATED)
        result = -2;

    if (mysql_stmt_free_result(statement)) {
        verbose(0, "ERROR when freing result."); //error
    }

    if (mysql_stmt_close(statement)) {
        verbose(0, "\nERROR when freeing statement"); //error
    }

    return result;

}

void compute_pow(MYSQL *connection, int start_time, int end_time, unsigned long long result)
{
    char query[256];
    if (user_name == NULL && etag == NULL)
    {
        MYSQL_STMT *statement = mysql_stmt_init(connection);
        if (!statement)
        {
            verbose(0, "Error creating statement (%d): %s", mysql_errno(connection),
                    mysql_error(connection));
            return;
        }

        if (node_name != NULL && strcmp(node_name, "all"))
        {
            strcpy(query, MET_TIME);
            strcat(query, " AND node_id='");
            strcat(query, node_name);
            strcat(query, "'");
        }
        else
            strcpy(query, MET_TIME);

        if (verbose) {
            verbose(0, "QUERY: %s", query);
        }

        if (mysql_stmt_prepare(statement, query, strlen(query)))
        {
            avg_pow = stmt_error(statement);
            return;
        }
        //time_t start, end; 
        MYSQL_BIND sec_bind[2];
        memset(sec_bind, 0, sizeof(sec_bind));
        sec_bind[0].buffer_type = sec_bind[1].buffer_type = MYSQL_TYPE_LONG;
        sec_bind[0].buffer = (char *)&start_time;
        sec_bind[1].buffer = (char *)&end_time;

        MYSQL_BIND sec_res[2];
        memset(sec_res, 0, sizeof(sec_res));
        sec_res[0].buffer_type = sec_res[1].buffer_type = MYSQL_TYPE_LONGLONG;
        sec_res[0].buffer = &global_start_time;
        sec_res[1].buffer = &global_end_time;
        
        if (mysql_stmt_bind_param(statement, sec_bind))
        {
            avg_pow = stmt_error(statement);
            return;
        }
        if (mysql_stmt_bind_result(statement, sec_res))
        {
            avg_pow = stmt_error(statement);
            return;
        }
        if (mysql_stmt_execute(statement))
        {
            avg_pow = stmt_error(statement);
            return;
        }
        if (mysql_stmt_store_result(statement))
        {
            avg_pow = stmt_error(statement);
            return;
        }
        int status = mysql_stmt_fetch(statement);
        if (status != 0 && status != MYSQL_DATA_TRUNCATED)
            avg_pow = -2;

        char sbuff[64], ebuff[64];
        if (verbose)
        {
            verbose(0, "original  \t start_time: %d\t end_time: %d\n", start_time, end_time);
            verbose(0, "from query\t start_time: %d\t end_time: %d\n", global_start_time, global_end_time);
            verbose(0, "from query\t start_time: %s\t end_time: %s\n",
                    ctime_r(&global_start_time, sbuff), ctime_r(&global_end_time, ebuff));
            verbose(0, "result: %llu", result);
        }
        if (global_start_time != global_end_time && result > 0)
            avg_pow = result / (global_end_time-global_start_time);
    
        if (verbose)
        {
            verbose(0, "avg_pow after computation: %lu", avg_pow);
            verbose(0, "end-start: %d", global_end_time-global_start_time);
        }

        mysql_stmt_close(statement);
    }

}

void print_all(MYSQL *connection, int start_time, int end_time, char *inc_query)
{
    char query[512];
    int i;
    char all_nodes = 0;

    sprintf(query, inc_query, start_time, end_time);

    if (verbose) {
        verbose(0, "query: %s", query);
    }
    
    if (mysql_query(connection, query))
    {
        verbose(0, "MYSQL error");
        return;
    }
    MYSQL_RES *result = mysql_store_result(connection);
  
    if (result == NULL) 
    {
        verbose(0, "MYSQL error");
        return;
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
    if (!strcmp(inc_query, ALL_USERS)) {
        verbose(0, "%15s %15s", "Energy (J)", "User");
    } else if (!strcmp(inc_query, ALL_TAGS)) {
        verbose(0, "%15s %15s", "Energy (J)", "Energy tag");
    } else if (global_end_time > 0) {
        verbose(0, "%15s %15s %15s", "Energy (J)", "Node", "Avg. Power");
        all_nodes = 1;
    }
    else {
        verbose(0, "%15s %15s", "Energy (J)", "Node");
    }


    while ((row = mysql_fetch_row(result))!= NULL) 
    { 
        for(i = 0; i < num_fields; i++) {
            verbosen(0, "%15s ", row[i] ? row[i] : "NULL");
        }
          
        if (row[0] && all_nodes) { //when getting energy we compute the avg_power
            verbosen(0, "%15d", (atoll(row[0]) /(global_end_time - global_start_time)));
	}

        verbose(0, " ");
    }
    mysql_free_result(result);
}

void main(int argc,char *argv[])
{
    char path_name[256];
    cluster_conf_t my_conf;
    time_t start_time = 0;
    time_t end_time = time(NULL);
    int divisor = 1;
    int opt;
    char all_users = 0;
    char all_nodes = 0;
    char all_tags = 0;
    struct tm tinfo = {0};

    if (get_ear_conf_path(path_name) == EAR_ERROR)
    {
        verbose(0, "Error getting ear.conf path."); //error
        exit(1);
    }

    if (read_cluster_conf(path_name, &my_conf) != EAR_SUCCESS) {
        verbose(0, "Error reading configuration"); //error
    }
    
    if (getuid() != 0 && !is_privileged_command(&my_conf))
    {
        verbose(0, "This command can only be executed by privileged users. Contact your admin for more info.");
        free_cluster_conf(&my_conf);
        exit(1); //error
    }

    MYSQL *connection = mysql_init(NULL);
    if (!connection)
    {
        verbose(0, "Error creating MYSQL object"); //error
        free_cluster_conf(&my_conf);
        exit(1);
    }

    if (strlen(my_conf.database.user_commands) < 1) 
        verbose(0, "Warning: commands' user is not defined in ear.conf");

    if (!mysql_real_connect(connection, my_conf.database.ip, my_conf.database.user_commands, my_conf.database.pass_commands, my_conf.database.database, my_conf.database.port, NULL, 0))
    {
        verbose(0, "Error connecting to the database (%d): %s",
                mysql_errno(connection), mysql_error(connection)); //error
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
                if (!strcmp(node_name, "all"))
                    all_nodes=1;
                break;
            case 'u':
                user_name = optarg;
                if (!strcmp(user_name, "all"))
                    all_users=1;
                break;
            case 't':
                etag = optarg;
                if (!strcmp(etag, "all"))
                    all_tags=1;
                break;
            case 'e':
                if (strptime(optarg, "%Y-%m-%e", &tinfo) == NULL)
                {
                    verbose(0, "Incorrect time format. Supported format is YYYY-MM-DD"); //error
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
                    verbose(0, "Incorrect time format. Supported format is YYYY-MM-DD"); //error
                    mysql_close(connection);
                    free_cluster_conf(&my_conf);
                    exit(1);
                    break;
                }
                start_time = mktime(&tinfo);
                break;
        }
    }

    if (!all_users && !all_nodes && !all_tags)
    {
        unsigned long long result = get_sum(connection, start_time, end_time, divisor);
        compute_pow(connection, start_time, end_time, result);
    
        if (!result) {
            verbose(0, "No results in that period of time found");
        } else if (result < 0)
        {
            verbose(0, "Error querying the database."); //error
            exit(1);
        }
        else
        {
            char sbuff[64], ebuff[64];
            strtok(ctime_r(&end_time, ebuff), "\n");
            strtok(ctime_r(&start_time, sbuff), "\n");
            verbose(0, "Total energy spent from %s to %s: %llu J", sbuff, ebuff, result);
            if (avg_pow <= 0)
            {
                if (user_name == NULL && etag == NULL && verbose)
                    verbose(0, "Error when reading time info from database, could not compute average power."); //error
            }
            else if (avg_pow > 0) {
                verbose(0, "Average power during the period: %lu W", avg_pow);
            }
        }    
    }
    else if (all_users)
        print_all(connection, start_time, end_time, ALL_USERS);
    else if (all_nodes)
    {
        compute_pow(connection, start_time, end_time, 0);
        print_all(connection, start_time, end_time, ALL_NODES);
    }
    else if (all_tags)
        print_all(connection, start_time, end_time, ALL_TAGS);
    
    mysql_close(connection);
    free_cluster_conf(&my_conf);


    exit(1);
}




