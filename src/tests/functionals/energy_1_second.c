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
#include <metrics/ipmi/energy_node.h>

int EAR_VERBOSE_LEVEL=1;

void main(int argc,char *argv[])
{
	unsigned long init, end;
	// Init 
	node_energy_init();
	// get accumulated energy
	read_dc_energy(&init);
	sleep(1);
	// get accumulated energy
	read_dc_energy(&end);
	fprintf(stderr,"Total energy consumed after 1 sec  %lu J (%lu-%lu)\n",(end-init)/1000,end,init);
	node_energy_dispose();

}

