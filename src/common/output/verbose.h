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

#ifndef EAR_VERBOSE_H
#define EAR_VERBOSE_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <common/output/output_conf.h>

int verb_level		__attribute__((weak)) = 0;
int verb_channel	__attribute__((weak)) = 2;
int verb_timestamp	__attribute__((weak)) = 1;
int warn_channel	__attribute__((weak)) = 2;

static time_t verbose_time_log;
static struct tm *verbose_tm_log;
static char s_log[64];



#define verbose(v, ...) \
	if (v <= verb_level) { \
		if (verb_timestamp){ \
        time(&verbose_time_log);\
        verbose_tm_log=localtime(&verbose_time_log);\
        strftime(s_log, sizeof(s_log), "%c", verbose_tm_log);\
        dprintf(verb_channel, "%s:",s_log); \
		}\
        dprintf(verb_channel, __VA_ARGS__); \
        dprintf(verb_channel, "\n"); \
	}

// Compact version
#define verb(v, ...) \
	verbose(v, __VA_ARGS__)

// No new line version
#define verbosen(v, ...) \
	if (v <= verb_level) { \
        if (verb_timestamp){ \
        time(&verbose_time_log);\
        verbose_tm_log=localtime(&verbose_time_log);\
        strftime(s_log, sizeof(s_log), "%c", verbose_tm_log);\
        dprintf(verb_channel, "%s:",s_log); \
        }\
        dprintf(verb_channel, __VA_ARGS__); \
	}

// Set
#define VERB_SET_FD(fd) \
	verb_channel = fd;

#define WARN_SET_FD(fd) \
	warn_channel = fd;

#define VERB_SET_LVL(level) \
	verb_level = level;

#define VERB_SET_TS(ts) verb_timestamp=ts;

// Warnings
#if SHOW_WARNINGS0
#define warning(...) \
{\
        if (verb_timestamp){ \
        time(&verbose_time_log);\
        verbose_tm_log=localtime(&verbose_time_log);\
        strftime(s_log, sizeof(s_log), "%c", verbose_tm_log);\
        dprintf(warn_channel, "%s:",s_log); \
        }\
	 	dprintf(warn_channel, __VA_ARGS__);\
}

#else
#define warning(...)
#endif

#if SHOW_WARNINGS1
#define warning1(...) \
{ \
        if (verb_timestamp){ \
        time(&verbose_time_log);\
        verbose_tm_log=localtime(&verbose_time_log);\
        strftime(s_log, sizeof(s_log), "%c", verbose_tm_log);\
        dprintf(warn_channel, "%s:",s_log); \
        }\
		dprintf(warn_channel, __VA_ARGS__); \
	}
#else
#define warning1(...)
#endif

#endif //EAR_VERBOSE_H
