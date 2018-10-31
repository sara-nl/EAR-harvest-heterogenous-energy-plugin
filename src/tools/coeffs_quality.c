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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/sizes.h>
#include <common/states.h>
#include <common/types/projection.h>
#include <common/types/application.h>
#include <common/types/coefficient.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/database/db_helper.h>

#define LINE "-----------------------------------------------------------------------------------------------------\n"

int EAR_VERBOSE_LEVEL = 0;

// buffers
static char name_node[SZ_NAME_SHORT];
static char path_coeffs[SZ_PATH];
static char path_input[SZ_PATH];
static char buffer[SZ_PATH];

/* node */
static cluster_conf_t conf;

/* allocation */
static coefficient_t *coeffs;
static application_t *apps;
static application_t *mrgd;
static double *prjs;
static double *errs;
static double *errs_gen;
static double *errs_med;

/* indexes */
static int n_coeffs;
static int n_apps;
static int n_mrgd;
static int n_prjs;
static int n_errs;

// General metrics
static uint frq_base;

/* other */
static uint opt_a;
static uint opt_s;
static uint opt_g;
static uint opt_d;
static uint opt_i;
static uint opt_h;
static uint opt_c;

/*
 *
 * Work bench
 *
 */

static int find(application_t *apps, int n_apps, char *app_id, uint frq_base)
{
	int equal_id = 0;
	int equal_fq = 0;
	int i;

	for(i = 0; i < n_apps; ++i)
	{
		equal_id = strcmp(app_id, apps[i].job.app_id) == 0;
		equal_fq = frq_base == apps[i].signature.def_f;

		if (equal_id && equal_fq) {
			return i;
		}
	}

	return -1;
}

static application_t *merge()
{
	char *app_id;
	double cpi, tpi, pow, tim, counter;
	int equal_id, equal_f;
	int i, j, k;
	uint freq;

	// n_apps * n_coeffs * (cpi + time + power)
	n_prjs = n_apps * n_coeffs * 3;
	n_errs = n_apps * n_coeffs * 3;

	// Allocation
	mrgd = (application_t *) calloc(n_apps, sizeof(application_t));
	prjs = (double *) calloc(n_prjs, sizeof(double));
	errs = (double *) calloc(n_errs, sizeof(double));
	errs_med = (double *) calloc(n_coeffs * 4, sizeof(double));
	errs_gen = (double *) calloc(n_coeffs * 4, sizeof(double));

	// Initialization
	for(i = 0; i < n_apps; ++i) {
		mrgd[i].job.app_id[0] = '\0';
	}

	//
	for(i = 0, j = 0; i < n_apps; ++i)
	{
		freq = apps[i].signature.def_f;
		app_id = apps[i].job.app_id;

		if (find(mrgd, j, app_id, freq) == -1)
		{
			memcpy(&mrgd[j], &apps[i], sizeof(application_t));

			tpi = apps[i].signature.TPI;
			cpi = apps[i].signature.CPI;
			tim = apps[i].signature.time;
			pow = apps[i].signature.DC_power;
			counter = 1.0;

			for(k = i + 1; k < n_apps; ++k)
			{
				equal_id = strcmp(app_id, apps[k].job.app_id) == 0;
				equal_f  = freq == apps[k].signature.def_f;

				if (equal_id && equal_f)
				{
					tpi += apps[k].signature.TPI;
					cpi += apps[k].signature.CPI;
					tim += apps[k].signature.time;
					pow += apps[k].signature.DC_power;
					counter += 1.0;
				}
			}

			mrgd[j].signature.DC_power = pow / counter;
			mrgd[j].signature.time     = tim / counter;
			mrgd[j].signature.TPI      = tpi / counter;
			mrgd[j].signature.CPI      = cpi / counter;

			j += 1;
		}
	}

	n_mrgd = j;

	return mrgd;
}

