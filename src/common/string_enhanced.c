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

static char hinput[STEN_BUFF_SIZE];
static char output[STEN_BUFF_SIZE];
static const char *sym = STEN_SYMBOL;
static unsigned int table_format[STEN_MAX_COLS];
static unsigned int columns;
static FILE *stream;

void tprintf_init(FILE *_stream, unsigned int *_table_format, unsigned int _columns)
{
    stream = _stream;
    columns = _columns;
    memcpy(table_format, _table_format, columns * sizeof(unsigned int));
}

void tprintf_format()
{
    char *p1 = strstr(hinput, sym);
    char *p2 = hinput;
    char *p3 = output;

    int l = strlen(hinput);
    int i = 0;
    int c = 0;

    while(p1 && i < columns)
    {
        while(p2 != p1) {
            *p3 = *p2;
            ++c;
            ++p2;
            ++p3;
        }

        while(c < table_format[i]) {
            *p3 = ' ';
            ++c;
            ++p3;
        }

        ++i;
        p1++;
        p1++;
        p2 = p1;
        p1 = strstr(p1, sym);

        if (!p1) p1 = &hinput[l];
        c = 0;
    }

    fprintf(stream, "%s\n", output);
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
