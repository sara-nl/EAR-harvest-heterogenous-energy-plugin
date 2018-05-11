/**************************************************************
*   Energy Aware Runtime (EAR)
*   This program is part of the Energy Aware Runtime (EAR).
*
*   EAR provides a dynamic, dynamic and ligth-weigth solution for
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

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

static struct flock lock;

int create_lock(char *lock_file_name)
{
    int fd = open(lock_file_name, O_WRONLY|O_CREAT,S_IWUSR);
	if (fd<0) return fd;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
}
int lock_file(int fd)
{
	if (fd>=0){
		lock.l_type = F_WRLCK;
		return fcntl(fd, F_SETLKW, &lock);
	}else return -1;
}
int unlock_file(int fd)
{
	if (fd>=0){
		lock.l_type = F_UNLCK;
		return fcntl(fd, F_SETLKW, &lock);
	}else return -1;
}
void lock_dispose(int fd,char *lock_file_name)
{
	if (fd>=0){
		close(fd);
		unlink(lock_file_name);
	}
}

int lock_master(char *lock_file_name)
{
	int fd=open(lock_file_name,O_WRONLY|O_CREAT|O_EXCL,S_IWUSR);
	return fd;
}
void unlock_master(int fd,char *lock_file_name)
{
	close(fd);
	unlink(lock_file_name);
	
}

