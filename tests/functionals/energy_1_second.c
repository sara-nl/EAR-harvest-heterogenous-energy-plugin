/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/
#include <stdlib.h>
#include <stdio.h>
#include <ear_node_energy_metrics.h>
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

