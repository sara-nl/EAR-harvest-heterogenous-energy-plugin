/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <common/config.h>
#include <common/system/file.h>
#include <common/system/symplug.h>
#include <common/string_enhanced.h>
#include <library/loader/loader.h>
#include <library/loader/module_mpi.h>
#include <library/loader/module_common.h>

static mpic_t next_mpic;
static mpif_t next_mpif;
mpic_t ear_mpic;
mpif_t ear_mpif;

void (*func_con) (void);
void (*func_des) (void);



static void module_mpi_get_libear(char *path_lib_so,char *libhack,char *path_so, int *lang_c, int *lang_f)
{
	static char buffer[4096];
	char *extension = NULL;
	char *vers = NULL;
	int len = 4096;
	int fndi = 0;
	int fndo = 0;
	int fndm = 0;

	*lang_c = 0;
	*lang_f = 0;
	if (path_lib_so != NULL) verbose(2,"LOADER path_lib_so %s",path_lib_so);
	if (libhack != NULL) verbose(2,"LOADER  using HACK %s",libhack);

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
	extension = INTEL_EXT;

	if (fndo) {
		extension = OPENMPI_EXT;
	}

	if ((vers = getenv(FLAG_NAME_LIBR)) != NULL) {
		if (strlen(vers) > 0) {
			sprintf(buffer, "%s.so", vers);		
			extension = buffer;
		}
	}
  if (path_lib_so != NULL) verbose(2,"LOADER: path %s rellibname %s extension %s",path_lib_so, REL_NAME_LIBR, extension);

	//
	if (libhack == NULL){	
		sprintf(path_so, "%s/%s.%s", path_lib_so, REL_NAME_LIBR, extension);
	}else{
		sprintf(path_so, "%s", libhack);
	}

	verbose(2,"LOADER: lib path %s",path_so);
	
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

#if 0
static void module_mpi_dlsym_next()
{
	verbose(2, "LOADER: module_mpi_dlsym_next loading library");
	symplug_join(RTLD_NEXT, (void **) &ear_mpic, mpic_names, MPIC_N);
	symplug_join(RTLD_NEXT, (void **) &ear_mpif, mpif_names, MPIF_N);
	verbose(3, "LOADER: dlsym for C init returned %p", ear_mpic.Init);
	verbose(3, "LOADER: dlsym for F init returned %p", ear_mpif.init);
}
#endif

static int module_mpi_dlsym(char *path_so, int lang_c, int lang_f)
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
	verbose(3, "LOADER: MPI (C) next symbols loaded, Init address is %p", next_mpic.Init);
	verbose(3, "LOADER: MPI (F) next symbols loaded, Init address is %p", next_mpif.init);

	//
	libear = dlopen(path_so, RTLD_NOW | RTLD_GLOBAL);
	if (libear == NULL){
		error("LOADER: dlopen error %s", dlerror());
	}else{
		verbose(3, "LOADER: dlopen returned %p", libear);
	}
	
	void (*ear_mpic_setnext) (mpic_t *) = NULL;
	void (*ear_mpif_setnext) (mpif_t *) = NULL;

	if (libear != NULL)
	{
		ear_mpic_setnext = dlsym(libear, "ear_mpic_setnext");
		ear_mpif_setnext = dlsym(libear, "ear_mpif_setnext");

		if (ear_mpic_setnext == NULL && lang_c) {
			lang_c = 0;
		}
		if (ear_mpif_setnext == NULL && lang_f) {
			lang_f = 0;
		}
		if (!lang_c && !lang_f) {
			verbose(1, "LOADER: the loaded library does not have MPI symbols");
			dlclose(libear);
			libear = NULL;
		}
	}

	if (libear != NULL && lang_c) symplug_join(libear, (void **) &ear_mpic, ear_mpic_names, MPIC_N);
	if (libear != NULL && lang_f) symplug_join(libear, (void **) &ear_mpif, ear_mpif_names, MPIF_N);

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
	if (!lang_c && !lang_f) {
		return 0;
	}
	
	if (ear_mpic_setnext != NULL) ear_mpic_setnext(&next_mpic);
	if (ear_mpif_setnext != NULL) ear_mpif_setnext(&next_mpif);

  func_con = dlsym(libear, "ear_constructor");
  func_des = dlsym(libear, "ear_destructor");

  verbose(3, "LOADER: function constructor %p", func_con);
  verbose(3, "LOADER: function destructor %p", func_des);

	
	return 1;
}

static int module_mpi_is()
{
	return !(dlsym(RTLD_DEFAULT, "MPI_Get_library_version") == NULL);
}

int module_mpi(char *path_lib_so,char *libhack)
{
	static char path_so[4096];
	int lang_c;
	int lang_f;

	verbose(3, "LOADER: loading module MPI");

	if (!module_mpi_is()) {
		verbose(3, "LOADER: no MPI detected");
		return 0;
	}

	//
	module_mpi_get_libear(path_lib_so,libhack,path_so, &lang_c, &lang_f);
	//
	if (!module_mpi_dlsym(path_so, lang_c, lang_f)){
		return 0;
	}
	if (atexit(module_mpi_destructor) != 0) {
    verbose(1, "LOADER: cannot set exit function");
  }
  if (func_con != NULL) {
    func_con();
  }

	return 1;
	
}

void module_mpi_destructor()
{
  verbose(3, "LOADER: loading module default (destructor)");

  if (func_des != NULL) {
    func_des();
  }
}

