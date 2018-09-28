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
#include <common/database/db_helper.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>

int EAR_VERBOSE_LEVEL=1;

#define APP_QUERY "SELECT * FROM Applications WHERE job_id=%d"

void main(int argc,char *argv[])
{
	int num_apps, job_id, step_id = -1;
	application_t *apps;
    char *token;
    if (argc < 2)
    {
        fprintf(stderr, "Usage is get_projections jobid[.stepid])\n");
        exit(0);
    }

    job_id = atoi(strtok(argv[1], "."));
    token = strtok(NULL, ".");
    if (token != NULL) step_id = atoi(token);

    char ear_path[256];
	cluster_conf_t my_conf;
    if (get_ear_conf_path(ear_path)==EAR_ERROR){
        printf("Error getting ear.conf path\n");
        exit(0);
    }
    read_cluster_conf(ear_path,&my_conf);
	init_db_helper(&my_conf.database);
    
    char query[256], sub_query[64];
    sprintf(query, APP_QUERY, job_id);

    if (step_id > -1)
    {
        sprintf(sub_query, "AND step_id=%d", step_id);
        strcat(query, sub_query);
    }
    num_apps = db_read_applications_query(&apps, query);
    int i;
    for (i = 0; i < num_apps; i++)
        print_application(&apps[i]);
    free(apps);

}
