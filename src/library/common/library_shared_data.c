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

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <common/config.h>
#include <common/states.h>
#include <common/output/verbose.h>
#include <common/types/configuration/cluster_conf.h>
#include <library/common/library_shared_data.h>

static int fd_conf,fd_signatures;

int  get_lib_shared_data_path(char *tmp,char *path)
{
	if ((tmp==NULL) || (path==NULL)) return EAR_ERROR;
	sprintf(path,"%s/.ear_lib_shared_data",tmp);
	return EAR_SUCCESS;	
}
int  get_shared_signatures_path(char *tmp,char *path)
{
	if ((tmp==NULL) || (path==NULL)) return EAR_ERROR;
	sprintf(path,"%s/.ear_shared_signatures",tmp);
	return EAR_SUCCESS;	
}




/***** SPECIFIC FUNCTIONS *******************/



lib_shared_data_t * create_lib_shared_data_area(char * path)  
{      	
	lib_shared_data_t sh_data,*my_area;
	my_area=(lib_shared_data_t *)create_shared_area(path,(char *)&sh_data,sizeof(lib_shared_data_t),&fd_conf,1);
	return my_area;
}

lib_shared_data_t * attach_lib_shared_data_area(char * path)
{
    return (lib_shared_data_t *)attach_shared_area(path,sizeof(lib_shared_data_t),O_RDWR,&fd_conf,NULL);
}                                
void dettach_lib_shared_data_area()
{
	dettach_shared_area(fd_conf);
}

void lib_shared_data_area_dispose(char * path)
{
	dispose_shared_area(path,fd_conf);
}


void print_lib_shared_data(lib_shared_data_t *sh_data)
{
	fprintf(stderr,"sh_data num_processes %lu signatures %d cas_counters %lf\n",sh_data->num_processes,sh_data->num_signatures,sh_data->cas_counters);

}

/// SIGNATURES

// Creates a shared memory region between eard and ear_lib. returns NULL if error.
shsignature_t * create_shared_signatures_area(char * path, int np)  
{      	
	shsignature_t *my_sig,*p2;

	my_sig=(shsignature_t*)malloc(sizeof(shsignature_t)*np);
	p2=create_shared_area(path,(char *)my_sig,sizeof(shsignature_t)*np,&fd_signatures,1);
	free(my_sig);
	return p2;
}

shsignature_t * attach_shared_signatures_area(char * path,int np)
{
    return (shsignature_t *)attach_shared_area(path,sizeof(shsignature_t)*np,O_RDWR,&fd_signatures,NULL);
}                                
void dettach_shared_signatures_area()
{
	dettach_shared_area(fd_signatures);
}
void shared_signatures_area_dispose(char * path)
{
	dispose_shared_area(path,fd_signatures);
}


void signature_ready(shsignature_t *sig)
{
	sig->ready=1;
}

int compute_total_signatures_ready(lib_shared_data_t *data,shsignature_t *sig)
{
	int i,total=0;
  for (i=0;i<data->num_processes;i++) total=total+sig[i].ready;
	data->num_signatures=total;
	return total;
}
int are_signatures_ready(lib_shared_data_t *data,shsignature_t *sig)
{
	int i;
	compute_total_signatures_ready(data,sig);
	return(data->num_signatures==data->num_processes);
}

void clean_signatures(lib_shared_data_t *data,shsignature_t *sig)
{
	int i,total=0;
  for (i=0;i<data->num_processes;i++) sig[i].ready=0;
}

void clean_mpi_info(lib_shared_data_t *data,shsignature_t *sig)
{
  int i,total=0;
  for (i=0;i<data->num_processes;i++){ 
		sig[i].mpi_info.mpi_time=0;
		sig[i].mpi_info.total_mpi_calls=0;
		sig[i].mpi_info.perc_mpi=0;
		sig[i].mpi_info.exec_time=0;
	}
}

void clean_my_mpi_info(mpi_information_t *info)
{
	info->mpi_time=0;
	info->total_mpi_calls=0;
	info->perc_mpi=0;
	info->exec_time=0;
}

int select_cp(lib_shared_data_t *data,shsignature_t *sig)
{
  int i,rank=sig[0].mpi_info.rank;
	double minp=sig[0].mpi_info.perc_mpi;
  for (i=1;i<data->num_processes;i++){
    if (sig[i].mpi_info.perc_mpi<minp){ 
			rank=sig[i].mpi_info.rank;
			minp=sig[i].mpi_info.perc_mpi;
		}
  }
	return rank;
}

int select_global_cp(int size,int max,int *ppn,mpi_information_t *my_mpi_info)
{
	int i,j;
	int rank;
	double minp=100.0;
	/* Node loop */
	for (i=0;i<size;i++){
		/* Inside node */
		for (j=0;j<ppn[i];j++){
			if (minp>my_mpi_info[i*max+j].perc_mpi){
				rank=my_mpi_info[i*max+j].rank;
				minp=my_mpi_info[i*max+j].perc_mpi;
			}
		}
	}
	return rank;
}



void print_shared_signatures(lib_shared_data_t *data,shsignature_t *sig)
{
	int i;
	double percmpi;

	for (i=0;i<data->num_processes;i++){
		fprintf(stdout,"RANK %d mpi_data[%d]={total_mpi_calls %u mpi_time %llu exec_time %llu PercTime %lf}\n",
		sig[i].mpi_info.rank,i,sig[i].mpi_info.total_mpi_calls,sig[i].mpi_info.mpi_time,sig[i].mpi_info.exec_time,sig[i].mpi_info.perc_mpi);
		fprintf(stdout,"signature[%d]={cpi %.3lf tpi %.3lf time %.3lf dc_power %.3lf}\n",i,sig[i].sig.CPI,sig[i].sig.TPI,
		sig[i].sig.time,sig[i].sig.DC_power);
	}
}


void copy_my_mpi_info(lib_shared_data_t *data,shsignature_t *sig,mpi_information_t *my_mpi_info)
{
	int i;
	for (i=0;i<data->num_processes;i++){
		memcpy(&my_mpi_info[i],&sig[i].mpi_info,sizeof(mpi_information_t));
	}
}

void compute_per_node_mpi_info(lib_shared_data_t *data,shsignature_t *sig,mpi_information_t *my_mpi_info)
{
  int i,min_i=0;
	double min_perc=sig[0].mpi_info.perc_mpi;
  for (i=1;i<data->num_processes;i++){
		if (sig[i].mpi_info.perc_mpi<min_perc){
			min_i=i;
			min_perc=sig[i].mpi_info.perc_mpi;
		}
  }
  memcpy(&my_mpi_info,&sig[min_i].mpi_info,sizeof(mpi_information_t));
}



