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
#include <sys/time.h>
#include <unistd.h>

#include <daemon/eard_api.h>

#define MATRIX_SIZE 1024
static double a[MATRIX_SIZE][MATRIX_SIZE];
static double b[MATRIX_SIZE][MATRIX_SIZE];
static double c[MATRIX_SIZE][MATRIX_SIZE];

/* Naive matrix multiply */
void run_test(int quiet) {

       double s;
       int i,j,k;
	

       if (!quiet) {
            verbose(0, "Doing a naive %dx%d MMM...",MATRIX_SIZE,MATRIX_SIZE);
       }

       for(i=0;i<MATRIX_SIZE;i++) {
     for(j=0;j<MATRIX_SIZE;j++) {
       a[i][j]=(double)i*(double)j;
       b[i][j]=(double)i/(double)(j+5);
     }
       }

       for(j=0;j<MATRIX_SIZE;j++) {
     for(i=0;i<MATRIX_SIZE;i++) {
       s=0;
       for(k=0;k<MATRIX_SIZE;k++) {
         s+=a[i][k]*b[k][j];
       }
       c[i][j] = s;
     }
       }

       s=0.0;
       for(i=0;i<MATRIX_SIZE;i++) {
        for(j=0;j<MATRIX_SIZE;j++) {
            s+=c[i][j];
        }
       }

       if (!quiet) {
       		verbose(0, "Matrix multiply sum: s=%lf",s);
       }
}

void main(int argc,char *argv[])
{
	int nsteps=10,i,events;
	long long *uncore_values;
	long long *rapl_values;
	unsigned long uncore_events;
	unsigned long rapl_events;
	unsigned long freq_events;
	unsigned long elapsed;
	double elapsed_sec;
	long long mem_bd;
	long long rapl_pack;
	long long rapl_memory;
	unsigned int sleep_time=5;
	application_t  my_app;
	
	my_app.job.id=1;
	my_app.job.step_id=0;

	struct timeval begin_time, end_time;
	verbose(0, "ear_daemon_test usage: ear_daemon_test [nsteps, def=%d]", nsteps);
	if (argc>1){ 
		nsteps=atoi(argv[1]);
	}
	verbose(0, "ear_daemon_test: Using %d steps every %d sec", nsteps, sleep_time);
	eards_connect(&my_app);
	freq_events=eards_get_data_size_frequency();
	uncore_events=eards_get_data_size_uncore();
	rapl_events=eards_get_data_size_rapl();
	verbose(0, "Uncore counters provides %d events", uncore_events/sizeof(long long));
	verbose(0, "RAPL counters provides %d events", rapl_events/sizeof(long long));
	uncore_values=(long long*)malloc(uncore_events);
	rapl_values=(long long*)malloc(rapl_events);
	for (i=0;i<nsteps;i++){
		mem_bd=0;rapl_pack=0;rapl_memory=0;
		// Start counters
		eards_start_uncore();
		eards_start_rapl();
		gettimeofday(&begin_time,NULL);
		// Wait
		sleep(sleep_time);
		// Stop and reset
		eards_read_uncore(uncore_values);
		eards_reset_uncore();	
		eards_read_rapl(rapl_values);
		eards_reset_rapl();
		gettimeofday(&end_time,NULL);
		elapsed=(unsigned long)(end_time.tv_sec*1000000+end_time.tv_usec)-(begin_time.tv_sec*1000000+begin_time.tv_usec);
		elapsed_sec=(double)elapsed/(double)1000000;
		verbose(0, "---------------------------");
		verbose(0, "Time between events %lu", elapsed);
		for (events=0;events<uncore_events/sizeof(long long);events++){
			mem_bd+=uncore_values[events];
		}
		rapl_pack=rapl_values[2]+rapl_values[3];
		rapl_memory=rapl_values[0]+rapl_values[1];
		verbose(0, "MEMORY_BD %lf GBS (CAS_COUNT %llu)", (double)(mem_bd*64)/(double)(elapsed_sec*(1024*1024*1024)), mem_bd);

		verbose(0, "PACK AVG POWER = %lf W (%llu/%lu)", (double)(rapl_pack/1.0e9)/elapsed_sec, rapl_pack, elapsed);
		verbose(0, "DRAM AVG POWER = %lf W (%llu/%lu)", (double)(rapl_memory/1.0e9)/elapsed_sec, rapl_memory, elapsed);

		verbose(0, "PACK ENERGY = %lf J", (double) rapl_pack / 1.0e9);
		verbose(0, "DRAM ENERGY = %lf J", (double) rapl_memory / 1.0e9);
	}
	eards_disconnect();
}	
