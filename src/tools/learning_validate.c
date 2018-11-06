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
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <common/types/sizes.h>
#include <common/types/coefficient.h>

//
char buffer[SZ_PATH];
char buffer_nodename[256];

//
static int opt_g;

//
static application_t *apps;
static int n_apps;

//
static int diff_apps;
static int diff_freq;
static int diff_060s;
static int diff_120s;
static double diff_exfr;
static ulong max_freq;
static ulong min_freq;

/*
 *
 *
 *
 */

static void analyze()
{
	char *name;
	ulong freq;
	int found_name;
	int found_freq;
	int a, i;

	//
	max_freq = apps[0].job.def_f;
	min_freq = apps[0].job.def_f;

	// Finding different apps
	for (a = 0; a < n_apps; a++)
	{
		name = apps[a].job.app_id;
		freq = apps[a].job.def_f;

		found_name = 0;
		found_freq = 0;

		for (i = 0; i < a; i++) {
			found_name = found_name | (strcmp(name, apps[i].job.app_id) == 0);
			found_freq = found_freq | (freq == apps[i].job.def_f);
		}

		diff_apps += !found_name;
		diff_freq += !found_freq;
		diff_060s += (apps[a].signature.time < 060.0);
		diff_120s += (apps[a].signature.time > 120.0);
		min_freq   = min_freq * (freq > min_freq) + freq * (freq < min_freq);
		max_freq   = max_freq * (freq < max_freq) + freq * (freq > max_freq);
	}

	diff_exfr = (double) n_apps / (double) diff_freq;
}

static void print()
{
	tprintf_init(stdout, STR_MODE_DEF, "18 11 15 12 12 15 12 12");

	tprintf("#Freqs.||Max. freq.||Min. freq.|| | #Apps||#Execs.||#Execs. freq.||# > 120s||# < 60s");
	tprintf("%d||%lu||%lu|| | %d||%d||%0.2lf||%u||%d",
		diff_freq, max_freq, min_freq, diff_apps, n_apps, diff_exfr, diff_120s, diff_120s);
}

/*
 *
 *
 *
 */

static void read_applications()
{
	application_t *apps_aux;

	//
	n_appsn = db_read_applications(&apps_aux, 1, 1000, buffer_nodename);

	//
	apps = calloc(n_apps, sizeof(application_t));
	memcpy(apps, apps_aux, n_appsn * sizeof(application_t));

	//
	free(apps_aux);
}

/*
 *
 *
 *
 */

static void usage(int argc, char *argv[])
{
	int i, c;

	if (argc < 3)
	{
		fprintf(stdout, "Usage: %s node.name [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  node.name is the name of the node to analyze\n");
		fprintf(stdout, "\nOptions:\n");
		fprintf(stdout, "\t-G \tSaves the default coefficients file in a\n");

		exit(1);
	}

	// Basic parametrs
	strcpy(buffer_nodename, argv[1]);

	// Flags
	while ((c = getopt (argc, argv, "G")) != -1)
	{
		switch (c)
		{
			case 'G':
				opt_a = 1;
				break;
			case '?':
				break;
			default:
				abort();
		}
	}
}

int main(int argc, char *argv[])
{
	read_applications();

	print();

    return 0;
}
