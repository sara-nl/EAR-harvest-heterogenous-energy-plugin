/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
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

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <common/config.h>
#define SHOW_DEBUGS 1
#include <common/output/verbose.h>
#include <common/math_operations.h>
#include <common/states.h>
#include <common/hardware/hardware_info.h>
#include <metrics/energy/energy_cpu.h>
#include <metrics/accumulators/power_metrics.h>
uint8_t power_mon_connected=0; 
rapl_data_t *RAPL_metrics;
static uint node_units;
static size_t node_size;


static uint8_t rootp=0;
static uint8_t pm_already_connected=0;
static uint8_t pm_connected_status=0;
static char my_buffer[1024];
static int num_packs=0;

int pm_get_data_size_rapl()
{
	if (rootp){
		// Check that 
		if (num_packs==0){
			num_packs=detect_packages(NULL);
			if (num_packs==0){
				error("Error detecting num packages");
				return EAR_ERROR;
			}
		}
		return RAPL_POWER_EVS*sizeof(rapl_data_t)*num_packs;
	}else{
		return EAR_ERROR;
		//return eards_get_data_size_rapl();
	}
}     
void pm_disconnect(ehandler_t *my_eh)
{
	if (rootp){
		energy_dispose(my_eh);
	}else{
		//eards_disconnect();
	}
}             
int pm_read_rapl(ehandler_t *my_eh,rapl_data_t *rm)
{
	if (rootp){
		return read_rapl_msr(my_eh->fds_rapl,(unsigned long long *)rm);
	}else{
		return EAR_ERROR;
	}
}            
int pm_node_dc_energy(ehandler_t *my_eh,node_data_t *dc)
{
	if (rootp){
		return energy_dc_read(my_eh,dc);
	}else{
		*dc=0;
		return EAR_ERROR;
		//return eards_node_dc_energy(dc);
	}
}
int pm_node_ac_energy(ehandler_t *my_eh, node_data_t *ac)
{
	if (rootp){
		return energy_ac_read(my_eh, ac);
	}else{
		//return eards_node_ac_energy(ac);
		return 0;
	}
}


int pm_connect(ehandler_t *my_eh)
{
	if (pm_already_connected) return pm_connected_status;
	if (getuid()==0)	rootp=1;
	if (rootp){
		debug("Initializing energy in main power_monitor  thread");
		pm_connected_status=energy_init(NULL, my_eh);
		if (pm_connected_status==EAR_SUCCESS){ 
			energy_units(my_eh,&node_units);
			energy_datasize(my_eh,&node_size);
			num_packs=detect_packages(NULL);
			if (num_packs==0){
				error("Packages cannot be detected");
				pm_connected_status=EAR_ERROR;
				return EAR_ERROR;
			}
			debug("%d packages detected in power metrics ",num_packs);
			pm_connected_status=init_rapl_msr(my_eh->fds_rapl);
			if (pm_connected_status!=EAR_SUCCESS){
				error("Error initializing RAPl in pm_connect");
			}
		}else{
			error("Initializing energy node plugin");
		}
		pm_already_connected=1;
		return pm_connected_status;
	}else{
		return EAR_ERROR;
	}	
		
}

// POWER FUNCTIONS
void compute_power(energy_data_t *e_begin,energy_data_t *e_end,power_data_t *my_power)
{
    double t_diff;
		unsigned long curr_node_energy;
		rapl_data_t		*dram,*pack;
		int p;

		dram=(rapl_data_t*)calloc(num_packs,sizeof(rapl_data_t));
		pack=(rapl_data_t*)calloc(num_packs,sizeof(rapl_data_t));

    // Compute the difference
    for (p=0;p<num_packs;p++) dram[p]=diff_RAPL_energy(e_end->DRAM_energy[p],e_begin->DRAM_energy[p]);
		for (p=0;p<num_packs;p++) pack[p]=diff_RAPL_energy(e_end->CPU_energy[p],e_begin->CPU_energy[p]);
		for (p=0;p<num_packs;p++) debug("energy dram pack %d %llu\n",p,dram[p]);
		for (p=0;p<num_packs;p++) debug("energy cpu pack %d %llu\n",p,pack[p]);
		// eh is not needed here
		energy_accumulated(NULL,&curr_node_energy,e_begin->DC_node_energy,e_end->DC_node_energy);
		t_diff=difftime(e_end->sample_time,e_begin->sample_time);
		// Compute the power
    my_power->begin=e_begin->sample_time;
    my_power->end=e_end->sample_time;
    my_power->avg_ac=0;
    my_power->avg_dc=(double)(curr_node_energy)/(t_diff*node_units);
		for (p=0;p<num_packs;p++) my_power->avg_dram[p]=(double)(dram[p])/(t_diff*1000000000);
    for (p=0;p<num_packs;p++) my_power->avg_cpu[p]=(double)(pack[p])/(t_diff*1000000000);
		for (p=0;p<num_packs;p++) debug("power dram p=%d %lf\n",p,my_power->avg_dram[p]);
		for (p=0;p<num_packs;p++) debug("power pack p=%d %lf\n",p,my_power->avg_cpu[p]);
		free(dram);
		free(pack);
}
void print_power(power_data_t *my_power)
{
    struct tm *current_t;
    char s[64],spdram[256],spcpu[256],s1dram[64],s1cpu[64];;
		int p;
		double dram_power=0,pack_power=0;
		dram_power=accum_dram_power(my_power);
		pack_power=accum_cpu_power(my_power);
    // We format the end time into localtime and string
    current_t=localtime(&(my_power->end));
    strftime(s, sizeof(s), "%c", current_t);

    printf("%s : Avg. DC node power %.2lf Avg. DRAM %.2lf Avg. CPU %.2lf\n",s,my_power->avg_dc,dram_power,pack_power); 
}

