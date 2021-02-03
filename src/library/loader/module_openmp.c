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
#include <library/loader/loader.h>
#include <library/loader/module_openmp.h>
#include <library/loader/module_common.h>

static void (*func_con) (void);
static void (*func_des) (void);


static int module_openmp_is()
{
	/* We manage OpenMP and MKL with same library */
	void * ompsym,*mklsym;
	ompsym = dlsym(RTLD_DEFAULT, "omp_get_max_threads") ;
	if (ompsym != NULL) return 1;
	mklsym = dlsym (RTLD_DEFAULT, "mkl_get_max_threads");
	if (mklsym != NULL) return 1;
	return 0;
}


static int module_constructor_dlsym(char *path_lib_so,char *libhack,char *path_so)
{
	void *libear;


  if ((path_lib_so == NULL) && (libhack == NULL)){
    verbose(0,"LOADER EAR path and EAR debug HACKS are NULL in CUDA module");
    return 0;
  }

  if (libhack == NULL){
    sprintf(path_so, "%s/%s.%s", path_lib_so, REL_NAME_LIBR, OPENMP_EXT);
  }else{
    sprintf(path_so, "%s", libhack);
  }
  

	verbose(2, "LOADER: loading library %s", path_so);

	if (!module_file_exists(path_so)) {
		verbose(0, "LOADER: impossible to find library '%s'", path_so);
		return 0;
	}
	
	//
	libear = dlopen(path_so, RTLD_NOW | RTLD_GLOBAL);
	verbose(3, "LOADER: dlopen returned %p", libear);

	if (libear == NULL) {
		verbose(0, "LOADER: dlopen failed %s", dlerror());
		return 0;
	}

	func_con = dlsym(libear, "ear_constructor");
	func_des = dlsym(libear, "ear_destructor");

	verbose(4, "LOADER: function constructor %p", func_con);
	verbose(4, "LOADER: function destructor %p", func_des);

	if (func_con == NULL && func_des == NULL) {
		dlclose(libear);
		return 0;
	}

	return 1;
}

int module_constructor_openmp(char *path_lib_so,char *libhack)
{
	static char path_so[4096];

	verbose(3, "LOADER: loading module openmp (constructor)");
  if (!module_openmp_is()){
    verbose(3, "LOADER: App is not an OpenMP app");
    return 0;
  }

	if (!module_constructor_dlsym(path_lib_so,libhack,path_so)) {
		return 0;
	}
	if (atexit(module_destructor_openmp) != 0) {
		verbose(1, "LOADER: cannot set exit function");
	}
	if (func_con != NULL) {
		func_con();
	}

	return 1;
}

void module_destructor_openmp()
{
	verbose(3, "LOADER: loading module openmp (destructor)");

	if (func_des != NULL) {
		func_des();
	}
}
