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


/** This program sets the node frequency . It must be executed with privileges
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <control/frequency.h>

void usage(char *prog)
{
	fprintf(stderr,"usage:%s cpus|[-h]\n",prog);
	fprintf(stdout,"This program changes the governor to ondemand. You need root privileges\n");
	exit(0);
}
void main(int argc,char *argv[])
{
	int uid;
	int cpus;
	if (argc==1) usage(argv[0]);
	if (argc==2){
		if (strcmp(argv[1],"-h")==0){
			usage(argv[0]);
		}else cpus=atoi(argv[1]);
	}else usage(argv[0]);
	
	if (getuid()!=0){ 
		usage(argv[0]);
	}
	printf("This program will change the governor to ondemand. CPUS %d\n",cpus);
	frequency_init(cpus);
	frequency_set_ondemand_governor_all_cpus();
	frequency_dispose();
}
