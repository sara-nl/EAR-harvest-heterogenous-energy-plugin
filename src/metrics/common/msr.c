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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <common/sizes.h>
#include <common/states.h>
#include <metrics/common/msr.h>

#define MSR_MAX	4096

static pthread_mutex_t lock_gen = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock_cpu[MSR_MAX];
static int counters[MSR_MAX];
static int init[MSR_MAX];
static int fds[MSR_MAX];

static struct error_s {
	char *lock;
	char *cpu_invalid;
	char *cpu_uninitialized;
} Error = {
	.lock        = "error during pthread_mutex",
	.cpu_invalid = "cpu number is out of range",
	.cpu_uninitialized = "cpu MSR is not initialized",
};

#define return_unlock_msg(s, message, lock) \
	pthread_mutex_unlock(lock); \
	return_msg(s, message);

#define return_unlock(s, lock) \
	pthread_mutex_unlock(lock); \
	return s;

/* */
state_t msr_open(uint cpu)
{
	char msr_file_name[SZ_PATH_KERNEL];

	if (cpu >= MSR_MAX) {
		return_msg(EAR_BAD_ARGUMENT, Error.cpu_invalid);
	}

	// General exclusion
	while (pthread_mutex_trylock(&lock_gen));

	if (init[cpu] == 0) {
		if (pthread_mutex_init(&lock_cpu[cpu], NULL) != 0) {
			return_unlock_msg(EAR_ERROR, Error.lock, &lock_gen);
		}
	}

	init[cpu] = 1;
	pthread_mutex_unlock(&lock_gen);

	// CPU exclusion
	while (pthread_mutex_trylock(&lock_cpu[cpu]));
	
	if (counters[cpu] == 0) {
		sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
		fds[cpu] = open(msr_file_name, O_RDWR);
	}
		
	if (fds[cpu] < 0) {
		return_unlock_msg(EAR_SYSCALL_ERROR, strerror(errno), &lock_cpu[cpu]);
	}
	
	counters[cpu] += 1;
	return_unlock(EAR_SUCCESS, &lock_cpu[cpu]);
}

/* */
state_t msr_close(uint cpu)
{
	if (cpu >= MSR_MAX) {
		return_msg(EAR_BAD_ARGUMENT, Error.cpu_invalid);
	}
	
	if (counters[cpu] == 0) {
		return_msg(EAR_NOT_INITIALIZED, Error.cpu_uninitialized);
	}
	
	while (pthread_mutex_trylock(&lock_cpu[cpu]));
	
	if (counters[cpu] == 0) {
		return_unlock_msg(EAR_NOT_INITIALIZED, Error.cpu_uninitialized, &lock_cpu[cpu]);
	}

	counters[cpu] -= 1;

	if (counters[cpu] == 0) {
		close(fds[cpu]);
		fds[cpu] = -1;
	}
	
	return_unlock(EAR_SUCCESS, &lock_cpu[cpu]);
}

/* */
state_t msr_read(uint cpu, void *buffer, size_t size, off_t offset)
{
	if (cpu >= MSR_MAX) {
		return_msg(EAR_BAD_ARGUMENT, Error.cpu_invalid);
	}
	
	if (counters[cpu] == 0 || fds[cpu] < 0) {
		return_msg(EAR_NOT_INITIALIZED, Error.cpu_uninitialized);
	}

	#ifdef MSR_LOCK
	while (pthread_mutex_trylock(&lock_cpu[cpu]));
	#endif

	if (pread(fds[cpu], buffer, size, offset) != size) {
		#ifdef MSR_LOCK
		return_unlock_msg(EAR_SYSCALL_ERROR, strerror(errno), &lock_cpu[cpu]);
		#else
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
		#endif
	}

	#ifdef MSR_LOCK
	return_unlock(EAR_SUCCESS, &lock_cpu[cpu]);
	#else
	return EAR_SUCCESS;
	#endif
}

/* */
state_t msr_write(uint cpu, const void *buffer, size_t size, off_t offset)
{
	if (cpu >= MSR_MAX) {
		return_msg(EAR_BAD_ARGUMENT, Error.cpu_invalid);
	}

	if (counters[cpu] == 0 || fds[cpu] < 0) {
		return_msg(EAR_NOT_INITIALIZED, Error.cpu_uninitialized);
	}

	#ifdef MSR_LOCK
	while (pthread_mutex_trylock(&lock_cpu[cpu]));
	#endif

	if (pwrite(fds[cpu], buffer, size, offset) != size) {
		#ifdef MSR_LOCK
		return_unlock_msg(EAR_SYSCALL_ERROR, strerror(errno), &lock_cpu[cpu]);
		#else
		return_msg(EAR_SYSCALL_ERROR, strerror(errno));
		#endif
	}

	#ifdef MSR_LOCK
	return_unlock(EAR_SUCCESS, &lock_cpu[cpu]);
	#else
	return EAR_SUCCESS;
	#endif
}
