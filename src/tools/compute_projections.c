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
#include <projections.h>

void usage(char *app)
{
	fprintf(stdout,"usage: %s coefficients_filename CPI_f0 TPI_f0 Time_f0 Power_f0 F0(Hz)\n",app);
	exit(1);
}

void main(int argc,char *argv[])
{
	struct Coefficients_info *my_coeffs;
	int fd,ret,size,p_states,i;
	long f0;
	double cpi_f0,tpi_f0,time_f0,power_f0,cpi_p;
	if (argc!=7) usage(argv[0]);
	cpi_f0=atof(argv[2]);
	tpi_f0=atof(argv[3]);
	time_f0=atof(argv[4]);
	power_f0=atof(argv[5]);
	f0=(unsigned long)atol(argv[6]);
	fprintf(stderr,"Reference values: CPI %f TPI %f Time %f Power %f\n",cpi_f0,tpi_f0,time_f0,power_f0);
	fd=open(argv[1],O_RDONLY);
	if (fd<0){
		fprintf(stderr,"Coefficients file error: %s\n",strerror(errno));
		exit(1);
	}
	size=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	// The number of p_states depends on the architecture
	my_coeffs=(struct Coefficients_info *)malloc(size);
	if (my_coeffs==NULL){
		fprintf(stderr,"Erroc malloc returns null (%s)\n",strerror(errno));
		exit(1);	
	}
	// Reset the memory to zeroes
	memset(my_coeffs,0,sizeof(struct Coefficients_info));
	p_states=size/sizeof(struct Coefficients_info);
	// We read the coefficients 
	if ((ret=read(fd,my_coeffs,size))!=size){
		fprintf(stderr,"Error when readin coefficients file (%s)\n",strerror(errno));
		exit(1);
	}

	fprintf(stdout,"FREQ;CPI_PROJ;TIME_PROJ;POWER_PROJ\n");
	for (i=0;i<p_states;i++){
		if (my_coeffs[i].available){
			cpi_p=cpi_proj(cpi_f0,tpi_f0,&my_coeffs[i]);
			fprintf(stdout,"%u;%f;%f;%f\n",my_coeffs[i].pstate,cpi_p,time_proj(time_f0,cpi_p,cpi_f0,f0,my_coeffs[i].pstate),power_proj(power_f0,tpi_f0,&my_coeffs[i]));
		}
	}	
	
}
