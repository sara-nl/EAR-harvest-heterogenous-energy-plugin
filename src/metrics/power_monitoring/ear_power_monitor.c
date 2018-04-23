/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <common/math_operations.h>



// eards_api is the ear daemon public api to be used by external apps
#include <metrics/power_monitoring/ear_power_monitor.h>
#include <common/eards_api.h>


uint8_t power_mon_connected=0; 
rapl_data_t *RAPL_metrics;

#define VERBOSE(msg) printf(msg)

static uint8_t rootp=0;
static uint8_t pm_already_connected=0;
static uint8_t pm_connected_status=0;
static char my_buffer[1024];

int pm_get_data_size_rapl()
{
	if (rootp){
		// Check that 
		return RAPL_EVS*sizeof(rapl_data_t);
	}else{
		return eards_get_data_size_rapl();
	}
}     
void pm_disconnect()
{
	if (rootp){
		node_energy_dispose();
	}else{
		eards_disconnect();
	}
}             
int pm_start_rapl()
{
	if (rootp){
		return start_rapl_metrics();
	}else{
		return eards_start_rapl();
	}
}            
int pm_stop_rapl(rapl_data_t *rm)
{
	if (rootp){
		return stop_rapl_metrics(rm);
	}else{
		// Check that
		return eards_read_rapl(rm);
	}
}            
int pm_read_rapl(rapl_data_t *rm)
{
	if (rootp){
		return read_rapl_metrics(rm);
	}else{
		return eards_read_rapl(rm);
	}
}            
int pm_node_dc_energy(node_data_t *dc)
{
	if (rootp){
		return read_dc_energy(dc);
	}else{
		return eards_node_dc_energy(dc);
	}
}
int pm_node_ac_energy(node_data_t *ac)
{
	if (rootp){
		return read_ac_energy(ac);
	}else{
		//return eards_node_ac_energy(ac);
		return 0;
	}
}


int pm_connect()
{
	if (pm_already_connected) return pm_connected_status;
	if (getuid()==0)	rootp=1;
	if (rootp){
		pm_connected_status=node_energy_init();
		if (pm_connected_status==EAR_SUCCESS) pm_connected_status=init_rapl_metrics();
		pm_already_connected=1;
		return pm_connected_status;
	}else{
		pm_already_connected=1;
		pm_connected_status=eards_connect();
		return pm_connected_status;
	}	
		
}

// POWER FUNCTIONS
void compute_power(energy_data_t *e_begin,energy_data_t *e_end,power_data_t *my_power)
{
    energy_data_t e_diff;
    double t_diff;

    // Compute the difference
	//print_energy_data(e_begin);
	//print_energy_data(e_end);
    diff_energy_data(e_end,e_begin,&e_diff);
	t_diff=difftime(e_end->sample_time,e_begin->sample_time);
    my_power->begin=e_begin->sample_time;
    my_power->end=e_end->sample_time;
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

void report_periodic_power(int fd,power_data_t *my_power)
{
    struct tm *current_t;
    char s[64];
    // We format the end time into localtime and string
    current_t=localtime(&(my_power->end));
    strftime(s, sizeof(s), "%c", current_t);

    sprintf(my_buffer,"%s : Avg. DC node power %.2lf Avg. DRAM %.2lf Avg. CPU %.2lf\n",s,my_power->avg_dc,
    my_power->avg_dram[0]+my_power->avg_dram[1],my_power->avg_cpu[0]+my_power->avg_cpu[1]);
	write(fd,my_buffer,strlen(my_buffer));
}

// END POWER


node_data_t diff_node_energy(node_data_t end,node_data_t init)
{
	node_data_t ret=0;
	if (end>init){ 
		ret=end-init;
	} else{
		//VERBOSE("OVERFLOW DETECTED!\n");
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
		ret = ullong_diff_overflow(init, end);
		//printf("OVERFLOW DETECTED RAPL! %llu,%llu\n",init,end);
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
	power_mon_connected=1;
	return POWER_MON_OK;
}
void end_power_monitoring()
{
	if (power_mon_connected) pm_disconnect();
	power_mon_connected=0;
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
		//pm_start_rapl();
		pm_node_dc_energy(&dc);
		time(&acc_energy->sample_time);
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


int diff_energy_data(energy_data_t *end,energy_data_t *init,energy_data_t *diff)
{
	diff->DC_node_energy=0;diff->DRAM_energy[0]=0;diff->DRAM_energy[1]=0;diff->CPU_energy[0]=0;diff->CPU_energy[1]=0;
	if (power_mon_connected){
		if ((init==NULL) || (end==NULL) || (diff==NULL)) return POWER_MON_ERROR;	
		// overflows must be considered, this is a prototype
		diff->DC_node_energy=diff_node_energy(end->DC_node_energy,init->DC_node_energy);
		//diff->AC_node_energy=diff_node_energy(end->AC_node_energy,init->AC_node_energy);
		diff->AC_node_energy=0;
		printf("DRAM end( %#llx %#llx) init (%#llx %#llx)\n",
		end->DRAM_energy[0],end->DRAM_energy[1],init->DRAM_energy[0],init->DRAM_energy[1]);
		printf("CPU end( %#llx %#llx) init (%#llx %#llx)\n",
		end->CPU_energy[0],end->CPU_energy[1],init->CPU_energy[0],init->CPU_energy[1]);
	 
		diff->DRAM_energy[0]=diff_RAPL_energy(end->DRAM_energy[0],init->DRAM_energy[0]);
		diff->DRAM_energy[1]=diff_RAPL_energy(end->DRAM_energy[1],init->DRAM_energy[1]);
		diff->CPU_energy[0]=diff_RAPL_energy(end->CPU_energy[0],init->CPU_energy[0]);
		diff->CPU_energy[1]=diff_RAPL_energy(end->CPU_energy[1],init->CPU_energy[1]);
		return POWER_MON_OK;
	}else{
		return POWER_MON_ERROR;
	}
}

void copy_energy_data(energy_data_t *dest,energy_data_t *src)
{
	dest->sample_time=src->sample_time;
	dest->AC_node_energy=src->AC_node_energy;
	dest->DC_node_energy=src->DC_node_energy;
	dest->DRAM_energy[0]=src->DRAM_energy[0];
	dest->DRAM_energy[1]=src->DRAM_energy[1];
	dest->CPU_energy[0]=src->CPU_energy[0];
	dest->CPU_energy[1]=src->CPU_energy[1];
}
void print_energy_data(energy_data_t *e)
{
    struct tm *current_t;
    char s[64];
    // We format the end time into localtime and string
    current_t=localtime(&(e->sample_time));
    strftime(s, sizeof(s), "%c", current_t);

	printf("time %s DC %lu DRAM (%llu+%llu) CPU (%llu+%llu) \n",s,e->DC_node_energy,e->DRAM_energy[0],e->DRAM_energy[1],
	e->CPU_energy[0],e->CPU_energy[1]);
}

