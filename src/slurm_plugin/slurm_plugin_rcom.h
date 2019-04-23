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

#ifndef EAR_SLURM_PLUGIN_REPORTS_H
#define EAR_SLURM_PLUGIN_REPORTS_H

#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_environment.h>

int plug_shared_readservs(spank_t sp, plug_serialization_t *sd);
int plug_shared_readfreqs(spank_t sp, plug_serialization_t *sd);
int plug_shared_readsetts(spank_t sp, plug_serialization_t *sd);

int plug_rcom_eard_job_start(spank_t sp, plug_serialization_t *sd);
int plug_rcom_eard_job_finish(spank_t sp, plug_serialization_t *sd);

int plug_rcom_eargmd_job_start(spank_t sp, plug_serialization_t *sd);
int plug_rcom_eargmd_job_finish(spank_t sp, plug_serialization_t *sd);

#endif //EAR_SLURM_PLUGIN_REPORTS_H
