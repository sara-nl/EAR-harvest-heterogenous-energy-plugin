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



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <metrics/ipmi/energy_node.h>

void main(int argc,char *argv[])
{
	unsigned long energy_freq,init,end;
	struct timeval begin_time,end_time;
	unsigned long min_interval;

    if (getuid()!=0){
        printf("Warning, this test need root privileges, execute it as root or with sudo\n");
    }

	// Init 
	node_energy_init();
	read_dc_energy(&energy_freq);
    do {
		read_dc_energy(&init);
     } while(init == energy_freq);

     gettimeofday(&begin_time,NULL);

     do{
		read_dc_energy(&end);
     } while(init == end);

     gettimeofday(&end_time,NULL);
     min_interval  = (end_time.tv_sec * 1000000 + end_time.tv_usec);
     min_interval -= (begin_time.tv_sec *1000000 + begin_time.tv_usec);

	fprintf(stderr,"Energy changes every %u usecs....\n",min_interval);
	node_energy_dispose();

}

