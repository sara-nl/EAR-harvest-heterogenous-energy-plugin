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
#include <tools/coeffs_jobproj.h>

#define LINE "-----------------------------------------------------------------------------------------------------\n"

int EAR_VERBOSE_LEVEL = 0;
char buffer[SZ_PATH];

typedef struct control
{
	/* allocation */
	application_t *apps;
	application_t *mrgd;
	coefficient_t *cofs;
	projection_t  *prjs;
	/* indexes */
	int n_apps;
	int n_mrgd;
	int n_cofs;
	/* other */
	job_id job_id;
} control_t;
	
control_t cntr;

/*
 *
 * Projections
 *
 */

double projection_pow(double pow_sign, double tpi_sign, coefficient_t *cofs)
{
	return cofs->A * pow_sign + cofs->B * tpi_sign + cofs->C;
}

double projection_cpi(double cpi_sign, double tpi_sign, coefficient_t *cofs)
{
	return cofs->D * cpi_sign + cofs->E * tpi_sign + cofs->F;
}

double projection_tim(double tim_sign, double cpi_proj, double cpi_sign, ulong f0, ulong fn)
{
	return (tim_sign * cpi_proj / cpi_sign) * ((double) f0 / (double) fn);
}

/*
 *
 * Work bench
 *
 */

int find(application_t *apps, int n_apps, char *app_id, char *node_id, uint frq_base)
{
	int equal_id = 0;
	int equal_no = 0;
	int equal_fq = 0;
	int i;

	for(i = 0; i < n_apps; ++i)
	{
		equal_id = strcmp( app_id, apps[i].job.app_id) == 0;
		equal_no = strcmp(node_id,    apps[i].node_id) == 0;
		equal_fq = frq_base == apps[i].signature.def_f;

		if (equal_id && equal_no && equal_fq) {
			return i;
		}
	}

	return -1;
}

application_t *merge(control_t *control)
{
	application_t *apps, *mrgd;
	double cpi, tpi, pow, tim;
	double counter;

	uint  app_fq;
	char *app_id;
	char *nod_id;
	int n_apps;

	int equal_id;
	int equal_fq;
	int equal_no;
	int i, j, k;

	// Allocation
	apps   = control->apps;
	n_apps = control->n_apps;
	mrgd   = (application_t *) calloc(n_apps, sizeof(application_t));

	// Initialization
	for(i = 0; i < n_apps; ++i) {
		mrgd[i].job.app_id[0] = '\0';
	}

	//
	for(i = 0, j = 0; i < n_apps; ++i)
	{
		app_fq = apps[i].signature.def_f;
		app_id = apps[i].job.app_id;
		nod_id = apps[i].node_id;

		if (find(mrgd, j, app_id, app_fq) == -1)
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
				equal_no = strcmp(node, apps[k].job.app_id) == 0;
				equal_fq = app_fq == apps[k].signature.def_f;

				if (equal_id && equal_no && equal_fq)
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

	control->mrgd = mrgd;
	control->n_mrgd = j;

	for(i = 0; i < j; ++i) {
		fprintf(stderr, "%s %s\n", mrgd[i].node_id, mrgd[i]);
	}

	return mrgd;
}

/*
 *
 * Initialization
 *
 */

void read_applications(control_t *cntr)
{
	application_t *apps;
	int n_apps = 0;
	int i;

	sprintf(buffer, "select * from Applications where job_id = %lu", cntr->job_id);

	//
	n_apps = db_read_applications_query(&apps, buffer);

	if (n_apps == 0) {
		fprintf(stderr, "No apps found for the job_id '%lu'\n", cntr->job_id);
		exit(1);
	}

	
	if (cntr->learning) {
		n_appsl = db_read_applications(&appsl, 0, 1000, cntr->name_node);
	}

	//
	cntr->apps = calloc(cntr->n_apps, sizeof(application_t));

	//
	memcpy(cntr->apps, appsn, n_appsn * sizeof(application_t));

	// Merging
	merge(cntr);
}

void read_coefficients(cluster_conf_t *conf, control_t *cntr)
{
	sprintf(cntr->path_cofs, "%s/island0/cofs.%s",
		conf->earlib.coefficients_pathname, cntr->name_node);

	cntr->n_cofs = coeff_file_read(cntr->path_cofs, &cntr->cofs);

	if (cntr->n_cofs == 0) {
		fprintf(stderr, "No coefficient file found for the node '%s'\n", cntr->name_node);	
		exit(1);
	}
}

void usage(int argc, char *argv[], control_t *cntr)
{
	int i = 0;

	if (argc < 2) {
		fprintf(stdout, "Usage: %s job_id [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  The job_id of the job to project.\n");
		exit(1);
	}

	// Basic parametrs
	cntr->job_id = (unsigned long) atoi(argv[2]);
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

	return 0;
}
