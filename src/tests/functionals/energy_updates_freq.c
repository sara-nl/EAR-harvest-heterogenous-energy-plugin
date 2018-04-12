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



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <metrics/ipmi/energy_node.h>

int EAR_VERBOSE_LEVEL=1;

void main(int argc,char *argv[])
{
	unsigned long energy_freq,init,end;
	// Init 
	node_energy_init();
	energy_freq=node_energy_frequency();
	if (energy_freq>0){
		fprintf(stderr,"Energy changes every %u usecs....testing...sleeping %lu usecs\n",energy_freq,energy_freq*2);
		read_dc_energy(&init);
		usleep(energy_freq*2);
		read_dc_energy(&end);	
		fprintf(stderr,"Energy after %lu usecs %lu mJ (%lu-%lu)\n",energy_freq*2,(end-init),end,init);
	}else{
		fprintf(stderr,"Warning...DC node energy seems not being supported\n");
	}
	node_energy_dispose();

}

