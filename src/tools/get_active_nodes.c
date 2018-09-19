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


#include <stdio.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <common/database/db_helper.h>
#include <common/types/configuration/cluster_conf.h>

#define NODE_QUERY "SELECT distinct(node_id) from Periodic_metrics where end_time >= (unix_timestamp(now())-%d)"

int EAR_VERBOSE_LEVEL=1;
int verbose = 0;
const char *__NAME__ = "ear_event_print";

void show_query_result(cluster_conf_t my_conf, char *query)
{
    int i;
    MYSQL *connection = mysql_init(NULL);


    if (verbose) printf("query: %s\n", query);
    
    if (connection == NULL)
    {
        fprintf(stderr, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }

    if(!mysql_real_connect(connection, my_conf.database.ip, my_conf.database.user,"", my_conf.database.database, my_conf.database.port, NULL, 0))
    {
        fprintf(stderr, "Error connecting to the database(%d):%s\n", mysql_errno(connection), mysql_error(connection));
        mysql_close(connection);
        exit(0);
    }
    
    if (mysql_query(connection, query))
    {
        fprintf(stderr, "MYSQL error\n"); 
        return;
    }
    
    MYSQL_RES *result = mysql_store_result(connection);
  
    if (result == NULL) 
    {
        fprintf(stderr, "MYSQL error\n"); 
        return;
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;

    printf("%15s\n", "NODE ID");
    while ((row = mysql_fetch_row(result))!= NULL) 
    { 
        for(i = 0; i < num_fields; i++) 
        {
            printf("%15s ", row[i] ? row[i] : "NULL"); 
        }

        printf("\n"); 
    }
    mysql_free_result(result);
}

void main(int argc,char *argv[])
{
    char path_name[256];
    cluster_conf_t my_conf;

    if (get_ear_conf_path(path_name)==EAR_ERROR){
        printf("Error getting ear.conf path\n");
        exit(1);
    }

    if (read_cluster_conf(path_name, &my_conf) != EAR_SUCCESS) VERBOSE_N(0, "ERROR reading cluster configuration\n");
    
    char *user = getlogin();
    if (user == NULL)
    {
        fprintf(stderr, "ERROR getting username, cannot verify identity of user executing the command. Exiting...\n");
        free_cluster_conf(&my_conf);
        exit(1);
    }
    else if (is_privileged(&my_conf, user, NULL, NULL) || getuid() == 0)
    {
        user = NULL; //by default, privilegd users or root will query all user jobs
    }

    char query[256];
    sprintf(query, NODE_QUERY, (my_conf.db_manager.insr_time * 2));

    show_query_result(my_conf, query); 
    free_cluster_conf(&my_conf);
}
