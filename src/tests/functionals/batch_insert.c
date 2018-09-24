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
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <common/config.h>
#include <common/states.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/database/db_helper.h>
#include <common/database/mysql_io_functions.h>

int EAR_VERBOSE_LEVEL = 0;

void main(int argc,char *argv[])
{
    int verbose = 0;
    int i;

	cluster_conf_t my_cluster;
	char ear_path[256];
	if (get_ear_conf_path(ear_path)==EAR_ERROR){
		printf("Error getting ear.conf path\n");
		exit(0);
	}
	read_cluster_conf(ear_path,&my_cluster);

    
    int num_apps = 0;

    char query[256];
    
    int job_id, step_id = 0;
    char is_learning = 0;
    char *token;
 
    if (argc < 2) 
        num_apps = 35000;
    else
        num_apps = atoi(argv[1]);

    application_t * apps = calloc(num_apps, sizeof(application_t));

    int base = time(NULL);
    for (i = 0; i < num_apps; i++)
    {
        apps[i].job.id = base;
        apps[i].job.step_id = i;
    }
    printf("num_apps: %d found\n", num_apps);
    

    init_db_helper(&my_cluster.database);

    db_batch_insert_applications(apps, num_apps);

    free(apps);

    free_cluster_conf(&my_cluster);
}
