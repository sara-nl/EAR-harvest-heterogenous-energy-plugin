/**************************************************************
 * *   Energy Aware Runtime (EAR)
 * *   This program is part of the Energy Aware Runtime (EAR).
 * *
 * *   EAR provides a dynamic, transparent and ligth-weigth solution for
 * *   Energy management.
 * *
 * *       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * *   BSC Contact     mailto:ear-support@bsc.es
 * *   Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * *   EAR is free software; you can redistribute it and/or
 * *   modify it under the terms of the GNU Lesser General Public
 * *   License as published by the Free Software Foundation; either
 * *   version 2.1 of the License, or (at your option) any later version.
 * *   
 * *   EAR is distributed in the hope that it will be useful,
 * *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 * *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * *   Lesser General Public License for more details.
 * *   
 * *   You should have received a copy of the GNU Lesser General Public
 * *   License along with EAR; if not, write to the Free Software
 * *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * *   The GNU LEsser General Public License is contained in the file COPYING  
 * */
#include <stdio.h>
#include <common/database/db_helper.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>

int EAR_VERBOSE_LEVEL=1;

void usage(char *name)
{
    printf("Usage: %s [nodename] [number of apps] [1|0, is_learning (default 0)]\n");
}

void main(int argc, char *argv[])
{
	int num_apps, is_learning=0;
    int total_apps = 0;
    int max_apps = 0;
	int i;
	application_t *apps;
	if (argc<3){
        usage(argv[0]);
	}
    if (argc > 4)
        if (is_learning = atoi(argv[3]))
            printf("Reading applications from learning db\n"); 
    else{
		printf("Reading applications from normal DB\n");
	}
    max_apps = atoi(argv[2]);
    if (max_apps < 1)
    {
        printf("Number of requested apps must be >= 1 (current: %d).\n", max_apps);
        exit(0);
    }
    char ear_path[256];
	cluster_conf_t my_conf;
    if (get_ear_conf_path(ear_path)==EAR_ERROR){
            printf("Error getting ear.conf path\n");
            exit(0);
    }

    read_cluster_conf(ear_path,&my_conf);
	init_db_helper(&my_conf.database);
	num_apps=db_read_applications(&apps,is_learning, 50, argv[1]);
    while (num_apps > 0)
    {
	    for (i=0;i<num_apps;i++){
		    report_application_data(&apps[i]);
	    }
        free(apps);
        total_apps += num_apps;
	    num_apps=db_read_applications(&apps,is_learning, 50, "r22u21");
    }
}
