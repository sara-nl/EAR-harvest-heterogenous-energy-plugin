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

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <common/string_enhanced.h>

static char _buffer[3][64];
static uint _digits;
static uint _i;

static void _add_point_ull (ull number)
{
    if (number < 0) {
        printf ("-");
        _add_point_ull(-number);
        return;
    }

    if (number < 1000) {
        sprintf(_buffer[_i], "%s%llu", _buffer[_i], number);
        return;
    }

    _add_point_ull(number / 1000);
    sprintf(_buffer[_i], "%s.%03llu", _buffer[_i], (number % 1000));
}

char* add_point_ull(ull number)
{
	_i = _i + 1 - (_i == 2) * 3;
	 _buffer[_i][0] = '\0';
	_add_point_ull(number);
	return _buffer[_i];
}

static void _add_point_ulong (ulong number)
{
    if (number < 0) {
        printf ("-");
        _add_point_ulong(-number);
        return;
    }

    if (number < 1000) {
        sprintf(_buffer[_i], "%s%lu", _buffer[_i], number);
        return;
    }

    _add_point_ulong(number / 1000);
    sprintf(_buffer[_i], "%s.%03lu", _buffer[_i], (number % 1000));
}

char* add_point_ulong(ulong number)
{
    _buffer[_i][0] = '\0';
    _add_point_ulong(number);
    return _buffer[_i];
}

static void _add_point_uint (uint number)
{
    if (number < 0) {
        printf ("-");
        _add_point_uint(-number);
        return;
    }

    if (number < 1000) {
        sprintf(_buffer[_i], "%s%u", _buffer[_i], number);
        return;
    }

    _add_point_uint(number / 1000);
    sprintf(_buffer[_i], "%s.%03u", _buffer[_i], (number % 1000));
}

char* add_point_uint(uint number)
{
    _buffer[_i][0] = '\0';
    _add_point_uint(number);
    return _buffer[_i];
}

//TODO: remove
void print_spacing_digits(uint digits)
{
    _digits = digits;
}

void set_spacing_digits(uint digits)
{
    _digits = digits;
}

void print_spacing_ull(ull number)
{
    int digits = _digits;
    printf("%llu", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf( " ");
        digits--;
    }
}

void print_spacing_ulong(ulong number)
{
    int digits = _digits;
    printf("%lu", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf( " ");
        digits--;
    }
}

void print_spacing_uint(uint number)
{
    int digits = _digits;
    printf("%u", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf(" ");
        digits--;
    }
}

void print_spacing_int(int number)
{
    int digits = _digits;
    printf("%d", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf(" ");
        digits--;
    }
}

void print_spacing_string(char* string)
{
    int digits = _digits - strlen(string);
    
	if (digits < 0) {
		printf("...");
		string = &string[-digits + 4];
		digits = 1;
	}

	printf("%s", string);

    while(digits > 0) {
        printf(" ");
        digits--;
    }
}

void print_spacing_string_align_left(char* string, uint left_spaces)
{
    int digits = _digits - strlen(string);

    while(digits > 0) {
        printf(" ");
        digits--;
    }

    printf("%s", string);

    while(left_spaces > 0) {
        printf(" ");
        left_spaces--;
    }
}

void strtoup(char *string)
{
    while (*string) {
        *string = toupper((unsigned char) *string);
        string++;
    }
}

char* strclean(char *string, char chr)
{
    char *index = strchr(string, chr);
    if (index == NULL) return NULL;
    string[index - string] = '\0';
    return index;
}

void remove_chars(char *s, char c)
{
    int writer = 0, reader = 0;

    while (s[reader])
    {
        if (s[reader]!=c) 
            s[writer++] = s[reader];
        
        reader++;       
    }

    s[writer]=0;
}
