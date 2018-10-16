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


#define COL_RED "\x1b[31m"
#define COL_GRE "\x1b[32m"
#define COL_YLW "\x1b[33m"
#define COL_BLU "\x1b[34m"
#define COL_MGT "\x1b[35m"
#define COL_CYA "\x1b[36m"
#define COL_RST "\x1b[0m"

char *colors[7] = { COL_RED, COL_GRE, COL_YLW, COL_BLU, COL_MGT, COL_CYA, COL_RST };

void main(int argc,char *argv[])
{
    char buffer[256];
	int num_apps;
    int total_apps = 0;
	int i, c = 0;
    char *node_name = NULL;
	application_t *apps;
	cluster_conf_t my_conf;
    
	node_name = argv[1];
	c = atoi(argv[2]) % 6; 
 
	if (get_ear_conf_path(buffer)==EAR_ERROR) {
		printf("Error getting ear.conf path\n");
		exit(0);
    }
	
	//
    read_cluster_conf(buffer, &my_conf);
	
	//
	init_db_helper(&my_conf.database);
	
	//
	num_apps = db_read_applications(&apps, 1, 50, node_name);
   
	// 
	tprintf_init(stdout, "22 11 12 10 8 14 12");
 
	while (num_apps > 0)
    {
		total_apps += num_apps;
	    
		for (i = 0; i < num_apps; i++)
		{
			if (strcmp(buffer, apps[i].node_id) != 0) {
				strcpy(buffer, apps[i].node_id);
			}
		
			tprintf("%s%s%s||%s||%lu||%0.2lf||%0.2lf|| | %lu || %0.2lf",
				colors[c], apps[i].job.app_id, colors[6], apps[i].node_id,
				apps[i].job.def_f, apps[i].signature.time, apps[i].signature.DC_power,
				apps[i].signature.avg_f, apps[i].signature.GBS);
	    }

        free(apps);
	    
		num_apps = db_read_applications(&apps, 1, 50, node_name);
    }
}
