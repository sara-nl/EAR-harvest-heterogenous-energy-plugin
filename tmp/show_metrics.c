/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <cpufreq.h>
#include <unistd.h>
#include <stdlib.h>

#define DEBUG

#include <ear_db_type.h>

void Usage(char *app)
{
	char men[128];
	sprintf(men,"Usage: %s system_database_filename\n",app);
	write(1,men,strlen(men));
	exit(1);
}

int main(int argc,char *argv[])
{
	int fd,i;
	application_t test;
	unsigned int num_app;
	//char *header_instances="USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;GFLOPS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;POLICY;POLICY_TH\n";
	char *header_instances="USERNAME;JOB_ID;NODENAME;APPNAME;AVG.FREQ;TIME;CPI;TPI;GBS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;DEF.FREQ;EDP;POLICY;POLICY_TH\n";
	char EAR_metric[1024];

	if (argc!=2) Usage(argv[0]);
	// We read data from data file
	fd=open(argv[1],O_RDONLY);
	if (fd<0){
		perror("Error opening DB file");
		_exit(1);
	}
	write(1,header_instances,strlen(header_instances));	
	while (read(fd,&test,sizeof(application_t))==sizeof(application_t)){
		//sprintf(EAR_metric,"%s;%s;%s;%s;%u;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%u;%s;%.3lf\n",test.user_id,test.job_id,test.node_id,test.app_id,test.f,test.seconds,test.CPI,test.TPI_f0,test.GBS_f0,test.Gflops,test.POWER_f0,test.DRAM_POWER,test.PCK_POWER,test.nominal,test.policy,test.th);
		sprintf(EAR_metric,"%s;%s;%s;%s;%u;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%u;%.3lf;%s;%.3lf\n",test.user_id,test.job_id,test.node_id,test.app_id,test.f,test.seconds,test.CPI,test.TPI_f0,test.GBS_f0,test.POWER_f0,test.DRAM_POWER,test.PCK_POWER,test.nominal,test.EDP,test.policy,test.th);
		write(1,EAR_metric,strlen(EAR_metric));
	}
	close(fd);
 
}