static void compute()
{
	// App mrgd error
	double cpi_sign, tpi_sign, tim_sign, pow_sign;

	// Other data
	int c, a, k;

	// Initializing columns
	for (a = 0; a < n_mrgd; ++a)
	{
		if (mrgd[a].signature.def_f == frq_base)
		{
			for (c = 0, k = 0; c < n_coeffs; c++)
            {
                if (coeffs[c].available && coeffs[c].pstate_ref == frq_base)
                {
                    k += find(mrgd, n_mrgd, mrgd[a].job.app_id, coeffs[c].pstate) != -1;
				}
			}

			// No more than one application to compare
			if (k <= 1) {
				continue;
			}

			cpi_sign = mrgd[a].signature.CPI;
			tpi_sign = mrgd[a].signature.TPI;
			tim_sign = mrgd[a].signature.time;
			pow_sign = mrgd[a].signature.DC_power;

			for (c = 0; c < n_coeffs; c++)
			{
				if (coeffs[c].available && coeffs[c].pstate_ref == frq_base)
				{
					// Error
					prjs[a][c+0] = coeff_project_cpi(cpi_sign, tpi_sign, &coeffs[c]);
					prjs[a][c+1] = coeff_project_tim(tim_sign, cpi_proj, cpi_sign, frq_base, coeffs[c].pstate);
					prjs[a][c+2] = coeff_project_pow(pow_sign, tpi_sign, &coeffs[c]);

					// Fin that application for that coefficient
					k = find(mrgd, n_mrgd, mrgd[a].job.app_id, coeffs[c].pstate);

					if (k != -1)
					{
						application_t *m = &mrgd[k];

						errs[a][c+0] = fabs((1.0 - (m->signature.CPI / cpi_proj)) * 100.0);
						errs[a][c+1] = fabs((1.0 - (m->signature.time / tim_proj)) * 100.0);
						errs[a][c+2] = fabs((1.0 - (m->signature.DC_power / pow_proj)) * 100.0);
					}

					// Medium error
					if (coeffs[c].pstate != frq_base)
					{
						errs_med[c+0] += errs[a][c+0];
						errs_med[c+1] += errs[a][c+1];
						errs_med[c+2] += errs[a][c+2];
						errs_med[c+3] += 1.0;
					}
				}
			}
		}
	}

	// Coefficients medium error
	for (c = 0; c < n_coeffs; c++)
	{
		if (n_merr[c] > 0.0)
		{
			// Medium error
			errs_med[c+0] = errs_med[c+0] / errs_med[c+3];
			errs_med[c+1] = errs_med[c+1] / errs_med[c+3];
			errs_med[c+2] = errs_med[c+2] / errs_med[c+3];

			// General error
			errs_gen[c+0] += errs_med[c+0];
			errs_gen[c+1] += errs_med[c+1];
			errs_gen[c+2] += errs_med[c+2];
			errs_gen[c+3] += 1.0;
		}
	}

	// General medium error
	errs_gen[c+0] = errs_med[c+0] / errs_gen[c+3];
	errs_gen[c+1] = errs_med[c+1] / errs_gen[c+3];
	errs_gen[c+2] = errs_med[c+2] / errs_gen[c+3];
}

void print()
{
	int c, a, k;

	tprintf_init(stderr, STR_MODE_COL, "18 11 15 12 12 15 12 12");

	for (a = 0; a < n_mrgd; ++a)
	{
		if (mrgd[a].signature.def_f == frq_base)
		{
			for (c = 0, k = 0; c < n_coeffs; c++)
			{
				if (coeffs[c].available && coeffs[c].pstate_ref == frq_base)
				{
					k += find(mrgd, n_mrgd, mrgd[a].job.app_id, coeffs[c].pstate) != -1;
				}
			}

			// No more than one application to compare
			if (k <= 1) {
				continue;
			}

			tprintf("%s||@%u|| | T. Real||T. Proj||T. Err|| | P. Real||P. Proj||P. Err",
					merged[j].job.app_id, merged[j].signature.def_f);

			for (c = 0; c < n_coeffs; c++)
			{
				if (coeffs[c].available && coeffs[c].pstate_ref == frq_base)
				{
					tprintf("->||%lu|| | %0.2lf||%0.2lf||%0.2lf|| | %0.2lf||%0.2lf||%0.2lf",
							coeffs[i].pstate, m->signature.time, tim_proj, tim_serr,
							m->signature.DC_power, pow_proj, pow_serr);
				}
			}
		}
	}
}

/*
 *
 * Read
 *
 */

