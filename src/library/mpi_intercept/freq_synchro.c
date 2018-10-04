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
#include <common/ear_verbose.h>
#include <library/common/externs.h>

#if EAR_LIB_SYNC
extern MPI_Comm masters_comm;
extern int num_masters;
extern int my_master_rank;
extern int my_master_size;
extern int masters_connected;
extern int *local_f;
extern int *remote_f;
extern unsigned masters_comm_created;
#endif

#if COORDINATE_FREQUENCIES

static const char *__NAME__ = "EARL-Sync";
extern char *__HOST__ ;
MPI_Request synchro_request;
static int synchro_pending=0;

void global_frequency_selection_send(ulong my_local_f)
{
	if (masters_comm_created){
        	local_f[0]=(int)my_local_f;
		earl_verbose(0,"Node %d sending freq %lu to masters group (masters %d)",my_master_rank,my_local_f,my_master_size);
		/* Check for error values */
        	PMPI_Iallgather(local_f, 1, MPI_INT, remote_f, my_master_size, MPI_INT, masters_comm,&synchro_request);
		synchro_pending=1;
	}
}
ulong global_frequency_selection_synchro()
{
	ulong global_f=0;
	int i;
	MPI_Status status;
	if (!synchro_pending) return 0;
	if (masters_comm_created){
                earl_verbose(0,"Waiting & Processing frequencies");
		PMPI_Wait(&synchro_request,&status);
		synchro_pending=0;
                for (i=0;i<my_master_size;i++){
                        if (my_master_rank==0) {
                                earl_verbose(0,"Frequency selected by node %d %lu",i,remote_f[i]);
                        }
                        global_f+=(ulong)remote_f[i];
                }
                global_f=global_f/my_master_size;
                if (my_master_rank==0) earl_verbose(0,"Global frequency should be %lu ",global_f);

	}
	return global_f;
}
#endif


