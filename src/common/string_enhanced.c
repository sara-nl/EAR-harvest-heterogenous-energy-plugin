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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <common/colors.h>
#include <common/string_enhanced.h>

static const char *sym = STR_SYMBOL;
static unsigned int format[STR_MAX_COLUMNS];
static unsigned int columns;
static unsigned int mode;
static FILE *stream;

/*
 *
 *
 *
 */

static int tprintf_color_open(char **iput, char **oput)
{
	char *ibuf = *iput;
	char *obuf = *oput;

	if (strncmp(ibuf, STR_RED, STR_COL_CHR) == 0)
	{
		sprintf(obuf, "%s", COL_RED);
		*iput = &ibuf[STR_COL_CHR];
		*oput = &obuf[COL_CHR];
		return 1;
	}

	if (strncmp(ibuf, STR_YLW, STR_COL_CHR) == 0)
	{
		sprintf(obuf, "%s", COL_YLW);
		*iput = &ibuf[STR_COL_CHR];
		*oput = &obuf[COL_CHR];
		return 1;
	}


	return 0;
}

static void tprintf_color_close(char **oput)
{
	char *obuf = *oput;

	sprintf(obuf, "%s", COL_CLR);
	*oput = &obuf[CLR_CHR];
}

/*
 *
	 *
 *
 */

int tprintf_init(FILE *_stream, int _mode, char *_format)
{
	int len = strlen(_format);
    char *tok;

	if (len >= STR_SIZE_BUFFER) {
		columns = 0;
		return -1;
	}

	//
	columns = 0;
	stream  = _stream;
	mode    = _mode;


	// Getting the format
	strcpy(tprintf_ibuf, _format);

	//
	tok = strtok(tprintf_ibuf, " ");

	//
	while (tok != NULL && columns < STR_MAX_COLUMNS) {
		format[columns++] = atoi(tok);
		tok = strtok(NULL, " ");
	}

	if (columns >= STR_MAX_COLUMNS) {
		columns = 0;
		return -1;
	}

	return 0;
}

int tprintf_format()
{
    char *p1 = strstr(tprintf_ibuf, sym);
    char *p2 = tprintf_ibuf;
    char *p3 = tprintf_obuf;

    int len = strlen(tprintf_ibuf);
	int chr = 0;
	int col = 0;
    int i = 0;

	if ((len >= STR_SIZE_BUFFER) || (columns == 0)) {
		return -1;
	}

    while(p1 && i < columns)
    {
    	// If color
		if (mode == STR_MODE_TAB_COL) {
			col = tprintf_color_open(&p2, &p3);
		}

    	// Character copy and count
        while(p2 != p1) {
            *p3 = *p2;
            ++chr;
            ++p2;
            ++p3;
		}

        // Number of characters per column
		if (mode == STR_MODE_CSV_DEF) {
			*p3 = ';';
			++p3;
		}

        while(mode <= STR_MODE_TAB_COL && chr < format[i]) {
            *p3 = ' ';
            ++chr;
            ++p3;
        }

		if (col) {
			tprintf_color_close(&p3);
			col = 0;
		}

		if (p1 == &tprintf_ibuf[len]) {
			break;
		}

        ++i;
        p1++;
        p1++;
        p2 = p1;
        p1 = strstr(p1, sym);

        if (!p1) p1 = &tprintf_ibuf[len];
		
        chr = 0;
    }

	p3[0] = '\n';
	p3[1] = '\0';
    fprintf(stream, "%s", tprintf_obuf);

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
