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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <global_manager/eargm_rapi.h>
#include <common/types/configuration/cluster_conf.h>

void usage(char *app)
{
	verbose(0, "usage:%s num_nodes", app);
	exit(1);
}

cluster_conf_t my_cluster;

void main(int argc,char *argv[])
{
	int eargms;
	unsigned int num_nodes;
	char myhost[NAME_SIZE],my_ear_conf_path[NAME_SIZE];
	if (argc!=2) usage(argv[0]);
	num_nodes=(unsigned int)atoi(argv[1]);
	// NEW_JOB
    if (get_ear_conf_path(my_ear_conf_path)==EAR_ERROR){
        verbose(0,"Error opening ear.conf file, not available at regular paths (/etc/ear/ear.conf or $EAR_INSTALL_PATH/etc/sysconf/ear.conf)");
        exit(0);
    }

	read_cluster_conf(my_ear_conf_path,&my_cluster);
	#if API_DEBUG
	verbose(0, "Using port %u", my_cluster.eargm.port);
	#endif
	eargms=eargm_connect(my_cluster.eargm.host,my_cluster.eargm.port);
	if(eargms<0){ 
		verbose(0, "Connection error");
		exit(1);
	}
	if (!eargm_new_job(num_nodes)) {
		verbose(0, "eargm_new_job error sending new_job command"); //error
	}
	eargm_disconnect();
}
