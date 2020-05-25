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
#include <common/system/file.h>
#include <common/system/symplug.h>
#include <common/output/verbose.h>
#include <common/string_enhanced.h>
#include <common/config/config_env.h>
#include <library/loader/module_mpi.h>

static mpic_t next_mpic;
static mpif_t next_mpif;
mpic_t ear_mpic;
mpif_t ear_mpif;

static int module_file_exists(char *path)
{
	return (access(path, X_OK) == 0);
}

static void module_mpi_get_libear(char *path_so, int *lang_c, int *lang_f)
{
	static char buffer[4096];
	char *extension = NULL;
	char *path = NULL;
	char *hack = NULL;
	char *vers = NULL;
	int len = 4096;
	int fndi = 0;
	int fndo = 0;
	int fndm = 0;

	*lang_c = 0;
	*lang_f = 0;

	if ((hack = getenv(HACK_PATH_LIBR)) != NULL) {
		hack = getenv(HACK_PATH_LIBR);
	} else if ((path = getenv(VAR_INS_PATH)) == NULL) {
		verbose(2, "LOADER: installation path not found");
		return;
	}

	// Getting the library version
	MPI_Get_library_version(buffer, &len);
	// Passing to lowercase
	strtolow(buffer);

	if (strstr(buffer, "intel") != NULL) {
		fndi = 1;
	} else if (strstr(buffer, "open mpi") != NULL) {
		fndo = 1;
	} else if (strstr(buffer, "mvapich") != NULL) {
		fndm = 1;
	} else {
		verbose(2, "LOADER: no mpi version found");
		return;
	}
		
	verbose(2, "LOADER: mpi_get_version (intel: %d, open: %d, mvapich: %d)",
		fndi, fndo, fndm);

	//
	extension = "so";

	if (fndo) {
		extension = "ompi.so";
	}

	if ((vers = getenv(FLAG_NAME_LIBR)) != NULL) {
		if (strlen(vers) > 0) {
			sprintf(buffer, "%s.so", vers);		
			extension = buffer;
		}
	}

	//
	if (!hack) {
		sprintf(path_so, "%s/%s/%s.%s", path, REL_PATH_LIBR, REL_NAME_LIBR, extension);
	} else {
		sprintf(path_so, "%s/%s.%s", hack, REL_NAME_LIBR, extension);
	}

	// Last chance to force a concrete library file.
	if ((hack = getenv(HACK_FILE_LIBR)) != NULL) {
		sprintf(path_so, "%s", hack);
        }
	
	//if (!file_is_regular(path_so)) {
	if (!module_file_exists(path_so)) {
		verbose(0, "LOADER: impossible to find library '%s'", path_so);
		return;
	}

	//
	*lang_f = !fndi;
	*lang_c = 1;

	return;
}

static void module_mpi_dlsym(char *path_so, int lang_c, int lang_f)
{
	void **next_mpic_v = (void **) &next_mpic;
	void **next_mpif_v = (void **) &next_mpif;
	void **ear_mpic_v  = (void **) &ear_mpic;
	void **ear_mpif_v  = (void **) &ear_mpif;
	void *libear;
	int i;

	verbose(2, "LOADER: module_mpi_dlsym loading library %s (c: %d, f: %d)",
		path_so, lang_c, lang_f);

	symplug_join(RTLD_NEXT, (void **) &next_mpic, mpic_names, MPIC_N);
	symplug_join(RTLD_NEXT, (void **) &next_mpif, mpif_names, MPIF_N);
	verbose(3, "LOADER: dlsym for C init returned %p", next_mpic.Init);
	verbose(3, "LOADER: dlsym for F init returned %p", next_mpif.init);

	//
	libear = dlopen(path_so, RTLD_NOW | RTLD_GLOBAL);
	verbose(3, "LOADER: dlopen returned %p", libear);

	if (libear != NULL)
	{
		if (lang_c) {
			symplug_join(libear, (void **) &ear_mpic, ear_mpic_names, MPIC_N);
		}
		if (lang_f) {
			symplug_join(libear, (void **) &ear_mpif, ear_mpif_names, MPIF_N);
		}
	}

	//
	for(i = 0; i < MPIC_N; ++i)
	{
		if(ear_mpic_v[i] == NULL) {
			ear_mpic_v[i] = next_mpic_v[i];
		}
	}
	for(i = 0; i < MPIF_N; ++i)
	{
		if(ear_mpif_v[i] == NULL) {
			ear_mpif_v[i] = next_mpif_v[i];
		}
	}
	
	// Setting MPI next symbols
	if (libear != NULL)
	{
		void (*ear_mpic_setnext) (mpic_t *) = dlsym(libear, "ear_mpic_setnext");
		void (*ear_mpif_setnext) (mpif_t *) = dlsym(libear, "ear_mpif_setnext");
		ear_mpic_setnext(&next_mpic);
		ear_mpif_setnext(&next_mpif);
	}
}

static int module_mpi_is()
{
	return !(dlsym(RTLD_DEFAULT, "MPI_Get_library_version") == NULL);
}

static void module_mpi_init()
{
	char *verb;
	
	verbose(3, "LOADER: function module_mpi");
	if ((verb = getenv("SLURM_LOADER_VERBOSE")) != NULL)
	{
		VERB_SET_EN(1);
		VERB_SET_LV(atoi(verb));
	}
}

int module_mpi()
{
	static char path_so[4096];
	int lang_c;
	int lang_f;


	module_mpi_init();
	
	verbose(3, "module mpi in '%s'", program_invocation_name);

	if (!module_mpi_is()) {
		verbose(3, "LOADER: no MPI detected");
		return 0;
	}
	
	module_mpi_get_libear(path_so, &lang_c, &lang_f);
	module_mpi_dlsym(path_so, lang_c, lang_f);

	return 1;
}
