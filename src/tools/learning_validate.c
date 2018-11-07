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
#include <common/sizes.h>
#include <common/string_enhanced.h>
#include <common/types/application.h>
#include <common/types/coefficient.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/database/db_helper.h>

//
int EAR_VERBOSE_LEVEL = 0;

//
char buffer[SZ_PATH];
char buffer_nodename[256];

//
static int opt_g;
static int opt_h;

//
static application_t *apps;
static int n_apps;

//
static int diff_apps;
static int diff_freq;
static int diff_060s;
static int diff_120s;
static int diff_400w; // A node oscillates bw 100-400W with no GPU
static int diff_100w; // 
static int diff_000g; // (3,400,000,000) * 2 * 8 * 8 = 435 GB/s max BW
static int diff_450g; // (DDR4@3.4GHz OC) * (dual) * (8 bytes) * (8 channels)
static double diff_exfr;
static ulong max_freq;
static ulong min_freq;

/*
 *
 *
 *
 */

static void print_header()
{
	// If not general the header is printed
	if (!opt_g)
	{
		tprintf_init(stdout, STR_MODE_COL, "15 10 10 10");

		tprintf("Application|||Time||Power||Bandwidth");
		tprintf("-----------|||----||-----||---------");

		return;
	}

	// If general
	tprintf_init(stdout, STR_MODE_COL, "10 7 8 8 9 9 9 10 10 10");

	// If header
	if (opt_h) {
		tprintf("Nodename|||#Apps||#Execs||#Times|||#Freqs||Max||Min|||W. tim.||W. pow.||W. gbs.");
		tprintf("--------|||-----||------||------|||------||---||---|||-------||-------||-------");
	}
}

static void print_individual(application_t *app)
{
    char *col_time;
    char *col_powe;
    char *col_bwth;
	int warn_time;
    int warn_powe;
	int warn_bwth;
    double time;
    double powe;
    double bwth;
    ulong freq;
    char *name;

	if (opt_g) {
		return;
	}

	//
	name = app->job.app_id;
	freq = app->job.def_f;
	time = app->signature.time;
	powe = app->signature.DC_power;
	bwth = app->signature.GBS;

	// Warnings
	warn_time = (time > 120.0) | (time < 060.0);
	warn_powe = (powe > 400.0) | (powe < 100.0);
	warn_bwth = (bwth > 450.0) | (bwth < 000.0);

	// 
    col_time = (warn_time == 0) ? "": STR_RED;
    col_powe = (warn_powe == 0) ? "": STR_RED;
    col_bwth = (warn_bwth == 0) ? "": STR_RED;
    
    if (warn_time || warn_powe || warn_bwth)	{
        tprintf("%s|||%s%0.2lf||%s%0.2lf||%s%0.2lf", name,
			col_time, time, col_powe, powe, col_bwth, bwth);
    }
}

static void print_summary()
{
	int warn_bwth;
	int warn_powe;
	int warn_time;
	char *col_bwth;
	char *col_powe;
	char *col_time;

	if (n_apps == 0 || !opt_g) {
		return;
	}

	//
	warn_bwth = diff_450g + diff_000g;
	warn_powe = diff_400w + diff_100w;
	warn_time = diff_120s + diff_060s;

	//
	col_time = (warn_time == 0) ? "": STR_RED;
	col_powe = (warn_powe == 0) ? "": STR_RED;
	col_bwth = (warn_bwth == 0) ? "": STR_RED;

	// Table
	tprintf("%s|||%d||%d||%0.2lf|||%d||%lu||%lu|||%s%d||%s%d||%s%d",
			buffer_nodename, diff_apps, n_apps, diff_exfr,
			diff_freq, max_freq, min_freq,
			col_time, warn_time, col_powe, warn_powe, col_bwth, warn_bwth
	);
}

static void analyze()
{
	char *name;
	ulong freq;
	double time;
	double powe;
	double bwth;
	int found_name;
	int found_freq;
	int a, i;

	if (n_apps == 0) {
		return;
	}

	//
	max_freq = apps[0].job.def_f;
	min_freq = apps[0].job.def_f;

	// Finding different apps
	for (a = 0; a < n_apps; a++)
	{
		name = apps[a].job.app_id;
		freq = apps[a].job.def_f;
		time = apps[a].signature.time;
		powe = apps[a].signature.DC_power;
		bwth = apps[a].signature.GBS;

		found_name = 0;
		found_freq = 0;

		for (i = 0; i < a; i++) {
			found_name = found_name | (strcmp(name, apps[i].job.app_id) == 0);
			found_freq = found_freq | (freq == apps[i].job.def_f);
		}

		diff_apps += !found_name;
		diff_freq += !found_freq;
		diff_060s += (time < 060.0);
		diff_120s += (time > 120.0);
		diff_100w += (powe < 100.0);
		diff_400w += (powe > 400.0);
		diff_450g += (bwth > 450.0);
		diff_000g += (bwth < 000.0);
		min_freq   = min_freq * (freq >= min_freq) + freq * (freq < min_freq);
		max_freq   = max_freq * (freq <= max_freq) + freq * (freq > max_freq);
		
		print_individual(&apps[a]);
	}

	diff_exfr = (double) n_apps / (double) diff_freq;
}

/*
 *
 *
 *
 */

static void read_applications()
{
	static cluster_conf_t conf;

	get_ear_conf_path(buffer);

	if (read_cluster_conf(buffer, &conf) != EAR_SUCCESS){
		fprintf(stderr, "Error reading cluster configuration.\n");
		exit(1);
	}

	init_db_helper(&conf.database);

	application_t *apps_aux;

	//
	n_apps = db_read_applications(&apps_aux, 1, 1000, buffer_nodename);

	if (n_apps == 0) {
		return;
	}

	//
	apps = calloc(n_apps, sizeof(application_t));
	memcpy(apps, apps_aux, n_apps * sizeof(application_t));

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

	if (argc < 2)
	{
		fprintf(stdout, "Usage: %s node.name [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  node.name is the name of the node to analyze\n");
		fprintf(stdout, "\nOptions:\n");
		fprintf(stdout, "\t-G \tShows one lined general summary \n");
		fprintf(stdout, "\t-H \tShows the header in one lined summary mode.\n");

		exit(1);
	}

	// Basic parametrs
	strcpy(buffer_nodename, argv[1]);

	// Flags
	while ((c = getopt (argc, argv, "GH")) != -1)
	{
		switch (c)
		{
			case 'G':
				opt_g = 1;
				break;
			case 'H':
                opt_h = 1;
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
	usage(argc, argv);

	read_applications();

	print_header();

	analyze();

	print_summary();

    return 0;
}
