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
#ifndef _FILE_H
#define _FILE_H

#include <string.h>
#include <common/sizes.h>
#include <common/states.h>

#define FC_MAX_OBJECTS	10
#define F_WR 			O_WRONLY
#define F_CR 			O_CREAT
#define F_TR			O_TRUNC
#define F_UR 			S_IRUSR
#define F_UW 			S_IWUSR
#define F_UX 			S_IXUSR
#define F_GR 			S_IRGRP
#define F_GW 			S_IWGRP
#define F_GX 			S_IXGRP
#define F_OR 			S_IROTH
#define F_OW 			S_IWOTH
#define F_OX 			S_IXOTH

typedef struct file_chkp {
	char *address[FC_MAX_OBJECTS];
	size_t size[FC_MAX_OBJECTS];
	char path[SZ_PATH];
	int n;
} file_chkp_t;

// Format: file_chkp_t *fc, (object_t) obj
#define file_chkp_add_vector(fc, vec, nelems) \
	file_chkp_add(fc, (char *) vec, nelems * sizeof(obj[0]));

#define file_chkp_add_object(fc, obj) \
	file_chkp_add(fc, (char *) &obj, sizeof(obj));

// Format: file_chkp_t *fc, (char *) str
#define file_chkp_add_string(fc, str) \
	file_chkp_add(fc, str, strlen(str));

#define file_chkp_addm(fc, fmt, nargs, ...) \
	file_chkp_add_multiple(fc, fmt, strlen(fmt), nargs, __VA_ARGS__);

/** */
int file_is_regular(const char *path);

/** */
state_t file_read(const char *path, char *buffer, size_t size);

/** */
state_t file_write(const char *path, const char *buffer, size_t size);

/** */
state_t file_clean(const char *path);

/** */
state_t file_chkp_init(file_chkp_t *fc, char *path, char *name);

/** */
state_t file_chkp_add(file_chkp_t *fc, char *object, size_t size);

/** */
state_t file_chkp_add_multiple(file_chkp_t *fc, char *format, int num_args, ...);

/** */
state_t file_chkp_read(file_chkp_t *fc);

/** */
state_t file_chkp_write(file_chkp_t *fc);

/** Locks the file for writting in a fixe dpart */
int file_lock(int fd);

/** Unlocks the file */
int file_unlock(int fd);

/** Opens a file with O_EXCL, only one process per node will do that */
int file_lock_master(char *lock_file_name);

/** Releases a lock file */
void file_unlock_master(int fd,char *lock_file_name);

/** Creates a file to be used  as lock. It doesn't locks the file */
int file_lock_create(char *lock_file_name);

/** Closes and removes the lock file */
void file_lock_clean(int fd,char *lock_file_name);

#endif

