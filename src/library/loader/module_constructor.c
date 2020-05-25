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

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <common/output/verbose.h>
#include <common/config/config_env.h>
#include <library/loader/module_constructor.h>

void (*func_con) (void);
void (*func_des) (void);

static int module_file_exists(char *path)
{
	return (access(path, X_OK) == 0);
}

static int module_constructor_dlsym(char *path_so)
{
	void *libear;
	char *hack;

	// Last chance to force a concrete library file.
	if ((hack = getenv(HACK_FILE_LIBR)) == NULL) {
		return 0;
        }

	sprintf(path_so, "%s", hack);
	verbose(2, "LOADER: module_constructor_dlsym loading library %s", path_so);
	
	//
	libear = dlopen(path_so, RTLD_NOW | RTLD_GLOBAL);
	verbose(3, "LOADER: dlopen returned %p", libear);

	if (libear != NULL) {
		func_con = dlsym(libear, "ear_constructor");
		func_des = dlsym(libear, "ear_destructor");
	}

	return 1;
}

void module_constructor()
{
	static char path_so[4096];

	verbose(3, "LOADER: function module_constructor");

	if (!module_constructor_dlsym(path_so)) {
		return;
	}

	if (atexit(module_destructor) != 0) {
		verbose(3, "LOADER: cannot set exit function");
	}
		
	
	if (func_con != NULL) {
		func_con();
	}
}

void module_destructor()
{
	verbose(3, "LOADER: function module_destructor");

	if (func_des != NULL) {
		func_des();
	}
}
