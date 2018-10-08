/**************************************************************
*	Energy Aware Runtim_serr (EAR)
*	This program is part of the Energy Aware Runtim_serr (EAR).
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
#include <tools/coeffs_quality.h>

#define LINE "-----------------------------------------------------------------------------------------------------\n"

int EAR_VERBOSE_LEVEL = 0;
char buffer[SZ_PATH];

typedef struct control
{
	/* node */
	char name_node[SZ_NAME_SHORT];
	char path_coeffs[SZ_PATH];
	/* allocation */
	application_t *apps;
	application_t *apps_merged;
	coefficient_t *coeffs;
	projection_t  *projs;
	/* indexes */
	int n_apps;
	int n_apps_merged;
	int n_coeffs;
	/* other */
	uint frq_base;
	uint learning;
	uint summary;
	uint hide;
} control_t;
	
control_t cntr;

/*
 *
 * Projections
 *
 */

double projection_pow(double pow_serrr0, double tpi_sign, coefficient_t *coeffs)
{
	return coeffs->A * pow_serrr0 + coeffs->B * tpi_sign + coeffs->C;
}

double projection_cpi(double cpi_sign, double tpi_sign, coefficient_t *coeffs)
{
	return coeffs->D * cpi_sign + coeffs->E * tpi_sign + coeffs->F;
}

double projection_tim(double tim_serr0, double cpi_proj, double cpi_sign, ulong f0, ulong fn)
{
	return (tim_serr0 * cpi_proj / cpi_sign) * ((double) f0 / (double) fn);
}

/*
 *
 * Work bench
 *
 */

int find(application_t *apps, int n_apps, char *app_id, uint frq_base)
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

application_t *merge(control_t *control)
{
	application_t *apps, *apps_merged;
	double cpi, tpi, pow, tim, counter;
	int n_apps, equal_id, equal_f;
	int i, j, k;
	char *app_id;
	uint frq_base;

	// Allocation
	apps        = control->apps;
	n_apps      = control->n_apps;
	apps_merged = (application_t *) calloc(n_apps, sizeof(application_t));

	// Initialization
	for(i = 0; i < n_apps; ++i) {
		apps_merged[i].job.app_id[0] = '\0';
	}

	//
	for(i = 0, j = 0; i < n_apps; ++i)
	{
		frq_base = apps[i].signature.def_f;
		app_id = apps[i].job.app_id;

		if (find(apps_merged, j, app_id, frq_base) == -1)
		{
			memcpy(&apps_merged[j], &apps[i], sizeof(application_t));

			tpi = apps[i].signature.TPI;
			cpi = apps[i].signature.CPI;
			tim = apps[i].signature.tim_serr;
			pow = apps[i].signature.DC_pow_serrr;
			counter = 1.0;

			for(k = i + 1; k < n_apps; ++k)
			{
				equal_id = strcmp(app_id, apps[k].job.app_id) == 0;
				equal_f  = frq_base == apps[k].signature.def_f;

				if (equal_id && equal_f)
				{
					tpi += apps[k].signature.TPI;
					cpi += apps[k].signature.CPI;
					tim += apps[k].signature.tim_serr;
					pow += apps[k].signature.DC_pow_serrr;
					counter += 1.0;
				}
			}

			apps_merged[j].signature.DC_pow_serrr = pow / counter;
			apps_merged[j].signature.tim_serr     = tim / counter;
			apps_merged[j].signature.TPI      = tpi / counter;
			apps_merged[j].signature.CPI      = cpi / counter;

			j += 1;
		}
	}

	control->apps_merged = apps_merged;
	control->n_apps_merged = j;

	return apps_merged;
}

