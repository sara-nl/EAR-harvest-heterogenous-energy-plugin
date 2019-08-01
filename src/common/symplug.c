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

#include <dlfcn.h>
#define SHOW_DEBUGS 1
#include <common/symplug.h>
#include <common/includes.h>

static void symplug_join(void *handle, void *calls[], const char *names[], uint n)
{
	uint i;

	for (i = 0; i < n; ++i) {
		calls[i] = dlsym(handle, names[i]);
		if (calls[i]!=NULL) {
			 debug("symbol %s found",names[i]);
		}
	}
}

state_t symplug_open(char *path, void *calls[], const char *names[], uint n)
{
	void *handle = dlopen(path, RTLD_LOCAL | RTLD_LAZY);

	if (!handle) {
		state_return_msg(EAR_ERROR, 0, "error during dlopen(), missing symbol?");
	}

	symplug_join(handle, calls, names, n);

	return EAR_SUCCESS;
}
