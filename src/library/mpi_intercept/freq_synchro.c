/**************************************************************
 * *       Energy Aware Runtime (EAR)
 * *       This program is part of the Energy Aware Runtime (EAR).
 * *
 * *       EAR provides a dynamic, transparent and ligth-weigth solution for
 * *       Energy management.
 * *
 * *       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017  
 * *       BSC Contact     mailto:ear-support@bsc.es
 * *       Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * *       EAR is free software; you can redistribute it and/or
 * *       modify it under the terms of the GNU Lesser General Public
 * *       License as published by the Free Software Foundation; either
 * *       version 2.1 of the License, or (at your option) any later version.
 * *       
 * *       EAR is distributed in the hope that it will be useful,
 * *       but WITHOUT ANY WARRANTY; without even the implied warranty of
 * *       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * *       Lesser General Public License for more details.
 * *       
 * *       You should have received a copy of the GNU Lesser General Public
 * *       License along with EAR; if not, write to the Free Software
 * *       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * *       The GNU LEsser General Public License is contained in the file COPYING  
 * */
/***********************************************************/
/* This set of functions are used to coordinate frequencies */
/***********************************************************/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/config.h>

#define SHOW_DEBUGS 1

#include <common/output/verbose.h>
#include <common/output/debug.h>
#include <common/output/error.h>
#include <library/common/externs.h>

#if EAR_LIB_SYNC
extern MPI_Comm masters_comm;
extern int num_masters;
extern int my_master_rank;
extern int my_master_size;
extern int masters_connected;
extern unsigned masters_comm_created;
#endif

#if COORDINATE_FREQUENCIES
#include <library/mpi_intercept/freq_synchro.h>
local_loop_info_t *local_f;
local_loop_info_t *remote_f;

MPI_Request synchro_request;
static int synchro_pending=0;

void configure_global_synchronization()
{
	char *ear_global_sync;
	ear_global_sync=getenv("EAR_GLOBAL_SYNC");
	if (ear_global_sync!=NULL){
		global_synchro=atoi(ear_global_sync);
	}
	if (my_master_rank==0) {
		verbose(1,"Global synchronizations set to %d",global_synchro);
	}
    local_f=(local_loop_info_t *)calloc(1,sizeof(local_loop_info_t));
    remote_f=(local_loop_info_t *)calloc(my_master_size,sizeof(local_loop_info_t));
    if ((local_f==NULL) || (remote_f==NULL)){
            error("Error, memory not available for synchronization\n");
            masters_comm_created=0;
    }
 
}

void global_frequency_selection_send(local_loop_info_t *my_info)
{
	if ((masters_comm_created) && (synchro_pending==0)){
			debug("Node %d sending (%lu,%lu,%lu) to masters group (masters %d)",my_master_rank,my_info->local_f,my_info->iter_time,my_info->mpi_iter_time,my_master_size);
			/* Check for error values */
        	PMPI_Iallgather(my_info, 3, MPI_UNSIGNED_LONG, remote_f, 3, MPI_UNSIGNED_LONG, masters_comm,&synchro_request);
			synchro_pending=1;
	}
}

ulong global_frequency_selection_synchro()
{
	ulong global_f=0;
	float avg_freq;
	int i,flag=0;
	MPI_Status status;
	if (synchro_pending!=1) return 0;
	if (masters_comm_created){
        debug("Master rank %u Waiting & Processing frequencies",my_master_rank);
		PMPI_Test(&synchro_request, &flag, &status);
		if (flag){
			synchro_pending=0;
            for (i=0;i<my_master_size;i++){
            	if (my_master_rank==0) {
            		debug("Frequency selected by node %d %lu",i,remote_f[i].local_f);
            		debug("IterTime node %d %lu",i,remote_f[i].iter_time);
            		debug("MpiIterTime node %d %lu",i,remote_f[i].mpi_iter_time);
            	}
            	global_f+=(ulong)remote_f[i].local_f;
            }
            avg_freq=global_f/my_master_size;
			avg_freq=avg_freq/(float)100000;
			global_f=(ulong)avg_freq*100000;
            if (my_master_rank==0) {
            	verbose(1,"Global frequency should be %lu ",global_f);
            }
		}else return 0;

	}
	return global_f;
}

#endif
