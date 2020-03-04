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
#define SHOW_DEBUGS
#include <stdlib.h>
#include <common/output/debug.h>
#include <common/system/symplug.h>
#include <library/api_loader/loader.h>

static char buffer1[4096];
static char buffer2[4096];
static char install[4096];
static char version[256];
int earf_loaded;
int earc_loaded;
mpic_t mpic_mpi;
mpif_t mpif_mpi;
earc_t mpic_ear;
earf_t mpif_ear;

void strntolow(char *string)
{
	while (*string) {
		*string = tolower((unsigned char) *string);
		string++;
	}
}

static int get_version(char *string, char *needle, char *buffer)
{
	char *p = strtok(string, " ");
	char *q;
	int found1 = 0;
	int found2 = 0;

	while (*p && found2 == 0) {
		if (strcmp(p, needle) == 0) {
			found1 = 1;
		}

		p = strtok(NULL, " ");

		if (*p && found1 == 1) {
			found2 = 1;
		}
	}

	if (found2 == 0) {
		return 0;
	}

	//
	found1 = 0;
	found2 = 0;

	//
	while (*p && found1 == 0) {
		if (*p == ' ' ||
			*p == '\t' ||
			*p == '\n' ||
			*p == ',' ||
			*p == '.' ||
			*p == ';' ||
			*p == ':') {
			p += 1;
		} else {
			found1 = 1;
		}
	}

	//
	q = p;

	while(*p && found2 == 0)
	{
		if (*p ==  ' ' ||
			*p == '\t' ||
			*p == '\n' ||
			*p ==  ',' ||
			*p ==  '.' ||
			*p ==  ';' ||
			*p == ':') {
			found2 = 1;
		} else {
			p += 1;
		}
	}

	*p = '\0';
	sprintf(buffer, "%s", q);

	return 1;
}

static int mpi_detect_intel()
{
	if (strstr(buffer1, "intel") != NULL)
	{
		get_version(strcpy(buffer2, buffer1), "library", version);
		return 1;
	}
	return 0;
}

static int mpi_detect_open()
{
	if (strstr(buffer1, "open mpi") != NULL)
	{
		get_version(strcpy(buffer2, buffer1), "ident:", version);
		return 1;
	}
	return 0;
}

static int mpi_detect_mvapich()
{
	if (strstr(buffer1, "mvapich") != NULL)
	{
		get_version(strcpy(buffer2, buffer1), "version", version);
		return 1;
	}
	return 0;
}

static void load_c_mpi_symbols()
{
	symplug_join(RTLD_NEXT, (void **) &mpic_mpi, mpic_names, mpic_n);
}

static void load_f_mpi_symbols()
{
	symplug_join(RTLD_NEXT, (void **) &mpif_mpi, mpif_names, mpif_n);
}

static void load_earc_symbols(char *path_so)
{
	debug("load_earc_symbols (%s)", path_so);
	symplug_open(path_so, (void **) &mpic_ear, earc_names, earc_n);
	// Symbols loaded
	earc_loaded = 1;
}

static void load_earf_symbols(char *path_so)
{
	debug("load_earf_symbols (%s)", path_so);
	symplug_open(path_so, (void **) &mpif_ear, earf_names, earf_n);
	// Symbols loaded
	earf_loaded = 1;
}

static void load_x_ear_symbols()
{
	int len = 4096;
	char *path;

	// Getting the library version
	MPI_Get_library_version(buffer1, &len);
	//debug("%s", buffer1);
	strntolow(buffer1);

	// Getting the installation path
	if ((path = getenv("EAR_INSTALL_PATH")) == NULL) {
		debug("installation path not found");
		return;
	}
	//
	sprintf(install, "%s/lib/libear.so", path);
	sprintf(install, "/home/xjaneas/git/ear_private/src/library/api/libinterface.so");
	sprintf(install, "/home/xjaneas/git/ear_private/src/library/libear.so");
	
	//
	if (mpi_detect_intel())
	{
		debug("Intel MPI detected, version '%s'", version);
		load_earc_symbols(install);
	} else if (mpi_detect_open()) {
		debug("Open MPI detected, version '%s'", version);
		load_earc_symbols(install);
		load_earf_symbols(install);
	} else if (mpi_detect_mvapich()) {
		debug("MVAPICH MPI detected, version '%s'", version);
		load_earc_symbols(install);
		load_earf_symbols(install);
	} else {
		debug("Unknown MPI detected");
	}

	return;
}

static int symbols_mpi()
{
	const char *sym_version[] = { "MPI_Get_library_version" };
	void *calls[1];
	symplug_join(RTLD_DEFAULT, calls, sym_version, 1);
	return (calls[0] != NULL);
}

void  __attribute__ ((constructor)) loader()
{
	if (symbols_mpi())
	{
		load_c_mpi_symbols();
		load_f_mpi_symbols();
		load_x_ear_symbols();
	}
	return;
}
