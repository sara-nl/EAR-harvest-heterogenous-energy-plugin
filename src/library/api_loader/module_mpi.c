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

#include <common/system/file.h>
#include <common/system/symplug.h>
#include <common/output/verbose.h>
#include <common/config/config_env.h>

static mpic_t next_mpic;
static mpif_t next_mpif;
mpic_t api_mpic;
mpif_t api_mpif;

static void module_mpi_get_libear(char *path_so, int *lang_c, int *lang_f)
{
	static char buffer[4096];
	char *library = NULL;
	char *path = NULL;
	int len = 4096;
	int fndi = 0;
	int fndo = 0;
	int fndm = 0;

	*lang_c = 0;
	*lang_f = 0;

	if ((path = getenv(VAR_INS_PATH)) == NULL) {
		verbose(2, "installation path not found");
		return;
	}

	// Getting the library version
	MPI_Get_library_version(buffer1, &len);
	// Passing to lowercase
	strntolow(buffer1);

	if (strstr(buffer, "intel") != NULL) {
		fndi = 1;
	} else if (strstr(buffer, "open mpi") != NULL) {
		fndo = 1;
	} else if (strstr(buffer, "mvapich") != NULL) {
		fndm = 1;
	} else {
		return;
	}

	//
	library = "libear.so";

	if (fndo) {
		library = "libear_ompi.so";
	}

	//
	sprintf(path_so, "%s/%s", path, library);
	verbose(2, "loading library %s", path_so);

	if (file_is_regular(buffer)) {
		return;
	}

	//
	*lang_f = !fndi;
	*lang_c = 1;

	return;
}

static void module_mpi_dlsym(char *path_so, int lang_c, int lang_f)
{
	void *libear;
	int i;

	verbose(2, "module_mpi_dlsym loading %s", path_so);

	if (lang_c) {
		symplug_join(RTLD_NEXT, (void **) &next_mpic, mpic_names, MPIC_N);
	}
	if (lang_f) {
		symplug_join(RTLD_NEXT, (void **) &next_mpif, mpif_names, MPIF_N);
	}

	//
	libear = dlopen(path_so, RTLD_NOW | RTLD_GLOBAL);

	if (libear != NULL) {
		symplug_join(libear, (void **) &api_mpic, api_mpic_names, API_MPIC_N);
		symplug_join(libear, (void **) &api_mpif, api_mpif_names, API_MPIF_N);
	}

	//
	for(i = 0; i < MPIC_N; ++i)
	{
		if(api_mpic[i] == NULL) {
			api_mpic[i] = next_mpic[i];
		}
	}
	for(i = 0; i < MPIF_N; ++i)
	{
		if(api_mpif[i] == NULL) {
			api_mpif[i] = next_mpif[i];
		}
	}

	// Setting MPI next symbols
	void (*api_mpic_setnext) (mpic_t *) = dlsym(libear, "api_mpic_setnext");
	void (*api_mpif_setnext) (mpif_t *) = dlsym(libear, "api_mpif_setnext");
	api_mpic_setnext(&next_mpic);
	api_mpif_setnext(&next_mpif);
}

static int module_mpi_is()
{
	return (dlsym(RTLD_DEFAULT, "MPI_Get_library_version") != NULL);
}

void module_mpi()
{
	static char path_so[4096];
	int lang_c;
	int lang_f;

	if (!module_mpi_is()) {
		return;
	}
	module_mpi_get_libear(path_so, &lang_c, &lang_f);
	module_mpi_dlsym(path_so, lang_c, lang_f);
}