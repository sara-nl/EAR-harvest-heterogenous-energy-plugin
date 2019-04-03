/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING
*/

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <mysql/mysql.h>
#include <common/output/verbose.h>
#include <common/string_enhanced.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/database/db_helper.h>

//#define ALL_QUERY   "DELETE Learning_signatures, Learning_applications, Learning_jobs FROM Learning_jobs " \
                    "INNER JOIN Learning_applications INNER JOIN Learning_signatures ON Learning_jobs.id=job_id " \
                    "AND Learning_jobs.step_id=Learning_applications.step_id AND Learning_signatures.id=signature_id"

#define ALL_QUERY1  "DELETE FROM Power_signatures WHERE id IN (SELECT power_signature_id from Learning_applications)"
#define ALL_QUERY2  "DELETE FROM Learning_applications"
#define ALL_QUERY3  "DELETE FROM Learning_signatures"
#define ALL_QUERY4  "DELETE FROM Learning_jobs"


#define NODE_QUERY  "DELETE Learning_signatures, Learning_applications, Learning_jobs FROM Learning_jobs " \
                    "INNER JOIN Learning_applications INNER JOIN Learning_signatures ON Learning_jobs.id=job_id " \
                    "AND Learning_jobs.step_id=Learning_applications.step_id AND Learning_signatures.id=signature_id "\
                    "WHERE node_id=\"%s\""

void main(int argc,char *argv[])
{
	int num_apps, is_learning=0;
    char passw[256];
    int total_apps = 0;
    char all_nodes = 0;
	application_t *apps;
	if (argc < 2) {
        printf("Requires an argument, either the node_name from which to delete records or [all] to delete all records. If the root user has a password, use -p\n");
        exit(0);
    }
    if (!strcmp(argv[1], "all")) all_nodes = 1;
    if (argc > 2)
    {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);

        printf("Introduce root's password:");
        fflush(stdout);
        fgets(passw, sizeof(passw), stdin);
        t.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        printf("\n");
        strclean(passw, '\n');
    }
    else
        strcpy(passw, "");

    char ear_path[256];
	cluster_conf_t my_conf;
    if (get_ear_conf_path(ear_path)==EAR_ERROR){
            printf("Error getting ear.conf path\n");
            exit(0);
    }
    read_cluster_conf(ear_path,&my_conf);
    char query[256];
    if (all_nodes)
        strcpy(query, ALL_QUERY1);
    else
        sprintf(query, NODE_QUERY, argv[1]);

    MYSQL *connection = mysql_init(NULL);
    
    if (connection == NULL)
    {
        verbose(0, "Error creating MYSQL object: %s", mysql_error(connection)); //error
        exit(1);
    }

    if(!mysql_real_connect(connection, my_conf.database.ip, "root", passw, my_conf.database.database, my_conf.database.port, NULL, 0))
    {
        verbose(0, "Error connecting to the database(%d):%s", mysql_errno(connection), mysql_error(connection)); //error
        mysql_close(connection);
        exit(0);
    }
    
//    printf("query: %s\n", query);
    if (all_nodes)
    {
        if (mysql_query(connection, query))
        {
            verbose(0, "MYSQL error(%d): %s", mysql_errno(connection), mysql_error(connection)); //error
            exit(0);
        }
        strcpy(query, ALL_QUERY2);
        if (mysql_query(connection, query))
        {
            verbose(0, "MYSQL error(%d): %s", mysql_errno(connection), mysql_error(connection)); //error
            exit(0);
        }
        strcpy(query, ALL_QUERY3);
        if (mysql_query(connection, query))
        {
            verbose(0, "MYSQL error(%d): %s", mysql_errno(connection), mysql_error(connection)); //error
            exit(0);
        }
        strcpy(query, ALL_QUERY4);
    }
    if (mysql_query(connection, query))
    {
        verbose(0, "MYSQL error(%d): %s", mysql_errno(connection), mysql_error(connection)); //error
        exit(0);
    }
    mysql_close(connection);
    if (all_nodes)
        printf("Successfully deleted all records.\n");
    else
        printf("Successfully deleted all records for node %s.\n", argv[1]);

}
