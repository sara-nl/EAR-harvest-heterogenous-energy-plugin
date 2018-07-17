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

#ifndef EAR_SLURM_PLUGIN_H
#define EAR_SLURM_PLUGIN_H

#include <slurm/slurm.h>
#include <slurm/spank.h>
#include <common/sizes.h>
#include <common/config.h>
#include <common/types/job.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>

#define PRODUCTION 		1
#define ESPANK_STOP 	-1
#define S_CTX_SRUN 		S_CTX_LOCAL
#define S_CTX_SBATCH	S_CTX_ALLOCATOR

// Verbosity
int EAR_VERBOSE_LEVEL = 0;
int verbosity = -1;

// Context
struct passwd *upw;
struct group *gpw;
uid_t uid;
gid_t gid;

// Buffers
char buffer1[SZ_PATH];
char buffer2[SZ_PATH];
char buffer3[SZ_PATH]; // helper buffer

// EARD variables
unsigned char eard_host[SZ_NAME_MEDIUM];
unsigned int  eard_port;
application_t eard_appl;

// EARGMD variables
unsigned char eargmd_host[SZ_NAME_MEDIUM];
unsigned int  eargmd_port;
unsigned int  eargmd_nods;

// Paths
char *etc_dir = NULL;
char *pre_dir = NULL;
char *tmp_dir = NULL;

#endif
