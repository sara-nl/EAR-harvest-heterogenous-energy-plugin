/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#ifndef _EARSYS_
#define _EARSYS_

// Verifies IBMEAM files are available
int init_dc_energy();
void end_dc_energy();
// Returns energy value provided by IBMAEM. Energy is reported in uJ
unsigned long int read_dc_energy();
// Returns the difference between two energy measurements
unsigned long int energy_diff(unsigned long int end,unsigned long int init);
#else
#endif
