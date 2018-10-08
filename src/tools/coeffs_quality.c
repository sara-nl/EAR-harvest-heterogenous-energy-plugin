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
#include <tools/coeffs_quality.h>

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
	uint learning;
	uint f0_mhz;
} control_t;
	
control_t cntr;

/*
 *
 * Projections
 *
 */

double power_proj(double power0, double tpi0, coefficient_t *coeffs)
{
	return coeffs->A * power0 + coeffs->B * tpi0 + coeffs->C;
}

double cpi_proj(double cpi0, double tpi0, coefficient_t *coeffs)
{
	return coeffs->D * cpi0 + coeffs->E * tpi0 + coeffs->F;
}

double time_proj(double time0, double cpip, double cpi0, ulong f0, ulong fn)
{
	return (time0 * cpip / cpi0) * ((double) f0 / (double) fn);
}

/*
 *
 * Work bench
 *
 */

int find(application_t *apps, int n_apps, char *app_id, uint f0_mhz)
{
	int equal_id = 0;
	int equal_fq = 0;
	int i;

	for(i = 0; i < n_apps; ++i)
	{
		equal_id = strcmp(app_id, apps[i].job.app_id) == 0;
		equal_fq = f0_mhz == apps[i].signature.def_f;

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
	uint f0_mhz;

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
		f0_mhz = apps[i].signature.def_f;
		app_id = apps[i].job.app_id;

		if (find(apps_merged, j, app_id, f0_mhz) == -1)
		{
			memcpy(&apps_merged[j], &apps[i], sizeof(application_t));

			tpi = apps[i].signature.TPI;
			cpi = apps[i].signature.CPI;
			tim = apps[i].signature.time;
			pow = apps[i].signature.DC_power;
			counter = 1.0;

			for(k = i + 1; k < n_apps; ++k)
			{
				equal_id = strcmp(app_id, apps[k].job.app_id) == 0;
				equal_f  = f0_mhz == apps[k].signature.def_f;

				if (equal_id && equal_f)
				{
					tpi += apps[k].signature.TPI;
					cpi += apps[k].signature.CPI;
					tim += apps[k].signature.time;
					pow += apps[k].signature.DC_power;
					counter += 1.0;
				}
			}

			apps_merged[j].signature.DC_power = pow / counter;
			apps_merged[j].signature.time     = tim / counter;
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
	// Error
	double cpi0, tpi0, tim0, pow0;
	double cpip, tpip, timp, powp;
	double cpie, tpie, time, powe;

	// Medium error
	double *tim_merr;
	double *pow_merr;
	double *tim_n;
	double *pow_n;

	// Other data
	application_t *apps, *m;
	coefficient_t *coeffs;
	int n_apps, n_coeffs;
	int i, j, k;
	uint f0_mhz;

	//
	f0_mhz   = control->f0_mhz;
	coeffs   = control->coeffs;
	apps     = control->apps_merged;
	n_coeffs = control->n_coeffs;
	n_apps   = control->n_apps_merged;

	// Medium Error
	tim_merr = calloc(n_coeffs + 1, sizeof(double));
	pow_merr = calloc(n_coeffs + 1, sizeof(double));

	// Initializing columns
	tprintf_init(stderr, "18 11 15 12 12 15 12 12");

	for (j = 0; j < n_apps; ++j)
	{
		if (apps[j].signature.def_f == f0_mhz)
		{
			for (i = 0, k = 0; i < n_coeffs; i++)
            {
                if (coeffs[i].available && coeffs[i].pstate_ref == f0_mhz)
                {
                    k += find(apps, n_apps, apps[j].job.app_id, coeffs[i].pstate) != -1;
				}
			}

			// No more than one application to compare
			if (k <= 1) {
				continue;
			}

			tprintf("%s||@%u|| | T. Real||T. Proj||T. Err|| | P. Real||P. Proj||P. Err",
					apps[j].job.app_id, apps[j].signature.def_f);

			cpi0 = apps[j].signature.CPI;
			tpi0 = apps[j].signature.TPI;
			tim0 = apps[j].signature.time;
			pow0 = apps[j].signature.DC_power;

			for (i = 0; i < n_coeffs; i++)
			{
				if (coeffs[i].available && coeffs[i].pstate_ref == f0_mhz)
				{
					// Error
					cpip = cpi_proj(cpi0, tpi0, &coeffs[i]);
					timp = time_proj(tim0, cpip, cpi0, f0_mhz, coeffs[i].pstate);
					powp = power_proj(pow0, tpi0, &coeffs[i]);

					//
					k = find(apps, n_apps, apps[j].job.app_id, coeffs[i].pstate);

					if (k != -1)
					{
						m = &apps[k];

						time = fabs((1.0 - (m->signature.time / timp)) * 100.0);
						powe = fabs((1.0 - (m->signature.DC_power / powp)) * 100.0);
						cpie = fabs((1.0 - (m->signature.CPI / cpip)) * 100.0);

						tprintf("->||%lu|| | %0.2lf||%0.2lf||%0.2lf|| | %0.2lf||%0.2lf||%0.2lf",
								coeffs[i].pstate, m->signature.time, timp, time,
								m->signature.DC_power, powp, powe);
					} else {
						tprintf("->||%lu|| | -||-||-|| | -||-||-", coeffs[i].pstate);
					}

					// Medium error
					tim_merr[i] += time;
					pow_merr[i] += powe;
					tim_n[i] += 1.0;
					pow_n[i] += 1.0;
				}
			}
		}
	}

	// Coefficients medium error
	tprintf("medium error||@%u|| | -||-||T. Err|| | -||-||P. Err",
			 apps[j].signature.def_f);

	for (i = 0; i < n_coeffs; i++)
	{
		if (coeffs[i].available && coeffs[i].pstate_ref == f0_mhz)
		{
			// Medium error
			tim_merr[i] = tim_merr[i] / tim_n[i];
			pow_merr[i] = pow_merr[i] / pow_n[i];

			tprintf("->||%lu|| | -||-||%0.2lf|| | -||-||%0.2lf",
					coeffs[i].pstate, tim_merr[i], pow_merr[i]);

			// General medium error
			tim_merr[n_coeffs] += tim_merr[i];
			pow_merr[n_coeffs] += pow_merr[i];
			tim_n[n_coeffs] += 1.0;
			pow_n[n_coeffs] += 1.0;
		}
	}

	// General medium error
	tim_merr[n_coeffs] = tim_merr[n_coeffs] / tim_n[n_coeffs];
	pow_merr[n_coeffs] = pow_merr[n_coeffs] / pow_n[n_coeffs];

	tprintf("general error||%lu|| | -||-||%0.2lf|| | -||-||%0.2lf",
			coeffs[i].pstate, tim_merr[n_coeffs], pow_merr[n_coeffs]);
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
	if (argc > 4 || argc < 3) {
		fprintf(stdout, "Usage: %s hostname frequency [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  The hostname of the node where to test the coefficients quality.\n");
		fprintf(stdout, "  The frequency is the nominal base frequency of that node.\n\n");
		fprintf(stdout, "Options:\n");
		fprintf(stdout, "\t-A, --all\tMerges the applications database in addition\n");
		fprintf(stdout, "\t\t\tof the learning applications database.\n");
		
		exit(1);
	}

	cntr->f0_mhz = (unsigned long) atoi(argv[2]);
	strcpy(cntr->name_node, argv[1]);

	cntr->learning = (argc == 4) && ((strcmp(argv[3], "-A") == 0) || (strcmp(argv[3], "--all") == 0));
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
