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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <common/string_enhanced.h>

static const char *sym = STEN_SYMBOL;
static unsigned int format[STEN_MAX_COLS];
static unsigned int columns;
static FILE *stream;

int tprintf_init(FILE *_stream, char *_format)
{
	int len = strlen(_format);
    char *tok;

	if (len >= STEN_BUFF_SIZE) {
		columns = 0;
		return -1;
	}

	strcpy(sten_hinput, _format);
	
	columns = 0;
	stream = _stream;
	tok = strtok(sten_hinput, " ");
	
	while (tok != NULL && columns < STEN_MAX_COLS) {
		format[columns++] = atoi(tok);
		tok = strtok(NULL, " ");
	}
	
	if (columns >= STEN_MAX_COLS) {
		columns = 0;	
		return -1;
	}

	return 0;
}

int tprintf_format()
{
    char *p1 = strstr(sten_hinput, sym);
    char *p2 = sten_hinput;
    char *p3 = sten_output;

    int len = strlen(sten_hinput);
    int i = 0;
    int c = 0;

	if ((len >= STEN_BUFF_SIZE) || (columns == 0)) {
		return -1;
	}

    while(p1 && i < columns)
    {
        while(p2 != p1) {
            *p3 = *p2;
            ++c;
            ++p2;
            ++p3;
        }

        while(c < format[i]) {
            *p3 = ' ';
            ++c;
            ++p3;
        }

		if (p1 == &sten_hinput[len]) {
			break;
		}

        ++i;
        p1++;
        p1++;
        p2 = p1;
        p1 = strstr(p1, sym);

        if (!p1) p1 = &sten_hinput[len];
		
        c = 0;
    }

	p3[0] = '\n';
	p3[1] = '\0';
    fprintf(stream, sten_output);

	return 0;
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