void report_periodic_power(int fd,power_data_t *my_power)
{
    struct tm *current_t;
		double dram_power=0,pack_power=0;
    char s[64],spdram[256],spcpu[256],s1dram[64],s1cpu[64];
		int p,pp;
	
		dram_power=accum_dram_power(my_power);
		pack_power=accum_cpu_power(my_power);
    // We format the end time into localtime and string
    current_t=localtime(&(my_power->end));
    strftime(s, sizeof(s), "%c", current_t);

		sprintf(spdram,"Avg. DRAM %.2lf[",dram_power);
		for (p=0;p<num_packs;p++){
			if (p<(num_packs-1)) sprintf(s1dram,"%.2lf,",my_power->avg_dram[p]);
			else sprintf(s1dram,"%.2lf]",my_power->avg_dram[p]);
			strcat(spdram,s1dram);
		}
		sprintf(spcpu,"Avg. CPU %.2lf[",pack_power);
    for (p=0;p<num_packs;p++){
      if (p<(num_packs-1)) sprintf(s1cpu,"%.2lf,",my_power->avg_cpu[p]);
      else sprintf(s1cpu,"%.2lf]",my_power->avg_cpu[p]);
      strcat(spcpu,s1cpu);
    }

    sprintf(my_buffer,"%s : Avg. DC node power %.2lf %s %s\n",s,my_power->avg_dc,spdram,spcpu);
		write(fd,my_buffer,strlen(my_buffer));
}

// END POWER


rapl_data_t diff_RAPL_energy(rapl_data_t end,rapl_data_t init)
{
	rapl_data_t ret=0;
	
	if (end>=init){
		ret=end-init;
	}else{
		ret = ullong_diff_overflow(init, end);
		//printf("OVERFLOW DETECTED RAPL! %llu,%llu\n",init,end);
	}
	return ret;
}

// EAR_API_ERRORS are pending to be defined
// eards API uses EAR_TMP environment variable to connect
int init_power_ponitoring(ehandler_t *my_eh)
{
	int ret;
	unsigned long rapl_size;
	if (power_mon_connected) return POWER_MON_OK;
	ret=pm_connect(my_eh);
	if (ret!=EAR_SUCCESS){
		error("Error in init_power_ponitoring");
		return POWER_MON_ERROR;
	}
	rapl_size=pm_get_data_size_rapl();
	if (rapl_size==0){
		pm_disconnect(my_eh);
		return POWER_MON_ERROR;
	}
	RAPL_metrics=(rapl_data_t *)malloc(rapl_size);
	if (RAPL_metrics==NULL){
		pm_disconnect(my_eh);
		return POWER_MON_ERROR;
	}
	memset((char *)RAPL_metrics,0,rapl_size);
	power_mon_connected=1;
	return POWER_MON_OK;
}
void end_power_monitoring(ehandler_t *my_eh)
{
	if (power_mon_connected) pm_disconnect(my_eh);
	power_mon_connected=0;
}

void null_energy_data(energy_data_t *acc_energy)
{
	int i;
	time(&acc_energy->sample_time);
	memset((char *)acc_energy->DC_node_energy,0,node_size);
	acc_energy->AC_node_energy=0;
	memset(acc_energy->DRAM_energy,0,num_packs*sizeof(rapl_data_t));
	memset(acc_energy->CPU_energy,0,num_packs*sizeof(rapl_data_t));
}

void null_power_data(power_data_t *p)
{
	p->begin=0;
	p->end=0;
	p->avg_dc=0;
	p->avg_ac=0;
	memset(p->avg_dram,0,num_packs*sizeof(double));
	memset(p->avg_cpu,0,num_packs*sizeof(double));
	
}


