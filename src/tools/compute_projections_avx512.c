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
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <errno.h>
#include <ear_models/ear_models.h>
#include <ear_db_type.h>
#include <projections_avx512.h>
#include <types.h>

void usage(char *app)
{
	fprintf(stdout,"usage: %s coefficients_filename csv_file f_nominal max_f_avx512 perc_avx512\n",app);
	exit(1);
}

void main(int argc,char *argv[])
{
	struct Coefficients_info *MY_COEFFS;
	int fd,ret,size,p_states,i,pstate_avx512=-1;
	FILE *fd_apps;
	char line[1024];
	long f0,f_avx512;
	double cpi_f0,tpi_f0,time_f0,power_f0,cpi_p,perc_avx512,perc_nonavx512;
	application_t app_data;
	if (argc!=6) usage(argv[0]);
	f0=atoi(argv[3]);
	f_avx512=atoi(argv[4]);
	perc_avx512=atof(argv[5]);
	perc_nonavx512=1-perc_avx512;
	fprintf(stderr,"Nominal %lu Max_avx512_f %lu perc_avx512 %lf\n",f0,f_avx512,perc_avx512);
	fprintf(stderr,"Reading coefficients %s\n",argv[1]);

	MY_COEFFS=read_coefficients_from_file(argv[1],&p_states);	
	if (MY_COEFFS==NULL){
		fprintf(stderr,"Erroc reading the coefficients\n");
		exit(1);	
	}
	fprintf(stdout,"USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH\n");
	fprintf(stderr,"Reading metrics %s\n",argv[2]);
	fd_apps=fopen(argv[2],"r");
	if (fd_apps==NULL){
		fprintf(stderr,"Error opening csv file %s (%s)\n",argv[2],strerror(errno));
		exit(1);
	}
	i=0;
	while((MY_COEFFS[i].pstate!=f_avx512) && (i<p_states)) i++;
	if (i!=p_states) pstate_avx512=i;
	if ((MY_COEFFS[pstate_avx512].available==0) && (perc_avx512>0)){
		fprintf(stderr,"Error , we can not compute projections since coefficients for %lu are not available\n",pstate_avx512);
		exit(1);	
	}
	fprintf(stderr,"AVX512 max frequency corresponds to pstate %d\n",pstate_avx512);
	// We jump the header
	fscanf(fd_apps,"%s\n",&line);
	// Read line by line
	while((ret=fscanf(fd_apps,"%[^;];%[^;];%[^;];%[^;];%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;%[^;];%lf\n",
	app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
	&app_data.f,
	&app_data.seconds,&app_data.CPI,&app_data.TPI_f0,&app_data.GBS_f0,&app_data.Gflops,&app_data.POWER_f0,
	&app_data.DRAM_POWER,&app_data.PCK_POWER,&app_data.nominal,app_data.policy,&app_data.th))>0){

	// When an execution with the reference frequency is detected, we project time and power for the available frequencies
	if (app_data.nominal==f0){
	    fprintf(stderr,"Reference values for %s: avgf %u CPI %f TPI %f Time %f Power %f\n",
		app_data.app_id,app_data.f,app_data.CPI,app_data.TPI_f0,app_data.seconds,app_data.POWER_f0);
		cpi_f0=app_data.CPI;
		tpi_f0=app_data.TPI_f0;
		time_f0=app_data.seconds;
		power_f0=app_data.POWER_f0;
	    	for (i=0;i<p_states;i++){
		    if (MY_COEFFS[i].available){
			// We have two cases
			if (MY_COEFFS[i].pstate>=MY_COEFFS[pstate_avx512].pstate){
			    fprintf(stderr,"CASE  1 %lu\n",MY_COEFFS[i].pstate);
			    // Case 1: MY_COEFFS[i].pstate>MY_COEFFS[pstate_avx512].pstate
			    cpi_p=cpi_proj(cpi_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[1],perc_nonavx512,perc_avx512);
			    // All the models are the same in this range of freqs. I generated three times to make it easy the loading in excel
			    //MODEL 1
			    fprintf(stdout,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;AVX512_MODEL1;%lf\n",
				app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
				MY_COEFFS[i].pstate,time_proj(time_f0,cpi_p,cpi_f0,f0,(MY_COEFFS[i].pstate*(unsigned long)(perc_nonavx512*100)+MY_COEFFS[1].pstate*(unsigned long)(perc_avx512*100))/100),
				cpi_p,0.0,0.0,0.0,power_proj(power_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[1],perc_nonavx512,perc_avx512),
				0.0,0.0,MY_COEFFS[i].pstate,perc_avx512);
			    //MODEL 2
			    fprintf(stdout,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;AVX512_MODEL2;%lf\n",
				app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
				MY_COEFFS[i].pstate,time_proj(time_f0,cpi_p,cpi_f0,f0,(MY_COEFFS[i].pstate*(unsigned long)(perc_nonavx512*100)+MY_COEFFS[1].pstate*(unsigned long)(perc_avx512*100))/100),
				cpi_p,0.0,0.0,0.0,power_proj(power_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[1],perc_nonavx512,perc_avx512),
				0.0,0.0,MY_COEFFS[i].pstate,perc_avx512);
			    //MODEL 3
			    fprintf(stdout,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;AVX512_MODEL3;%lf\n",
				app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
				MY_COEFFS[i].pstate,time_proj(time_f0,cpi_p,cpi_f0,f0,(MY_COEFFS[i].pstate*(unsigned long)(perc_nonavx512*100)+MY_COEFFS[1].pstate*(unsigned long)(perc_avx512*100))/100),
				cpi_p,0.0,0.0,0.0,power_proj(power_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[1],perc_nonavx512,perc_avx512),
				0.0,0.0,MY_COEFFS[i].pstate,perc_avx512);
			}else{
			    fprintf(stderr,"CASE 2 %lu \n",MY_COEFFS[pstate_avx512].pstate);
			    // Case 2: MY_COEFFS[i].pstate<MY_COEFFS[pstate_avx512].pstate
			    // MODEL 1 assumes under MY_COEFFS[pstate_avx512].pstate avx works the same as non-avx
			    // MODE 1
			    cpi_p=cpi_proj(cpi_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[i],perc_nonavx512,perc_avx512);
			    fprintf(stdout,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;AVX512_MODEL1;%lf\n",
				app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
				MY_COEFFS[i].pstate,time_proj(time_f0,cpi_p,cpi_f0,f0,(MY_COEFFS[i].pstate*(unsigned long)(perc_nonavx512*100)+MY_COEFFS[i].pstate*(unsigned long)(perc_avx512*100))/100),
				cpi_p,0.0,0.0,0.0,power_proj(power_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[i],perc_nonavx512,perc_avx512),
				0.0,0.0,MY_COEFFS[i].pstate,perc_avx512);
			    // MODEL 2 assumes under MY_COEFFS[pstate_avx512].pstate avx works similar to ONE p_state less
			    // MODE 2
			    cpi_p=cpi_proj(cpi_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[i-1],perc_nonavx512,perc_avx512);
			    fprintf(stdout,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;AVX512_MODEL2;%lf\n",
				app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
				MY_COEFFS[i].pstate,time_proj(time_f0,cpi_p,cpi_f0,f0,(MY_COEFFS[i].pstate*(unsigned long)(perc_nonavx512*100)+MY_COEFFS[i-1].pstate*(unsigned long)(perc_avx512*100))/100),
				cpi_p,0.0,0.0,0.0,power_proj(power_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[i-1],perc_nonavx512,perc_avx512),
				0.0,0.0,MY_COEFFS[i].pstate,perc_avx512);
			    // MODEL 3 assumes under MY_COEFFS[pstate_avx512].pstate avx works the same as before
			    // MODE 3
			    cpi_p=cpi_proj(cpi_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[1],perc_nonavx512,perc_avx512);
			    fprintf(stdout,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;AVX512_MODEL3;%lf\n",
				app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
				MY_COEFFS[i].pstate,time_proj(time_f0,cpi_p,cpi_f0,f0,(MY_COEFFS[i].pstate*(unsigned long)(perc_nonavx512*100)+MY_COEFFS[1].pstate*(unsigned long)(perc_avx512*100))/100),
				cpi_p,0.0,0.0,0.0,power_proj(power_f0,tpi_f0,&MY_COEFFS[i],&MY_COEFFS[1],perc_nonavx512,perc_avx512),
				0.0,0.0,MY_COEFFS[i].pstate,perc_avx512);
			}
		    }
	    }	
	}
    }
}

