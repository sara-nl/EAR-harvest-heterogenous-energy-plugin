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

#include "types/generic.h"

/**@{ Adds commas to numbers. It is printed in internal buffer and a pointer to
*   this buffer is returned. So do whatever you want before call again one
*   of these functions. */
char *add_point_ull(ull number);
char *add_point_ulong(ulong number);
char *add_point_uint(uint number); /**@}*/

/**@{ Prints a type splitted by columns. You have to call 'set_spacing_digits',
*   every time you want to define the number of characters per column. You can
*   define just once, or iteratively to use different column lengths. */
void set_spacing_digits(uint digits);
void print_spacing_digits(uint digits);
void print_spacing_ull(ull number);
void print_spacing_ulong(ulong number);
void print_spacing_uint(uint number);
void print_spacing_int(int number);
void print_spacing_string(char* string);
void print_spacing_string_align_left(char* string, uint left_spaces); /**@}*/

/** Cleans the character pointed by 'chr', adding an '\0' in its position. */
char* strclean(char *string, char chr);

/** Converts a string to upper case. */
void strtoup(char *string);

#else
#endif