void evaluate(control_t *control)
{
	// App merged error
	double cpi_sign, tpi_sign, tim_sign, pow_sign;
	double cpi_proj, tpi_proj, tim_proj, pow_proj;
	double cpi_serr, tpi_serr, tim_serr, pow_serr;

	// Medium error
	double *tim_merr;
	double *pow_merr;
	double *n_merr;

	// General error
	double tim_gerr;
	double pow_gerr;
	double n_gerr;

	// Other data
	application_t *merged;
	coefficient_t *coeffs;
	int n_merged;
	int n_coeffs;
	int frq_base;
	int i, j, k;

	//
	frq_base = control->frq_base;
	coeffs   = control->coeffs;
	n_coeffs = control->n_coeffs;
	merged   = control->apps_merged;
	n_merged = control->n_apps_merged;

	// Medium Error
	tim_merr = calloc(n_coeffs, sizeof(double));
	pow_merr = calloc(n_coeffs, sizeof(double));
	n_merr   = calloc(n_coeffs, sizeof(double));

	// General medium error
	tim_gerr = 0.0;
	pow_gerr = 0.0;
	n_gerr = 0.0;

	// Initializing columns
	tprintf_init(stderr, "18 11 15 12 12 15 12 12");

	for (j = 0; j < n_merged; ++j)
	{
		if (merged[j].signature.def_f == frq_base)
		{
			for (i = 0, k = 0; i < n_coeffs; i++)
            {
                if (coeffs[i].available && coeffs[i].pstate_ref == frq_base)
                {
                    k += find(merged, n_merged, merged[j].job.app_id, coeffs[i].pstate) != -1;
				}
			}

			// No more than one application to compare
			if (k <= 1) {
				continue;
			}

			tprintf("%s||@%u|| | T. Real||T. Proj||T. Err|| | P. Real||P. Proj||P. Err",
					merged[j].job.app_id, merged[j].signature.def_f);

			cpi_sign = merged[j].signature.CPI;
			tpi_sign = merged[j].signature.TPI;
			tim_sign = merged[j].signature.tim_serr;
			pow_sign = merged[j].signature.DC_pow_serrr;

			for (i = 0; i < n_coeffs; i++)
			{
				if (coeffs[i].available && coeffs[i].pstate_ref == frq_base)
				{
					// Error
					cpi_proj = projection_cpi(cpi_sign, tpi_sign, &coeffs[i]);
					tim_proj = projection_tim(tim_sign, cpi_proj, cpi_sign, frq_base, coeffs[i].pstate);
					pow_proj = projection_pow(pow_sign, tpi_sign, &coeffs[i]);

					// Fin that application for that coefficient
					k = find(merged, n_merged, merged[j].job.app_id, coeffs[i].pstate);

					if (k != -1)
					{
						m = &merged[k];

						tim_serr = fabs((1.0 - (m->signature.tim_serr / tim_proj)) * 100.0);
						pow_serr = fabs((1.0 - (m->signature.DC_pow_serrr / pow_proj)) * 100.0);
						cpi_serr = fabs((1.0 - (m->signature.CPI / cpi_proj)) * 100.0);

						if (!cntr->hide)
						{
							tprintf("->||%lu|| | %0.2lf||%0.2lf||%0.2lf|| | %0.2lf||%0.2lf||%0.2lf",
								coeffs[i].pstate, m->signature.tim_serr, tim_proj, tim_serr,
								m->signature.DC_pow_serrr, pow_proj, pow_serr);
						}
					} else {
						if (!cntr->hide) {
							tprintf("->||%lu|| | -||-||-|| | -||-||-", coeffs[i].pstate);
						}
					}

					// Medium error
					if (coeffs[i].pstate != frq_base) {
						tim_merr[i] += tim_serr;
						pow_merr[i] += pow_serr;
						n_merr[i]   += 1.0;
					}
				}
			}
		}
	}

	if (!cntr.summary) {
		return;
	}

	// Coefficients medium error
	fprintf(stderr, LINE);
	tprintf("medium error||@%u|| | -||-||T. Err|| | -||-||P. Err", frq_base);

	for (i = 0; i < n_coeffs; i++)
	{
		if (n_merr[i] > 0.0)
		{
			// Medium error
			tim_merr[i] = tim_merr[i] / n_merr[i];
			pow_merr[i] = pow_merr[i] / n_merr[i];

			tprintf("->||%lu|| | -||-||%0.2lf|| | -||-||%0.2lf",
					coeffs[i].pstate, tim_merr[i], pow_merr[i]);

			// General medium error
			tim_gerr += tim_merr[i];
			pow_gerr += pow_merr[i];
			n_gerr   += 1.0f;
		}
	}

	// General medium error
	fprintf(stderr, LINE);
	tim_gerr = tim_gerr / n_gerr;
	pow_gerr = pow_gerr / n_gerr;

	tprintf("general error||%lu|| | -||-||%0.2lf|| | -||-||%0.2lf",
			frq_base, tim_gerr, pow_gerr);
}

