/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
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

#include <control/frequency.h>

int EAR_VERBOSE_LEVEL=1;
void usage(char *app)
{
	printf("usage:%s node_freq (in MHz)\n",app);
	printf("	requires root privileges\n");
	printf("	execute it with sudo\n");
	exit(1);
}
void main(int argc,char *argv[])
{
	int uid;
	ulong f;
	if (argc!=2) usage(argv[0]);
	if (getuid()!=0) usage(argv[0]);
	f=(ulong)atoi(argv[1]);	
	printf("This program will change the governor to userspace and set the node f to %lu\n",f);
	frequency_init();
	frequency_set_userspace_governor_all_cpus();
	if (frequency_set_all_cpus(f)!=f){
		printf("warning, node freq was not changed correctly\n");
	}
	frequency_dispose();
	
}
