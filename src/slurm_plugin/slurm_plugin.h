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

#include <pwd.h>
#include <grp.h>
#include <slurm/slurm.h>
#include <slurm/spank.h>
#include <common/sizes.h>
#include <common/config.h>
#include <common/types/job.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>

#define PRODUCTION	 1
#define ESPANK_STOP	-1
#define S_CTX_SRUN	S_CTX_LOCAL
#define S_CTX_SBATCH	S_CTX_ALLOCATOR

// Verbosity
extern int verbosity;

// Buffers
extern char buffer1[SZ_PATH];
extern char buffer2[SZ_PATH];
extern char buffer3[SZ_PATH]; // helper buffer

#endif
