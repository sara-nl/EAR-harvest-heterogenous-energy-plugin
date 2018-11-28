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
#ifndef EAR_EARDBD_BODY_H
#define EAR_EARDBD_BODY_H

#define line "---------------------------------------------------------------\n"
#define col1 "\x1b[35m"
#define col2 "\x1b[0m"

// Meaning:
//  - m: master
//  - l: line
//  - p: shows the process
//  - 0: no arguments
//  - 1: arguments

#define verwho0(format) \
    fprintf(stderr, "%s, %s \n", str_who[mirror_iam], format);

#define verwho1(format, ...) \
    fprintf(stderr, "%s, " format "\n", str_who[mirror_iam], __VA_ARGS__);

#define printpl0(format) \
	fprintf(stderr, "%s, " format "\n", str_who[mirror_iam]);

#define printpl1(format, ...) \
	printl0(); \
	fprintf(stderr, "%s, " format "\n", str_who[mirror_iam], __VA_ARGS__);

#define printm1(...) \
    if (!forked || master_iam) { \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
    }

#define printmc1(...) \
    if (!forked || master_iam) { \
        fprintf(stderr, col1 __VA_ARGS__); \
        fprintf(stderr, col2 "\n"); \
    }

#define printl0() \
        fprintf(stderr, col1 line col2);

#define printml1(...) \
    if (!forked || master_iam) { \
        fprintf(stderr, col1 line __VA_ARGS__); \
        fprintf(stderr, col2 "\n"); \
    }

void body();

void release();

void dream();

#endif //EAR_EARDBD_BODY_H
