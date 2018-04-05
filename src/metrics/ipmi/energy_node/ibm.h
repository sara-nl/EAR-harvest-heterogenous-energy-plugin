/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _IBM_H_
#define _IBM_H_
/** Specific functions for CPU XX PLATFORM YY 
*   Grants access to ipmi device */
int ibm_node_energy_init();
int ibm_count_energy_data_length();
int ibm_read_dc_energy(unsigned long *energy);

/** AC energy is not yet supported */
int ibm_read_ac_energy(unsigned long *energy);

/** Release access to ipmi device */
int ibm_node_energy_dispose();


#else
#endif
