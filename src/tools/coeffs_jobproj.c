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

char *cofs_str[2] = { "ok", "def" }; 
char buffer[SZ_PATH];

int EAR_VERBOSE_LEVEL = 0;
int COFS_MAX = 5;

typedef struct control
{
	/* allocation */
	application_t *apps;
	application_t *mrgd;
	coefficient_t **cofs;
	projection_t *prjs;
	/* indexes */
	int n_apps;
	int n_mrgd;
	int *n_cofs;
	int *cofs_t;
	/* format */
	ulong freq_ref;
	ulong *cofs_ref;
	/* other */
	job_id step_id;
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
		
		if (find(mrgd, j, app_id, nod_id, app_fq) == -1)
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
				equal_no = strcmp(nod_id,    apps[k].node_id) == 0;
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

	return mrgd;
}

void evaluate(control_t *cntr)
{
	double cpi_sign, tpi_sign, tim_sign, pow_sign;
	double cpi_proj[COFS_MAX];
	double tpi_proj[COFS_MAX];
	double tim_proj[COFS_MAX];
	double pow_proj[COFS_MAX];
	
	application_t *m;
	coefficient_t *c;
	int i, j, k, n, r;

	tprintf_init(stderr, "10 5 8 12 10 10 10 10 10 12 10 10 10 10 10");

	tprintf("%s||Coe.||@%u|| | T. Real||T. 1||T. 2||T. 3||T. 4||T. 5|| | P. Real||P. 1||P. 2||P. 3||P. 4||P. 5",
				cntr->mrgd[0].job.app_id, cntr->mrgd[0].signature.def_f);

	for(i = 0; i < cntr->n_mrgd; ++i)
	{
		m = &cntr->mrgd[i];
		c = cntr->cofs[i];
		n = cntr->n_cofs[i];

		cpi_sign = m->signature.CPI;
		tpi_sign = m->signature.TPI;
		tim_sign = m->signature.time;
		pow_sign = m->signature.DC_power;

		// Coefficients format
		for (j = 0, r = 0; j < n; j++)
		{
			if (c[j].available && c[j].pstate_ref == cntr->freq_ref)
			{
				for (k = 0; k < COFS_MAX; ++k)
				{
					if (c[j].pstate == cntr->cofs_ref[k])
					{
						//fprintf(stderr, "r %d ref %lu pstate %lu\n", r, c[j].pstate_ref, c[j].pstate);
						cpi_proj[r] = projection_cpi(cpi_sign, tpi_sign, &c[j]);
						tim_proj[r] = projection_tim(tim_sign, cpi_proj[r], cpi_sign, cntr->freq_ref, c[j].pstate);
						pow_proj[r] = projection_pow(pow_sign, tpi_sign, &c[j]);
						r += 1;
					}
				}
			}
		}

		tprintf("%s||%s||%lu|| | %0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf|| | %0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf",
 			m->node_id, cofs_str[cntr->cofs_t[i]], m->signature.avg_f,
			m->signature.time, tim_proj[0], tim_proj[1], tim_proj[2], tim_proj[3], tim_proj[4],
			m->signature.DC_power, pow_proj[0], pow_proj[1], pow_proj[2], pow_proj[3], pow_proj[4]);

		//fprintf(stderr, "%s %lu\n", mrgd[i].node_id, mrgd[i].signature.def_f);
	}
	

	// Print legend
	fprintf(stderr, "-------------------------\n");
	
	tprintf_init(stderr, "5 12 12");
	tprintf("Idx||Freq. from||Freq. to");	
	for (k = 0; k < COFS_MAX; ++k)
    {
		tprintf("%d||%lu||%lu", k + 1, cntr->freq_ref, cntr->cofs_ref[k]);
	}
}

/*
 *
 * Initialization
 *
 */

void read_applications(control_t *cntr)
{
	application_t *apps;
	int i;

	sprintf(buffer, "SELECT * FROM Applications WHERE job_id = %lu AND step_id = %lu", cntr->job_id, cntr->step_id);

	//
	cntr->n_apps = db_read_applications_query(&apps, buffer);

	if (cntr->n_apps == 0) {
		fprintf(stderr, "No apps found for the job_id '%lu'\n", cntr->job_id);
		exit(1);
	}
	
	//
	cntr->apps = calloc(cntr->n_apps, sizeof(application_t));

	//
	memcpy(cntr->apps, apps, cntr->n_apps * sizeof(application_t));

	// Merging
	merge(cntr);
	
	//
	cntr->freq_ref = cntr->mrgd[0].signature.def_f;
}

void read_coefficients(cluster_conf_t *conf, control_t *cntr)
{
	my_node_conf_t *node;
	coefficient_t *c;	
	int i, j;

	cntr->cofs     = calloc(cntr->n_mrgd, sizeof(coefficient_t *));
	cntr->n_cofs   = calloc(cntr->n_mrgd, sizeof(int));
	cntr->cofs_t   = calloc(cntr->n_mrgd, sizeof(int));
	cntr->cofs_ref = calloc(COFS_MAX, sizeof(unsigned long));

	for (i = 0; i < cntr->n_mrgd; ++i)
	{
		node = get_my_node_conf(conf, cntr->mrgd[i].node_id);

		//
		sprintf(buffer, "%s/island%d/coeffs.%s",
			conf->earlib.coefficients_pathname, node->island, cntr->mrgd[i].node_id);
		
		// Reading the coefficient
		cntr->n_cofs[i] = coeff_file_read(buffer, &cntr->cofs[i]);

		if (cntr->n_cofs[i] <= 0)
		{
			//
			sprintf(buffer, "%s/island%d/coeffs.default",
            	conf->earlib.coefficients_pathname, node->island);
        
			//
			cntr->n_cofs[i] = coeff_file_read(buffer, &cntr->cofs[i]);
			
			if (cntr->n_cofs[i] <= 0) {
				fprintf(stderr, "no coefficients found, exiting\n");
				exit(1);
			}

			cntr->cofs_t[i] = 1;
		}
		else {
			cntr->cofs_t[i] = 0;
		}

		free(node);
	}


	// Getting coefficients format
	c = cntr->cofs[0];

	for(i = 0, j = 0; i < cntr->n_cofs[0] && j < COFS_MAX; ++i)
	{
		if (c[i].available && c[i].pstate_ref == cntr->freq_ref && c[i].pstate != cntr->freq_ref)
		{
			cntr->cofs_ref[j] = c[i].pstate;
			j += 1;
		}
	}
}

void usage(int argc, char *argv[], control_t *cntr)
{
	int i = 0;

	if (argc < 3) {
		fprintf(stdout, "Usage: %s job_id step_id [OPTIONS...]\n\n", argv[0]);
		fprintf(stdout, "  The job_id of the job to project.\n");
		fprintf(stdout, "  The step_id of the job to project.\n");
		exit(1);
	}

	// Basic parametrs
	cntr->job_id = (unsigned long) atoi(argv[1]);
	cntr->step_id = (unsigned long) atoi(argv[2]);
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

	// Read coefficients
	read_coefficients(&conf, &cntr);	

	//
	evaluate(&cntr);

	return 0;
}
