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
#include <float.h>
#include <common/ear_verbose.h>

#include <metrics/power_monitoring/ear_power_monitor.h>

extern int eard_must_exit;
#define NUM_SAMPLES_L1 	1000 	// maximum number of samples saved every -frequency_monitoring- usecs
#define NUM_SAMPLES_L2	100		// Every NUM_SAMPLES_L1 of L1 we will compute 1 sample of L2, 

unsigned int f_monitoring;

extern int application_id;

int idleNode=1;

power_data_t *L1_samples,*L2_samples;
int current_L1=0;
int num_L1=0;
int current_L2=0;
int num_L2=0;

/* AVG, MAX, MIN for app */
int samples=0;
double max_dc=0,min_dc=DBL_MAX,avg_dc;
int current_app_id=-1;

#define max(X,Y) (((X) > (Y)) ? (X) : (Y))
#define min(X,Y) (((X) < (Y)) ? (X) : (Y))

// BUFFERS

power_data_t * create_historic_buffer(int samples)
{
	void *mem;
	mem=malloc(sizeof(power_data_t)*samples);
	if (mem!=NULL) memset(mem,0,sizeof(power_data_t)*samples);
	return mem;
}
void update_historic_info(int appID,power_data_t *my_current_power)
{
    // print basic data in stdout
	if (idleNode && (appID>0)){
		// New application connected
		idleNode=0;
		avg_dc=0;max_dc=0;min_dc=DBL_MAX;
		samples=0;
		current_app_id=appID;
	}
	if (!idleNode && (appID<0)){
		// Application disconnected 
		idleNode=1;
		printf("Application %d disconnected: DC node power metrics (avg. %lf max %lf min %lf)\n",current_app_id,avg_dc/(double)samples,max_dc,min_dc);
		current_app_id=-1;
	}
	if (!idleNode){
		max_dc=max(max_dc,my_current_power->avg_dc);
		min_dc=min(min_dc,my_current_power->avg_dc);
		avg_dc+=my_current_power->avg_dc;
		samples++;
		
		printf("Application id %d: ",appID);
	}
    print_power(my_current_power);

	return;
}


// END BUFFERS

	

// frequency_monitoring will be expressed in usecs
void *eard_power_monitoring(void *frequency_monitoring)
{
	unsigned int *f_monitoringp=(unsigned int *)frequency_monitoring;
	energy_data_t e_begin;
	energy_data_t e_end;	
	unsigned long t_ms;
	time_t t_begin,t_end; 
	double t_diff;
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
		//exit(0);
	}
	L2_samples=create_historic_buffer(NUM_SAMPLES_L2);
	if (L2_samples==NULL){
		ear_verbose(0,"power monitoring: error allocating memory for logs\n");
		pthread_exit(0);
		//exit(0);
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
		t_diff=difftime(t_end,t_begin);	
	
		// Compute the power
		compute_power(&e_begin,&e_end,t_begin,t_end,t_diff,&my_current_power);
		// Save current power
		update_historic_info(application_id,&my_current_power);
		// Set values for next iteration
		copy_energy_data(&e_begin,&e_end);
		t_begin=t_end;
	}
	pthread_exit(0);
	//exit(0);
}