void read_applications()
{
	application_t *appsn;
	application_t *appsl;
	int n_appsn = 0;
	int n_appsl = 0;
	int i;

	//
	n_appsn = db_read_applications(&appsn, 1, 1000, name_node);

	if (n_appsn <= 0)
	{
		if (!opt_g) {
			fprintf(stderr, "No learning apps found for the node '%s'\n", name_node);
		}

		exit(1);
	}

	
	if (opt_a) {
		n_appsl = db_read_applications(&appsl, 0, 1000, name_node);
	}

	//
	n_apps = n_appsn + n_appsl;
	apps = calloc(n_apps, sizeof(application_t));

	//
	memcpy(apps, appsn, n_appsn * sizeof(application_t));

	if (opt_a) {
		memcpy(&apps[n_appsn], appsl, n_appsl * sizeof(application_t));
	}
}

void read_coefficients(cluster_conf_t *conf)
{
	char *node = name_node;
	int island;

	//
	island = get_node_island(conf, node);

	if (island == EAR_ERROR) {
		fprintf(stderr, "no island found for node %s, exiting\n", node);
		exit(1);
	}

	// If file is custom
	if (opt_i)
	{
		n_coeffs = coeff_file_read(path_input, &coeffs);
	}
	// If default is selected
	else if (!opt_d)
	{
		sprintf(path_coeffs, "%s/island%d/coeffs.%s",
				conf->earlib.coefficients_pathname, island, node);
		//
		n_coeffs = coeff_file_read(path_coeffs, &coeffs);
	}

	// Don't worked, looking for defaults
	if (n_coeffs <= 0)
	{
		sprintf(buffer, "%s/island%d/coeffs.default",
 			conf->earlib.coefficients_pathname, island);
		//
		n_coeffs = coeff_file_read(buffer, &coeffs);
		
		if (n_coeffs <= 0)
		{
			fprintf(stderr, "no default coefficients found, exiting\n");	
			exit(1);
		}
	}
}

/*
 *
 * Initialization
 *
 */

void usage(int argc, char *argv[])
{
	int i = 0;

	if (argc < 3)
	{
		fprintf(stdout, "Usage: %s hostname frequency [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  The hostname of the node where to test the coefficients quality.\n");
		fprintf(stdout, "  The frequency is the nominal base frequency of that node.\n\n");
		fprintf(stdout, "Options:\n");
		fprintf(stdout, "\t-A\tAdds also the applications database.\n");
		fprintf(stdout, "\t-C\tPrints the console output in CSV format.\n");
		fprintf(stdout, "\t-D\tUses the default coefficients.\n");
		fprintf(stdout, "\t-G\tShows only the opt_g medium error.\n");
		fprintf(stdout, "\t-H\tHides the applications projections and errors.\n");
		fprintf(stdout, "\t-I <path>\tUse a custom coefficients opt_i file.\n");
		fprintf(stdout, "\t-S\tShows the medium and opt_g errors.\n");
		exit(1);
	}

	while ((c = getopt (argc, argv, "ACDGHI:S")) != -1)
	{
		switch (c)
		{
			case 'A':
				opt_a = 1;
				break;
			case 'C':
				opt_c = 1;
				break;
			case 'D':
				opt_d = 1;
				break;
			case 'G':
				opt_g = 1;
				break;
			case 'H':
				opt_h = 1;
				break;
			case 'I':
				opt_i = 1;
				strcpy(buffer_input, optarg);
				break;
			case 'S':
				opt_s = 1;
				break;
			case '?':
				return 1;
			default:
				abort ();
		}
	}
}

void init(cluster_conf_t *conf)
{
	// Initialization
	get_ear_conf_path(buffer);

	if (read_cluster_conf(buffer, conf) != EAR_SUCCESS){
		fprintf(stderr, "Error reading cluster configuration.\n");
		exit(1);
	}

	init_db_helper(&conf->database);
}

/*
 *
 * Main
 *
 */

int main(int argc, char *argv[])
{
	// Initialization
	usage(argc, argv);

	init(&conf);

	// Read
	read_coefficients();

	read_applications();

	// Work bench
	merge();

	compute();

	print();

	return 0;
}
