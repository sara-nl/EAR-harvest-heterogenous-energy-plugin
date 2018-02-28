/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EARD_API_H
#define _EARD_API_H

#define EAR_SUCCESS 			0
#define EAR_ALREADY_CONNECTED 	-1
#define EARDS_NOT_READY			-2


int eards_connect();
void eards_disconnect();

int eards_read_rapl(unsigned long long *values);
int eards_start_rapl();

unsigned long eards_get_data_size_rapl();
unsigned long eards_node_energy_data_size();


#else
#endif
