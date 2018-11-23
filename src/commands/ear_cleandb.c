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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <mysql/mysql.h>
#include <common/config.h>
#include <common/database/db_helper.h>
#include <common/types/configuration/cluster_conf.h>

int EAR_VERBOSE_LEVEL = 1;

#define CLEAN_PERIODIC_QUERY "DELETE FROM Periodic_metrics WHERE end_time <= UNIX_TIMESTAMP(SUBTIME(NOW(),'31 0:0:0.00'))"

void usage(char *app)
{
	printf("Usage:%s [options]\n",app);
    printf("\t-p\t\tSpecify the password for MySQL's root user.\n");
	exit(0);
}
  
void main(int argc,char *argv[])
{
    char passw[256];
    if (argc > 2) usage(argv[0]);
    else if (argc == 2)
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
        strclean(passw, '\n');
        printf("\n");
    }
    else
        strcpy(passw, "");

    MYSQL *connection = mysql_init(NULL); 

    if (connection == NULL)
    {
        fprintf(stderr, "Error creating MYSQL object: %s \n", mysql_error(connection));
        exit(1);
    }

    cluster_conf_t my_cluster;
    char ear_path[256];
    if (get_ear_conf_path(ear_path) == EAR_ERROR)
    {
        fprintf(stderr, "Error getting ear.conf path\n");
        exit(0);
    }

    read_cluster_conf(ear_path, &my_cluster);

    init_db_helper(&my_cluster.database);

    db_run_query(CLEAN_PERIODIC_QUERY, "root", passw); 

    free_cluster_conf(&my_cluster);

    printf("Database successfully cleaned.\n");

    exit(1);
}
