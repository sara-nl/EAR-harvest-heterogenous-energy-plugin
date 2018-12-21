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



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <float.h>
#include <errno.h>
#include <common/output/verbose.h>
#include <common/types/generic.h>

#include <daemon/power_monitoring/ear_power_monitor.h>
#include <metrics/custom/frequency.h>

static const char *__NAME__ = "powermon: ";

//  That constant is replicated. We must fix that
#define MAX_PATH_SIZE 256

unsigned int f_monitoring;

int idleNode=1;


/* AVG, MAX, MIN for app */
int samples=0;
double max_dc=0,min_dc=DBL_MAX,avg_dc;
int current_app_id=-1;

extern char nodename[MAX_PATH_SIZE];
int fd_powermon=-1;



#define max(X,Y) (((X) > (Y)) ? (X) : (Y))
#define min(X,Y) (((X) < (Y)) ? (X) : (Y))



// Each sample is processed by this function
void update_historic_info(ulong f,power_data_t *my_current_power)
{
	printf("Avg. freq %lu: ",f);
    print_power(my_current_power);

	return;
}

void usage(char *app)
{
	printf("usage:%s sampling_period usec\n",app);
	exit(0);
}

// frequency_monitoring will be expressed in usecs
void main(int argc,char *argv[])
{
	energy_data_t e_begin;
	energy_data_t e_end;	
	unsigned long t_ms;
	time_t t_begin,t_end; 
	double t_diff;
	power_data_t my_current_power;
	ulong f;

	
	if (argc!=2) usage(argv[0]);
	f_monitoring=atoi(argv[1]);
	verbose(0,"ear_power_monitoring started: sampling period %d usec\n",f_monitoring);
	if (init_power_ponitoring()!=EAR_SUCCESS) verbose(0,"Error in init_power_ponitoring\n");
	t_ms=f_monitoring/1000;
	aperf_init(16);
	aperf_init_all_cpus(16,3200000);

	// Get time and Energy
	read_enegy_data(&e_begin);
	while(1)
	{
		aperf_get_avg_frequency_init_all_cpus();
		// Wait for N usecs
		usleep(f_monitoring);
		// Get time and Energy
		read_enegy_data(&e_end);
		f=aperf_get_avg_frequency_end_all_cpus();
	
		// Compute the power
		compute_power(&e_begin,&e_end,&my_current_power);
		// Save current power
		update_historic_info(f,&my_current_power);
		// Set values for next iteration
		copy_energy_data(&e_begin,&e_end);
		t_begin=t_end;
	}
	aperf_dispose();
	exit(0);
}
