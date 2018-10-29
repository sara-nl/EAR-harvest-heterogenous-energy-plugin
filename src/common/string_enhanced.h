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

#ifndef _STRING_ENHANCED_H_
#define _STRING_ENHANCED_H_

#include <linux/limits.h>

#define STR_MAX_COLUMNS		20
#define STR_SIZE_BUFFER		PIPE_BUF
#define STR_SYMBOL			"||"
#define STR_RED				"<red>"
#define STR_GRE 			"<gre>"
#define STR_YLW 			"<ylw>"
#define STR_BLU 			"<blu>"
#define STR_MGT 			"<mgt>"
#define STR_CYA 			"<cya>"
#define STR_COL_CHR			5
#define STR_MODE_TAB_DEF	1
#define STR_MODE_TAB_COL	2
#define STR_MODE_CSV_DEF	3

char tprintf_ibuf[STR_SIZE_BUFFER];
char tprintf_obuf[STR_SIZE_BUFFER];

#define tprintf(...) \
	snprintf(tprintf_ibuf, STR_SIZE_BUFFER-1, __VA_ARGS__); \
	tprintf_format();

/** **/
int tprintf_init(FILE *stream, int mode, char *format);

/** **/
int tprintf_format();

/** Cleans the character pointed by 'chr', adding an '\0' in its position. */
char* strclean(char *string, char chr);

/** Converts a string to upper case. */
void strtoup(char *string);

/** Removes characters c from string s. */
void remove_chars(char *s, char c);

#endif
