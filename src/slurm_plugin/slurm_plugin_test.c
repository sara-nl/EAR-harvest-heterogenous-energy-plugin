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

#include <common/file.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_test.h>
#include <slurm_plugin/slurm_plugin_environment.h>

char buffer1[SZ_PATH];
char buffer2[SZ_PATH];

static void writenv(spank_t sp, char *var, int fd)
{
	if (getenv_agnostic(sp, var, buffer1, SZ_PATH)) {
		sprintf(buffer2, "%s=%s\n", var, buffer1);
	} else {
		sprintf(buffer2, "%s=NULL\n", var, buffer1);
	}
	
	write(fd, buffer2, strlen(buffer2));
}

void plug_test(spank_t sp)
{
	ssize_t w;
	int fd;

	if (!isenv_agnostic(sp, "SLURM_PROCID", "0")) {
		return;
	}
	
	// Opening file
	getenv_agnostic(sp, "SLURM_SUBMIT_DIR", buffer2, SZ_PATH);
	getenv_agnostic(sp, "SLURM_JOBID", buffer1, SZ_PATH);
	sprintf(buffer2, "%s/earplug.%s", buffer2, buffer1);
	getenv_agnostic(sp, "SLURM_STEPID", buffer1, SZ_PATH);
	sprintf(buffer2, "%s.%s.test", buffer2, buffer1);

	fd = open(buffer2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	// Writing
	writenv(sp, "EAR_VERBOSE", fd);
	writenv(sp, "EAR_POWER_POLICY", fd);
	writenv(sp, "EAR_POWER_POLICY_TH", fd);
	writenv(sp, "EAR_FREQUENCY", fd);
	writenv(sp, "EAR_P_STATE", fd);
	writenv(sp, "EAR_LEARNING_PHASE", fd);
	writenv(sp, "EAR_ENERGY_TAG", fd);
	writenv(sp, "EAR_USER_DB_PATHNAME", fd);
	writenv(sp, "EAR_PERFORMANCE_PENALTY", fd);
	writenv(sp, "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN", fd);
	writenv(sp, "EAR_APP_NAME", fd);
	writenv(sp, "EAR_TMP", fd);
	writenv(sp, "LD_PRELOAD", fd);

	close(fd);
}
