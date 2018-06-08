/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, transparent and ligth-weigth solution for
*   Energy management.
*
*       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*   BSC Contact     mailto:ear-support@bsc.es
*   Lenovo contact  mailto:hpchelp@lenovo.com
*
*   EAR is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   EAR is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with EAR; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*   The GNU LEsser General Public License is contained in the file COPYING
*/
#include <sys/types.h>
#include <sys/stat.h>

#include <common/config.h>
#include <common/states.h>
#define MAX_PATH_SIZE 256

static char app_to_eard[MAX_PATH_SIZE];
static char eard_to_app[MAX_PATH_SIZE];
int create_app_connection(char *root)
{
	int fd;
	mode_t old_mask;
    sprintf(app_to_eard,"%s/.app_to_eard",root);
    sprintf(eard_to_app,"%s/.eard_to_app",root);
	old_mask=umask(0);
	// app_to_eard is used to send requests from app to eard
    if (mknod(app_to_eard,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
        if (errno!=EEXIST){
			return EAR_ERROR;
        }
    }
	if (mknod(eard_to_app,S_IFIFO|S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP|S_IROTH|S_IWOTH,0)<0){
        if (errno!=EEXIST){
            return EAR_ERROR;
        }
    }
	fd=open(app_to_eard,O_RDWR);
	umask(old_mask);
	return fd;
}

/** Returns the energy in mJ and the time in ms  */
int ear_energy(ulong *energy_mj,ulong *time_ms)
{
}

/** Releases resources to connect with applications */
int dispose_app_connection()
{
}
