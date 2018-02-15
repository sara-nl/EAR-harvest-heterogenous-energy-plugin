/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _IBMAEM_NODE_ENERGY_H_
#define _IBMAEM_NODE_ENERGY_H_

// Verifies IBMEAM files are available
int ibmaem_node_energy_init();
// Returns energy value provided by IBMAEM. Energy is reported in mJ
int ibmaem_read_dc_energy(unsigned long *energy);

/* AC energy is not supported */
int ibmaem_read_ac_energy(unsigned long *energy);
/* Release access to ipmi device */
int ibmaem_node_energy_dispose();

int ibmaem_count_energy_data_length();

#else
#endif
