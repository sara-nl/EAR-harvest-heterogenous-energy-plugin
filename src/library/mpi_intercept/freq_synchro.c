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
#include <common/output/verbose.h>
#include <library/common/externs.h>

#if EAR_LIB_SYNC
extern MPI_Comm masters_comm;
extern int num_masters;
extern int my_master_rank;
extern int my_master_size;
extern int masters_connected;
extern ulong *local_f;
extern ulong *remote_f;
extern unsigned masters_comm_created;
#endif

#if COORDINATE_FREQUENCIES

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
}

void global_frequency_selection_send(ulong my_local_f)
{
	if ((masters_comm_created) && (synchro_pending==0)){
        	local_f[0]=my_local_f;
		verbose(2,"Node %d sending freq %lu to masters group (masters %d)",my_master_rank,my_local_f,my_master_size);
		/* Check for error values */
        	PMPI_Iallgather(local_f, 1, MPI_UNSIGNED_LONG, remote_f, 1, MPI_UNSIGNED_LONG, masters_comm,&synchro_request);
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
        verbose(1,"Master rank %u Waiting & Processing frequencies",my_master_rank);
		PMPI_Test(&synchro_request, &flag, &status);
		if (flag){
		//PMPI_Wait(&synchro_request,&status);
			synchro_pending=-1;
            for (i=0;i<my_master_size;i++){
            if (my_master_rank==0) {
            	verbose(2,"Frequency selected by node %d %lu",i,remote_f[i]);
                        }
              	global_f+=(ulong)remote_f[i];
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
