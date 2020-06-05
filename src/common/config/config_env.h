/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/

#ifndef CONFIG_ENVIRONMENT_H
#define CONFIG_ENVIRONMENT_H

#define SCHED_SLURM 1
// Database
#define VAR_INS_PATH "EAR_INSTALL_PATH"
#define VAR_TMP_PATH "EAR_TMP"
#define VAR_ETC_PATH "EAR_ETC"
#define VAR_APP_NAME "EAR_APP_NAME"
#define VAR_OPT_VERB "EAR_VERBOSE"
#define VAR_OPT_POLI "EAR_POWER_POLICY"
#define VAR_OPT_FREQ "EAR_FREQUENCY"
#define VAR_OPT_PSTA "EAR_P_STATE"
#define VAR_OPT_LERN "EAR_LEARNING_PHASE"
#define VAR_OPT_ETAG "EAR_ENERGY_TAG"
#define VAR_OPT_USDB "EAR_USER_DB_PATHNAME"
#define VAR_OPT_THRA "EAR_POWER_POLICY_TH"
#define VAR_OPT_THRB "EAR_PERFORMANCE_PENALTY"
#define VAR_OPT_THRC "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN"

//
#define REL_PATH_LIBR "lib"
#define REL_NAME_LIBR "libear"
#define REL_PATH_LOAD "lib/libearld.so"


#ifdef SCHED_SLURM
//
#define VAR_OPT_TRAC "SLURM_EAR_TRACE_PATH"
#define HACK_PATH_LIBR "SLURM_HACK_LIBRARY"
#define HACK_FILE_LIBR "SLURM_HACK_LIBRARY_FILE"
#define HACK_FILE_LOAD "SLURM_HACK_LOADER"
#define FLAG_NAME_LIBR "SLURM_EAR_MPI_VERSION"


#define SCHED_LOADER_VERBOSE "SLURM_LOADER_VERBOSE"
#define SCHED_EAR_SHOW_SIGNATURES "SLURM_EAR_SHOW_SIGNATURES"
#define SCHED_EAR_POWER_POLICY "SLURM_EAR_POWER_POLICY"
#define SCHED_EAR_MIN_PERC_MPI "SLURM_EAR_MIN_PERC_MPI"
#define SCHED_EAR_RED_FREQ_IN_MPI "SLURM_EAR_RED_FREQ_IN_MPI"


#define SCHED_JOB_ID  "SLURM_JOB_ID"
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

#endif //CONFIG_ENVIRONMENT_H
