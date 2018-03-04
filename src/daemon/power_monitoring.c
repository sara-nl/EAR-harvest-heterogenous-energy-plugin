/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
    BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <common/ear_verbose.h>
#include <metrics/power_monitoring/ear_power_monitor.h>

extern int eard_must_exit;
#define NUM_SAMPLES_L1 	1000 	// maximum number of samples saved every -frequency_monitoring- usecs
#define NUM_SAMPLES_L2	100		// Every NUM_SAMPLES_L1 of L1 we will compute 1 sample of L2, 

unsigned int f_monitoring;

typedef struct power_data{
    time_t begin,end;
    double avg_dc,avg_ac;
    double avg_dram[NUM_SOCKETS];
    double avg_cpu[NUM_SOCKETS];
}power_data_t;

power_data_t *L1_samples,*L2_samples;
int current_L1=0;
int num_L1=0;
int current_L2=0;
int num_L2=0;


// BUFFERS

power_data_t * create_historic_buffer(int samples)
{
	void *mem;
	mem=malloc(sizeof(power_data_t)*samples);
	if (mem!=NULL) memset(mem,0,sizeof(power_data_t)*samples);
	return mem;
}
void update_historic_info(power_data_t *my_current_power)
{
	return;
}


// END BUFFERS

// POWER FUNCTIONS
void compute_power(energy_data_t *e_begin,energy_data_t *e_end,time_t t_begin,time_t t_end,power_data_t *my_power)
{
	energy_data_t e_diff;
	double t_diff;
	// Compute the difference
	diff_energy_data(e_end,e_begin,&e_diff);
	t_diff=difftime(t_end,t_begin);
	t_diff=t_diff*1000; // Because energy is returned in mJ
	printf("EBEGIN :");
	print_energy_data(e_begin);
	printf("EEND :");
	print_energy_data(e_end);
	printf("EDIFF :");
	print_energy_data(&e_diff);
	printf("AVg. power after %.0lf msec\n",t_diff);

	my_power->begin=t_begin;
	my_power->end=t_end;
	my_power->avg_ac=0;
	my_power->avg_dc=(double)e_diff.DC_node_energy/t_diff;
	my_power->avg_dram[0]=(double)e_diff.DRAM_energy[0]/t_diff;
	my_power->avg_dram[1]=(double)e_diff.DRAM_energy[1]/t_diff;
	my_power->avg_cpu[0]=(double)e_diff.CPU_energy[0]/t_diff;
	my_power->avg_cpu[1]=(double)e_diff.CPU_energy[1]/t_diff;
	
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
	

// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *frequency_monitoring)
{
	unsigned int *f_monitoringp=(unsigned int *)frequency_monitoring;
	energy_data_t e_begin,e_end;	
	unsigned long t_ms;
	time_t t_begin,t_end; 
	power_data_t my_current_power;

	ear_verbose(0,"power monitoring thread created\n");
	if (init_power_ponitoring()!=EAR_SUCCESS) ear_verbose(0,"Error in init_power_ponitoring\n");
	f_monitoring=*f_monitoringp;
	t_ms=f_monitoring/1000;

	// Create circular buffer for samples
	L1_samples=create_historic_buffer(NUM_SAMPLES_L1);
	if (L1_samples==NULL){
		ear_verbose(0,"power monitoring: error allocating memory for logs\n");
		pthread_exit(0);
	}
	L2_samples=create_historic_buffer(NUM_SAMPLES_L2);
	if (L2_samples==NULL){
		ear_verbose(0,"power monitoring: error allocating memory for logs\n");
		pthread_exit(0);
	}

	// We will collect and report avg power until eard finishes
	// Get time and Energy
	time(&t_begin);
	read_enegy_data(&e_begin);
	while(!eard_must_exit)
	{
		// Wait for N usecs
		usleep(f_monitoring);
		// Get time and Energy
		time(&t_end);
		read_enegy_data(&e_end);
		
		// Compute the power
		compute_power(&e_begin,&e_end,t_begin,t_end,&my_current_power);
		// print basic data in stdout
		print_power(&my_current_power);
		// Save current power
		update_historic_info(&my_current_power);
		// Set values for next iteration
		copy_energy_data(&e_begin,&e_end);
		t_begin=t_end;
	}
	pthread_exit(0);
}
