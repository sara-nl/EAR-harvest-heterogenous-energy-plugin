#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// eards_api is the ear daemon public api to be used by external apps
#include <metrics/power_monitoring/ear_power_monitor.h>


static uint8_t power_mon_connected=0; 
static rapl_data_t *RAPL_metrics;

#define VERBOSE(msg) printf(msg)

node_data_t diff_node_energy(node_data_t end,node_data_t init)
{
	node_data_t ret=0;
	if (end>init){ 
		ret=end-init;
	} else{
		VERBOSE("OVERFLOW DETECTED!\n");
	}
	return ret;
}
rapl_data_t diff_RAPL_energy(rapl_data_t end,rapl_data_t init)
{
	rapl_data_t ret=0;
	if (end>init){
		ret=end-init;
	}else{
		VERBOSE("OVERFLOW DETECTED RAPL!\n");
	}
	return ret;
}

// EAR_API_ERRORS are pending to be defined
// eards API uses EAR_TMP environment variable to connect
int init_power_ponitoring()
{
	int ret;
	unsigned long rapl_size;
	if (power_mon_connected) return POWER_MON_OK;
	ret=pm_connect();
	if (ret!=EAR_SUCCESS){
		return POWER_MON_ERROR;
	}
	rapl_size=pm_get_data_size_rapl();
	if (rapl_size!=sizeof(rapl_data_t)*NUM_SOCKETS*2){
		pm_disconnect();
		return POWER_MON_ERROR;
	}
	RAPL_metrics=malloc(rapl_size);
	if (RAPL_metrics==NULL){
		pm_disconnect();
		return POWER_MON_ERROR;
	}
	memset(RAPL_metrics,0,rapl_size);
	pm_start_rapl();
	power_mon_connected=1;
	return POWER_MON_OK;
}
void end_power_monitoring()
{
	if (power_mon_connected) pm_disconnect();
}

// acc_energy must be a valid address
// RAPL events: rapl:::DRAM_ENERGY:PACKAGE0, rapl:::DRAM_ENERGY:PACKAGE1
// 				rapl:::PACKAGE_ENERGY:PACKAGE0,rapl:::PACKAGE_ENERGY:PACKAGE1

int read_enegy_data(energy_data_t *acc_energy)
{
	node_data_t ac=0,dc=0;
	
	if (power_mon_connected){
		if (acc_energy==NULL) return POWER_MON_ERROR;
		// Contacting the eards api
		pm_read_rapl(RAPL_metrics);
		pm_start_rapl();
		pm_node_dc_energy(&dc);
		//pm_node_ac_energy(&ac); Not implemened yet
		acc_energy->DC_node_energy=dc;
		acc_energy->AC_node_energy=ac;
		acc_energy->DRAM_energy[0]=RAPL_metrics[0]/1000000;
		acc_energy->DRAM_energy[1]=RAPL_metrics[1]/1000000;
		acc_energy->CPU_energy[0]=RAPL_metrics[2]/1000000;
		acc_energy->CPU_energy[1]=RAPL_metrics[3]/1000000;
		return POWER_MON_OK;
	}else{
		return POWER_MON_ERROR;
	}
}


int diff_energy_data(energy_data_t *init,energy_data_t *end,energy_data_t *diff)
{
	if (power_mon_connected){
		if ((init==NULL) || (end==NULL) || (diff==NULL)) return POWER_MON_ERROR;	
		// overflows must be considered, this is a prototype
		diff->DC_node_energy=diff_node_energy(end->DC_node_energy,init->DC_node_energy);
		//diff->AC_node_energy=diff_node_energy(end->AC_node_energy,init->AC_node_energy);
		diff->AC_node_energy=0;
		diff->DRAM_energy[0]=diff_RAPL_energy(end->DRAM_energy[0],init->DRAM_energy[0]);
		diff->DRAM_energy[1]=diff_RAPL_energy(end->DRAM_energy[1],init->DRAM_energy[1]);
		diff->CPU_energy[0]=diff_RAPL_energy(end->CPU_energy[0],init->CPU_energy[0]);
		diff->CPU_energy[1]=diff_RAPL_energy(end->CPU_energy[1],init->CPU_energy[1]);
		return POWER_MON_OK;
	}else{
		return POWER_MON_ERROR;
	}
}

