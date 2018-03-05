#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>


// eards_api is the ear daemon public api to be used by external apps
#include <metrics/power_monitoring/ear_power_monitor.h>


uint8_t power_mon_connected=0; 
rapl_data_t *RAPL_metrics;

#define VERBOSE(msg) printf(msg)

int pm_connect()
{
#ifdef EARDS
	return EAR_SUCCESS;
#endif
#ifdef NO_EARDS
	int r=node_energy_init();
	if (r==EAR_SUCCESS) r=init_rapl_metrics();
	return r;
#endif
#ifdef EAR_USER
	return eards_connect();
#endif

}

// POWER FUNCTIONS
void compute_power(energy_data_t *e_begin,energy_data_t *e_end,time_t t_begin,time_t t_end,double sec, power_data_t *my_power)
{
    energy_data_t e_diff;
    double t_diff;

    // Compute the difference
	//print_energy_data(e_begin);
	//print_energy_data(e_end);
    diff_energy_data(e_end,e_begin,&e_diff);
	t_diff=sec;
    my_power->begin=t_begin;
    my_power->end=t_end;
    my_power->avg_ac=0;
    my_power->avg_dc=(double)(e_diff.DC_node_energy)/(t_diff*1000);
    my_power->avg_dram[0]=(double)(e_diff.DRAM_energy[0])/(t_diff*1000000000);
    my_power->avg_dram[1]=(double)(e_diff.DRAM_energy[1])/(t_diff*1000000000);
    my_power->avg_cpu[0]=(double)(e_diff.CPU_energy[0])/(t_diff*1000000000);
    my_power->avg_cpu[1]=(double)(e_diff.CPU_energy[1])/(t_diff*1000000000);
}
void print_power(power_data_t *my_power)
{
    struct tm *current_t;
    char s[64];
    // We format the end time into localtime and string
    current_t=localtime(&(my_power->end));
    strftime(s, sizeof(s), "%c", current_t);

    printf("%s : Avg. DC node power %.2lf Avg. DRAM %.2lf Avg. CPU %.2lf\n",s,my_power->avg_dc,
    my_power->avg_dram[0]+my_power->avg_dram[1],my_power->avg_cpu[0]+my_power->avg_cpu[1]);
}
// END POWER


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
	rapl_data_t aux;
	if (end>init){
		ret=end-init;
	}else{
		aux=(double)0xffffffff-init;
		ret=aux+end;
		printf("OVERFLOW DETECTED RAPL! %llu,%llu\n",init,end);
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
	RAPL_metrics=(rapl_data_t *)malloc(rapl_size);
	if (RAPL_metrics==NULL){
		pm_disconnect();
		return POWER_MON_ERROR;
	}
	memset((char *)RAPL_metrics,0,rapl_size);
	pm_start_rapl();
	pm_read_rapl(RAPL_metrics);
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
		//pm_reset_rapl();
		pm_start_rapl();
		pm_node_dc_energy(&dc);
		//pm_node_ac_energy(&ac); Not implemened yet
		acc_energy->DC_node_energy=dc;
		acc_energy->AC_node_energy=ac;
		acc_energy->DRAM_energy[0]=RAPL_metrics[0];
		acc_energy->DRAM_energy[1]=RAPL_metrics[1];
		acc_energy->CPU_energy[0]=RAPL_metrics[2];
		acc_energy->CPU_energy[1]=RAPL_metrics[3];
		return POWER_MON_OK;
	}else{
		return POWER_MON_ERROR;
	}
}

int reset_energy_data()
{
	if (power_mon_connected){
		pm_reset_rapl();
	}else{
		return POWER_MON_ERROR;
	}
}


int diff_energy_data(energy_data_t *end,energy_data_t *init,energy_data_t *diff)
{
	diff->DC_node_energy=0;diff->DRAM_energy[0]=0;diff->DRAM_energy[1]=0;diff->CPU_energy[0]=0;diff->CPU_energy[1]=0;
	if (power_mon_connected){
		if ((init==NULL) || (end==NULL) || (diff==NULL)) return POWER_MON_ERROR;	
		// overflows must be considered, this is a prototype
		diff->DC_node_energy=diff_node_energy(end->DC_node_energy,init->DC_node_energy);
		//diff->AC_node_energy=diff_node_energy(end->AC_node_energy,init->AC_node_energy);
		diff->AC_node_energy=0;
#if 0
		diff->DRAM_energy[0]=diff_RAPL_energy(end->DRAM_energy[0],init->DRAM_energy[0]);
		diff->DRAM_energy[1]=diff_RAPL_energy(end->DRAM_energy[1],init->DRAM_energy[1]);
		diff->CPU_energy[0]=diff_RAPL_energy(end->CPU_energy[0],init->CPU_energy[0]);
		diff->CPU_energy[1]=diff_RAPL_energy(end->CPU_energy[1],init->CPU_energy[1]);
#endif		
		diff->DRAM_energy[0]=end->DRAM_energy[0];
		diff->DRAM_energy[1]=end->DRAM_energy[1];
		diff->CPU_energy[0]=end->CPU_energy[0];
		diff->CPU_energy[1]=end->CPU_energy[1];

		return POWER_MON_OK;
	}else{
		return POWER_MON_ERROR;
	}
}

void copy_energy_data(energy_data_t *dest,energy_data_t *src)
{
	dest->AC_node_energy=src->AC_node_energy;
	dest->DC_node_energy=src->DC_node_energy;
	dest->DRAM_energy[0]=src->DRAM_energy[0];
	dest->DRAM_energy[1]=src->DRAM_energy[1];
	dest->CPU_energy[0]=src->CPU_energy[0];
	dest->CPU_energy[1]=src->CPU_energy[1];
}
void print_energy_data(energy_data_t *e)
{
	printf("DC %lu DRAM (%llu+%llu) CPU (%llu+%llu) \n",e->DC_node_energy,e->DRAM_energy[0],e->DRAM_energy[1],
	e->CPU_energy[0],e->CPU_energy[1]);
}