/*
 *
 * Initialization
 *
 */

void read_applications(control_t *cntr)
{
	application_t *appsn;
	application_t *appsl;
	int n_appsn = 0;
	int n_appsl = 0;
	int i;

	//
	n_appsn = db_read_applications(&appsn, 1, 1000, cntr->name_node);

	if (n_appsn == 0) {
		fprintf(stderr, "No learning apps found for the node '%s'\n", cntr->name_node);	
		exit(1);
	}

	
	if (cntr->learning) {
		n_appsl = db_read_applications(&appsl, 0, 1000, cntr->name_node);
	}

	//
	cntr->n_apps = n_appsn + n_appsl;
	cntr->apps = calloc(cntr->n_apps, sizeof(application_t));

	//
	memcpy(cntr->apps, appsn, n_appsn * sizeof(application_t));

	if (cntr->learning) {
		memcpy(&cntr->apps[n_appsn], appsl, n_appsl * sizeof(application_t));
	}
	
	// Merging
	merge(cntr);
}

void read_coefficients(cluster_conf_t *conf, control_t *cntr)
{
	sprintf(cntr->path_coeffs, "%s/island0/coeffs.%s",
		conf->earlib.coefficients_pathname, cntr->name_node);

	cntr->n_coeffs = coeff_file_read(cntr->path_coeffs, &cntr->coeffs);

	if (cntr->n_coeffs == 0) {
		fprintf(stderr, "No coefficient file found for the node '%s'\n", cntr->name_node);	
		exit(1);
	}
}

void usage(int argc, char *argv[], control_t *cntr)
{
	int i = 0;

	if (argc < 3) {
		fprintf(stdout, "Usage: %s hostname frequency [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  The hostname of the node where to test the coefficients quality.\n");
		fprintf(stdout, "  The frequency is the nominal base frequency of that node.\n\n");
		fprintf(stdout, "Options:\n");
		fprintf(stdout, "\t-A, --all\tMerges the applications database in addition\n");
		fprintf(stdout, "\t\t\tof the learning applications database.\n");
		fprintf(stdout, "\t-S, --summary\tShows the medium and general errors.\n");
		fprintf(stdout, "\t-H, --hide\tHides the merged applications projections and errors.\n");
		exit(1);
	}

	// Basic parametrs
	cntr->frq_base = (unsigned long) atoi(argv[2]);
	strcpy(cntr->name_node, argv[1]);

	// Additional parameters
	for (i = 3; i < argc; ++i)
	{
		cntr->learning = ((strcmp(argv[i], "-A") == 0) ||
						  (strcmp(argv[i], "--all") == 0));

		cntr->summary = ((strcmp(argv[i], "-S") == 0) ||
						 (strcmp(argv[i], "--summary") == 0));

		cntr->summary = ((strcmp(argv[i], "-H") == 0) ||
						 (strcmp(argv[i], "--hide") == 0));
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
	cluster_conf_t conf;

	//
	usage(argc, argv, &cntr);

	//
	init(&conf);

	// Allocating applications
	read_applications(&cntr);

	// Reading coefficients
	read_coefficients(&conf, &cntr);

	// Evaluating
	evaluate(&cntr);

	return 0;
}
