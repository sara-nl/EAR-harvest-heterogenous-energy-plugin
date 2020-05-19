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
#ifndef _SLURM_DEFITIONS_H
#define _SLURM_DEFITIONS_H

#define SCHED_LOADER_VERBOSE "SLURM_LOADER_VERBOSE"
#define SCHED_EAR_SHOW_SIGNATURES "SLURM_EAR_SHOW_SIGNATURES"
#define SCHED_EAR_POWER_POLICY "SLURM_EAR_POWER_POLICY"
#define SCHED_EAR_MIN_PERC_MPI "SLURM_EAR_MIN_PERC_MPI"
#define SCHED_EAR_RED_FREQ_IN_MPI "SLURM_EAR_RED_FREQ_IN_MPI"


#define SCHED_JOB_ID	"SLURM_JOB_ID"
#define SCHED_STEP_ID "SLURM_STEP_ID"
#define SCHED_STEPID "SLURM_STEPID"
#define SCHED_JOB_ACCOUNT "SLURM_JOB_ACCOUNT"

#define NULL_JOB_ID getpid()
#define NULL_STEPID (0xfffffffe)
#define NULL_ACCOUNT "NO_SLURM_ACCOUNT"

#define SCHED_EAR_DYNAIS_WINDOW_SIZE "SLURM_EAR_DYNAIS_WINDOW_SIZE"
#define SCHED_EAR_DEF_FREQ "SLURM_EAR_DEF_FREQ"
#define SCHED_EAR_TRACE_PATH "SLURM_EAR_TRACE_PATH"
#define SCHED_EAR_TRACE_PLUGIN "SLURM_EAR_TRACE_PLUGIN"


#endif

