/**************************************************************
 * *   Energy Aware Runtime (EAR)
 * *   This program is part of the Energy Aware Runtime (EAR).
 * *
 * *   EAR provides a dynamic, transparent and ligth-weigth solution for
 * *   Energy management.
 * *
 * *       It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
 * *
 * *       Copyright (C) 2017
 * *   BSC Contact     mailto:ear-support@bsc.es
 * *   Lenovo contact  mailto:hpchelp@lenovo.com
 * *
 * *   EAR is free software; you can redistribute it and/or
 * *   modify it under the terms of the GNU Lesser General Public
 * *   License as published by the Free Software Foundation; either
 * *   version 2.1 of the License, or (at your option) any later version.
 * *
 * *   EAR is distributed in the hope that it will be useful,
 * *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 * *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * *   Lesser General Public License for more details.
 * *
 * *   You should have received a copy of the GNU Lesser General Public
 * *   License along with EAR; if not, write to the Free Software
 * *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * *   The GNU LEsser General Public License is contained in the file COPYING
 * */

#ifndef EAR_ERROR_H
#define EAR_ERROR_H

#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <common/output/output_conf.h>

int error_channel	__attribute__((weak)) = 2;
int error_enabled	__attribute__((weak)) = 1;
int error_timestamp   __attribute__((weak)) = 0;

static time_t error_time_log;\
static struct tm *error_tm_log;\
static char s_log[64];\

#if SHOW_ERRORS
#define error(...) \
	{ \
		if (error_timestamp){ \
        	time(&error_time_log);\
        	error_tm_log=localtime(&error_time_log);\
        	strftime(s_log, sizeof(s_log), "%c", error_tm_log);\
        	dprintf(error_channel, "%s:",s_log); \
		} \
		\
		if (error_enabled) { \
			dprintf(error_channel, "ERROR: " __VA_ARGS__); \
			dprintf(error_channel, "\n"); \
		} \
	}
#else
#define error(...)
#endif

// Set
#define ERROR_SET_FD(fd)	error_channel = fd;
#define ERROR_SET_MUTE()	error_enabled = 0;
#define ERROR_SET_TS(ts)	error_timestamp = ts;

// Log
#if SHOW_LOGS

#define log(...) \
	{ \
		syslog(LOG_DAEMON|LOG_ERR, "LOG: " __VA_ARGS__); \
	}

#define log_open(package)	openlog(package, LOG_PID|LOG_PERROR, LOG_DAEMON);
#define log_close()			closelog();
#else
#define log(...)

#define log_open(package);
#define log_close()
#endif

#endif //EAR_ERROR_H
