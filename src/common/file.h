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

#include <common/states.h>

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

/** */
int file_is_regular(const char *path);

/** */
state_t file_read(const char *path, char *buffer, size_t size);

/** */
state_t file_write(const char *path, const char *buffer, size_t size);

/** */
state_t file_clean(const char *path);

#endif