int read_enegy_data(ehandler_t *my_eh,energy_data_t *acc_energy)
{
	node_data_t ac=0;
	int p;
	
	time(&acc_energy->sample_time);
	if (power_mon_connected){
		if (acc_energy==NULL) return POWER_MON_ERROR;
		// Contacting the eards api
		pm_read_rapl(my_eh,RAPL_metrics);
		pm_node_dc_energy(my_eh,acc_energy->DC_node_energy);
		acc_energy->AC_node_energy=ac;
		for (p=0;p<num_packs;p++){
			debug("DRAM pack %d =%llu\n",p,RAPL_metrics[p]);
		}
		for (p=0;p<num_packs;p++){
			debug("CPU pack %d =%llu\n",p,RAPL_metrics[num_packs+p]);
		}
		memcpy(acc_energy->DRAM_energy,RAPL_metrics,num_packs*sizeof(rapl_data_t));
		memcpy(acc_energy->CPU_energy,&RAPL_metrics[num_packs],num_packs*sizeof(rapl_data_t));
		return POWER_MON_OK;
	}else{
		return POWER_MON_ERROR;
	}
}


void copy_energy_data(energy_data_t *dest,energy_data_t *src)
{
	dest->sample_time=src->sample_time;
	dest->AC_node_energy=src->AC_node_energy;
	memcpy(dest->DC_node_energy,src->DC_node_energy,node_size);
	memcpy(dest->DRAM_energy,src->DRAM_energy,num_packs*sizeof(rapl_data_t));
	memcpy(dest->CPU_energy,src->CPU_energy,num_packs*sizeof(rapl_data_t));
}

void copy_power_data(power_data_t *dest,power_data_t *src)
{
	dest->begin=src->begin;
	dest->end=src->end;
	dest->avg_dc=src->avg_dc;
	dest->avg_ac=src->avg_ac;
	memcpy(dest->avg_dram,src->avg_dram,num_packs*sizeof(double));
	memcpy(dest->avg_cpu,src->avg_cpu,num_packs*sizeof(double));
}

void print_energy_data(energy_data_t *e)
{
    struct tm *current_t;
    char s[64];
		char nodee[256];
		int i,j;
    // We format the end time into localtime and string
    current_t=localtime(&(e->sample_time));
    strftime(s, sizeof(s), "%c", current_t);
		energy_to_str(NULL,nodee,e->DC_node_energy);	

	printf("time %s DC %s",s,nodee); 
	printf("DRAM (");
	for (j=0;j<num_packs;j++){
		if (j<(num_packs-1)){
			printf("%llu,",e->DRAM_energy[j]);
		}else{
			printf("%llu)",e->DRAM_energy[j]);
		}
	}	
	printf("CPU( ");
	for (j=0;j<num_packs;j++){
    if (j<(num_packs-1)){
      printf("%llu,",e->CPU_energy[j]);
    }else{
      printf("%llu)",e->CPU_energy[j]);
    }
  }

	printf("\n");
}

double accum_node_power(power_data_t *p)
{
	return p->avg_dc;
}

double accum_dram_power(power_data_t *p)
{
	double dram_power=0;
	int pid;
	for (pid=0;pid<num_packs;pid++){ 
		debug("Acuum_dram %d %lf total %lf",pid,p->avg_dram[pid],dram_power);
		dram_power=dram_power+p->avg_dram[pid];	
	}
	return dram_power;
}
double accum_cpu_power(power_data_t *p)
{
	double pack_power=0;
	int pid;
	for (pid=0;pid<num_packs;pid++){ 
		debug("Acuum_cpu %d %lf total %lf",pid,p->avg_cpu[pid],pack_power);
		pack_power=pack_power+p->avg_cpu[pid];
	}
	return pack_power;
}


void alloc_energy_data(energy_data_t *e)
{
	e->DC_node_energy=(edata_t*)malloc(node_size);
	e->AC_node_energy=(edata_t*)malloc(node_size);
	e->DRAM_energy=(rapl_data_t *)calloc(num_packs,sizeof(rapl_data_t));
	e->CPU_energy=(rapl_data_t *)calloc(num_packs,sizeof(rapl_data_t));
}

void alloc_power_data(power_data_t *p)
{
	p->avg_dram=(double*)calloc(num_packs,sizeof(double));
	p->avg_cpu=(double*)calloc(num_packs,sizeof(double));
}

void free_power_data(power_data_t *p)
{
	free(p->avg_dram);
	free(p->avg_cpu);
}

void free_energy_data(energy_data_t *e)
{
	free(e->DC_node_energy);
	free(e->AC_node_energy);
	free(e->DRAM_energy);
	free(e->CPU_energy);
	
}
