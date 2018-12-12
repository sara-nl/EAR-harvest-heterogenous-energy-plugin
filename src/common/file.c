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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <common/file.h>

static struct flock lock;

int file_lock(int fd)
{
	if (fd>=0){
		lock.l_type = F_WRLCK;
		return fcntl(fd, F_SETLKW, &lock);
	}else return -1;
}

int file_unlock(int fd)
{
	if (fd>=0){
		lock.l_type = F_UNLCK;
		return fcntl(fd, F_SETLKW, &lock);
	}else return -1;
}

int file_lock_master(char *lock_file_name)
{
	int fd=open(lock_file_name,O_WRONLY|O_CREAT|O_EXCL,S_IWUSR);
	return fd;
}

void file_unlock_master(int fd,char *lock_file_name)
{
	close(fd);
	unlink(lock_file_name);
}

void file_lock_clean(int fd,char *lock_file_name)
{
	if (fd>=0){
		close(fd);
		unlink(lock_file_name);
	}
}

int file_lock_create(char *lock_file_name)
{
	int fd = open(lock_file_name, O_WRONLY | O_CREAT, S_IWUSR);
	if (fd<0) return fd;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
}

int file_is_regular(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

ssize_t file_size(char *path)
{
	int fd = open(path, O_RDONLY);
	ssize_t size;

	if (fd < 0){
		state_return_msg((ssize_t) EAR_OPEN_ERROR, errno, strerror(errno));
	}

	size = lseek(fd, 0, SEEK_END);
	close(fd);

	return size;
}

state_t file_read(const char *path, char *buffer, size_t size)
{
	int fd = open(path, O_RDONLY);
	ssize_t r;

	if (fd < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	while ((r = read(fd, buffer, size)) > 0){
		size = size - r;
	}

	close(fd);

	if (r < 0) {
		state_return_msg(EAR_READ_ERROR, errno, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t file_write(const char *path, const char *buffer, size_t size)
{
	int fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	ssize_t w;

	if (fd < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	while ((w = write(fd, buffer, size)) > 0) {
		size = size - w;
	}

	close(fd);

	if (w < 0) {
		state_return_msg(EAR_WRITE_ERROR, errno, strerror(errno));
	}

	state_return(EAR_SUCCESS);
}

state_t file_clean(const char *path)
{
	if (remove(path) != 0) {
		state_return_msg(EAR_ERROR, errno, strerror(errno));
	}
	state_return(EAR_SUCCESS);
}

state_t file_chkp_init(file_chkp_t *fc, char *path, char *name)
{
	sprintf(fc->path, "%s/%s.chkp", path, name);
	fc->n = 0;
}

state_t file_chkp_add(file_chkp_t *fc, char *object, size_t size)
{
	if (fc->n == FC_MAX_OBJECTS) {
		return EAR_BAD_ARGUMENT;
	}

	fc->address[fc->n] = object;
	fc->size[fc->n] = size;
	fc->n += 1;

	return EAR_SUCCESS;
}

state_t file_chkp_add_multiple(file_chkp_t *fc, char *format, int num_args, ...)
{
	va_list list;
	char *p;
	int i;

	fprintf(stderr, "num args: %d\n", num_args);

	va_start(list, num_args);
	for(i = 0; i < num_args; i++)
	{
		p = va_arg(list, char *);
		fprintf(stderr, "'%s'\n", p);

		if (format[i] == 'S') {
			file_chkp_add_string(fc, p);
		} else {
			file_chkp_add_object(fc, p);
		}
	}
	va_end(list);

	return EAR_SUCCESS;
}

state_t file_chkp_read(file_chkp_t *fc)
{
	int fd = open(fc->path, O_RDONLY);
	size_t size;
	ssize_t r;
	int n = 0;

	if (fd < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	// Reading the 'n'
	r = read(fd, &fc->n, sizeof(int));

	fprintf(stderr, "n: %d\n", fc->n);

	// Reading the 'size[n]'
	r = read(fd, fc->size, sizeof(size_t) * fc->n);

	// Reading the content of 'address[n]'
	while (n < fc->n)
	{
		size = fc->size[n];
		fprintf(stderr, "size: %lu\n", size);
		r = read(fd, fc->address[n], size);
		n += 1;
	}

	close(fd);

	state_return(EAR_SUCCESS);
}

state_t file_chkp_write(file_chkp_t *fc)
{
	int fd = open(fc->path, F_WR | F_CR, F_UR | F_UW);
	size_t size;
	ssize_t w;
	int n = 0;

	if (fd < 0) {
		state_return_msg(EAR_SOCK_OP_ERROR, errno, strerror(errno));
	}

	// Writing the 'n'
	w = write(fd, &fc->n, sizeof(int));

	// Writing the 'size[n]'
	w = write(fd, fc->size, sizeof(size_t) * fc->n);

	// Writing the content of 'address[n]'
	while (n < fc->n)
	{
		size = fc->size[n];
		w = write(fd, fc->address[n], size);
		n += 1;
	}

	close(fd);

	state_return(EAR_SUCCESS);
}