/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _LENOVO_AIR_COOLING_H_
#define _LENOVO_AIR_COOLING_H_
/* Specific functions for CPU XX PLATFORM YY */
/* Grants access to ipmi device */
int lenovo_act_node_energy_init();
int lenovo_act_count_energy_data_length();
int lenovo_act_read_dc_energy(unsigned long *energy);
/* AC energy is not yet supported */
int lenovo_act_read_ac_energy(unsigned long *energy);
/* Release access to ipmi device */
int lenovo_act_node_energy_dispose();


#else
#endif
