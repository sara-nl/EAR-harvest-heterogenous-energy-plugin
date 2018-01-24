/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

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
#include <projections.h>
#include <coefficients_tools.h>

void usage(char *app)
{
	fprintf(stdout,"usage: %s coefficients_filename csv_file nominal\n",app);
	exit(1);
}

void main(int argc,char *argv[])
{
	struct Coefficients_info *my_coeffs;
	int fd,ret,size,p_states,i;
	FILE *fd_apps;
	char line[1024];
	long f0;
	double cpi_f0,tpi_f0,time_f0,power_f0,cpi_p;
	struct App_info app_data;
	if (argc!=4) usage(argv[0]);
	f0=atoi(argv[3]);
	fprintf(stderr,"Nominal %u\n",f0);
	fprintf(stderr,"Reading coefficients %s\n",argv[1]);

	my_coeffs=read_coefficients_from_file(argv[1],&p_states);	
	if (my_coeffs==NULL){
		fprintf(stderr,"Erroc reading the coefficients\n");
		exit(1);	
	}
	fprintf(stdout,"USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;PROJECTION;POLICY_TH\n");
	fprintf(stderr,"Reading metrics %s\n",argv[2]);
	fd_apps=fopen(argv[2],"r");
	if (fd_apps==NULL){
		fprintf(stderr,"Error opening csv file %s (%s)\n",argv[2],strerror(errno));
		exit(1);
	}
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
		    if (my_coeffs[i].available){
			    cpi_p=cpi_proj(cpi_f0,tpi_f0,&my_coeffs[i]);
			    fprintf(stdout,"%s;%s;%s;%s;%u;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%u;PROJECTION;0.0\n",
				app_data.user_id,app_data.job_id,app_data.node_id,app_data.app_id,
				my_coeffs[i].pstate,time_proj(time_f0,cpi_p,cpi_f0,f0,my_coeffs[i].pstate),
				cpi_p,0.0,0.0,0.0,power_proj(power_f0,tpi_f0,&my_coeffs[i]),
				0.0,0.0,f0);
		    }
	    }	
	}
    }
}
