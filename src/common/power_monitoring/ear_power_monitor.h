#ifndef _POWER_MON_H_
#define _POWER_MON_H_

#include <common/states.h>
#include <common/power_monitoring/specific_api.h>

#define NUM_SOCKETS 2
#define POWER_MON_OK	EAR_SUCCESS
#define POWER_MON_ERROR	EAR_ERROR

typedef unsigned long long rapl_data_t;
typedef unsigned long node_data_t;

typedef struct energy_mon_data{
	node_data_t AC_node_energy;
	node_data_t DC_node_energy;
	rapl_data_t DRAM_energy[NUM_SOCKETS];
	rapl_data_t CPU_energy[NUM_SOCKETS];
}energy_data_t;

//  Starts power monitoring
int init_power_ponitoring();
// Ends power monitoring
void end_power_monitoring();

// Energy is returned in mili Joules
int read_enegy_data(energy_data_t *acc_energy);
// Computes the difference between two energy measurements
int diff_energy_data(energy_data_t *init,energy_data_t *end,energy_data_t *diff);

#else
#endif
