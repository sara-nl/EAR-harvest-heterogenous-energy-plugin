/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ear_daemon_client.h>
#include <ear_verbose.h>

#define ENERGY1_INPUT 	0
#define NODE_ENERGY

#ifdef NODE_ENERGY
static unsigned long *energy_init, *energy_end, *acum_energy;
#endif

unsigned long int read_dc_energy()
{
 	unsigned long value;
	ear_daemon_client_node_dc_energy(acum_energy);
	value=*acum_energy*1000;	
	return value;
}

// Returns -1 if DC file doesn't exist or we cannot access it
int init_dc_energy()
{

	int node_energy_size;

	node_energy_size = ear_daemon_client_node_energy_data_size();
	energy_init = (unsigned long *) malloc(node_energy_size);
	energy_end = (unsigned long *) malloc(node_energy_size);
	acum_energy = (unsigned long *) malloc(node_energy_size);
	ear_daemon_client_node_dc_energy(energy_init);

	return 0;
}

int end_dc_energy()
{
	ear_daemon_client_node_dc_energy(energy_end);
	//ear_verbose(1,"Total energy consumed %luJ \n",(*energy_end-*energy_init)/1000);
	return 0;
}

unsigned long int energy_diff(unsigned long int end,unsigned long int init)
{
    unsigned long int to_max;
    // ULONG_MAX
    if (end<init){
        ear_debug(0,"EAR(%s) ENERGY END < INIT %llu - %llu (max %llu)\n",__FILE__,end,init,ULONG_MAX);
        to_max=ULONG_MAX-init;
        return (to_max+end);    
    }else return (end-init);
}

