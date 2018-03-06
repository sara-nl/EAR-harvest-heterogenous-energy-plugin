/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#include <stdio.h>
#include <stdlib.h>
#include <common/ear_daemon_client.h>

#define MATRIX_SIZE 1024
static double a[MATRIX_SIZE][MATRIX_SIZE];
static double b[MATRIX_SIZE][MATRIX_SIZE];
static double c[MATRIX_SIZE][MATRIX_SIZE];
int EAR_VERBOSE_LEVEL=0;

/* Naive matrix multiply */
void run_test(int quiet) {

       double s;
       int i,j,k;
	

       if (!quiet) {
            printf("Doing a naive %dx%d MMM...\n",MATRIX_SIZE,MATRIX_SIZE);
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

       if (!quiet) printf("Matrix multiply sum: s=%lf\n",s);
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

	struct timeval begin_time, end_time;
	fprintf(stderr, "ear_daemon_test usage: ear_daemon_test [nsteps, def=%d] \n",nsteps);
	if (argc>1){ 
		nsteps=atoi(argv[1]);
	}
	fprintf(stderr, "ear_daemon_test: Using %d steps every %d sec\n",nsteps,sleep_time);
	ear_daemon_client_connect();
	freq_events=ear_daemon_client_get_data_size_frequency();
	uncore_events=ear_daemon_client_get_data_size_uncore();
	rapl_events=ear_daemon_client_get_data_size_rapl();
	fprintf(stdout,"Uncore counters provides %d events\n",uncore_events/sizeof(long long));	
	fprintf(stdout,"RAPL counters provides %d events\n",rapl_events/sizeof(long long));	
	uncore_values=(long long*)malloc(uncore_events);
	rapl_values=(long long*)malloc(rapl_events);
	for (i=0;i<nsteps;i++){
		mem_bd=0;rapl_pack=0;rapl_memory=0;
		// Start counters
		ear_daemon_client_start_uncore();
		ear_daemon_client_start_rapl();
		gettimeofday(&begin_time,NULL);
		// Wait
		sleep(sleep_time);
		// Stop and reset
		ear_daemon_client_read_uncore(uncore_values);
		ear_daemon_client_reset_uncore();	
		ear_daemon_client_read_rapl(rapl_values);
		ear_daemon_client_reset_rapl();
		gettimeofday(&end_time,NULL);
		elapsed=(unsigned long)(end_time.tv_sec*1000000+end_time.tv_usec)-(begin_time.tv_sec*1000000+begin_time.tv_usec);
		elapsed_sec=(double)elapsed/(double)1000000;
		fprintf(stdout,"---------------------------\n");
		fprintf(stdout,"Time between events %lu\n",elapsed);
		for (events=0;events<uncore_events/sizeof(long long);events++){
			mem_bd+=uncore_values[events];
		}
		rapl_pack=rapl_values[2]+rapl_values[3];
		rapl_memory=rapl_values[0]+rapl_values[1];
		fprintf(stdout,"MEMORY_BD %lf GBS (CAS_COUNT %llu)\n",(double)(mem_bd*64)/(double)(elapsed_sec*(1024*1024*1024)),mem_bd);

		fprintf(stdout,"PACK AVG POWER = %lf W (%llu/%lu)\n",(double)(rapl_pack/1.0e9)/elapsed_sec,rapl_pack,elapsed);
		fprintf(stdout,"DRAM AVG POWER = %lf W (%llu/%lu)\n",(double)(rapl_memory/1.0e9)/elapsed_sec,rapl_memory,elapsed);

		fprintf(stdout,"PACK ENERGY = %lf J \n",(double)rapl_pack/1.0e9);
		fprintf(stdout,"DRAM ENERGY = %lf J \n",(double)rapl_memory/1.0e9);
	}
	ear_daemon_client_disconnect();
}	
