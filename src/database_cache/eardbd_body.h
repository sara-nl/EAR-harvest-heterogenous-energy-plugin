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

#define line "---------------------------------------------------------------"
#define col1 "\x1b[35m"
#define col2 "\x1b[0m"

// Meaning:
//  - m: master
//  - l: line
//  - p: shows the process
//  - 0: no arguments
//  - 1: arguments

#define printp0(format) \
	verb(0, "%s, %s", str_who[mirror_iam], format);

#define printp1(format, ...) \
	verb(0, "%s, " format, str_who[mirror_iam], __VA_ARGS__);

#define printpl0(format) \
	verb(0, "%s, " format "\n", str_who[mirror_iam]);

#define printpl1(format, ...) \
	verb(0, "%s, " format "\n", str_who[mirror_iam], __VA_ARGS__);

#define printm1(...) \
    if (!forked || master_iam) { \
		verb(0, __VA_ARGS__); \
    }

#define printl0() \
		verb(0, col1 line col2);

#define printml1(...) \
    if (!forked || master_iam) { \
        dprintf(verb_channel, col1 line "\n" __VA_ARGS__); \
        dprintf(verb_channel, col2 "\n"); \
	}

void body();

void release();

void dream();

#endif //EAR_EARDBD_BODY_H
