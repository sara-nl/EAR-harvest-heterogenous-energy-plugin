/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ear_node_energy_metrics.h>
int EAR_VERBOSE_LEVEL=1;
void main(int argc,char *argv[])
{
	unsigned long energy_freq,init,end;
	// Init 
	node_energy_init();
	energy_freq=node_energy_frequency();
	fprintf(stderr,"Energy changes every %u usecs....testing...sleeping %lu usecs\n",energy_freq,energy_freq*2);
	read_dc_energy(&init);
	usleep(energy_freq*2);
	read_dc_energy(&end);	
	fprintf(stderr,"Energy after %lu usecs %lu mJ (%lu-%lu)\n",energy_freq*2,(end-init),end,init);
	node_energy_dispose();

}

