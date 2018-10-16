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

int EAR_VERBOSE_LEVEL = 0;
int COLUMNS = 6;

typedef struct control
{
	/* Merged applications */
	application_t *mrgd;
	int mrgd_n;
	/* Coefficients */
	coefficient_t **cofs;
	int *cofs_n;
	int *cofs_s;
	/* Frequency */
	ulong *f_dst;
	ulong  f_src;
	/* Others */
	job_id step_id;
	job_id job_id;
	int compare;
	int cols_n;
} control_t;

static char *cofs_str[2] = { "ok", "def" };
static char buffer [SZ_PATH];
static char buffer2[SZ_PATH];
static char buffer3[SZ_PATH];
static control_t cntr;

/*
 *
 * Work bench
 *
 */

static void print_similars(control_t *cntr, application_t *app)
{
	application_t *apps;
    char tim_proj[COLUMNS][64];
    char pow_proj[COLUMNS][64];
	int napps, nsets, nppns;
	int icol, iapp, finished;

	//
	sprintf(buffer, "SELECT * from Applications WHERE (node_id = '%s') \
						AND (job_id,step_id) IN (SELECT id,step_id FROM Jobs WHERE policy = '%s' AND app_id = '%s' AND procs = %lu) \
						AND ((job_id != %lu) OR (step_id != %lu));",
					app->node_id,
						app->job.policy, app->job.app_id, app->job.procs,
						app->job.id, app->job.step_id);

	//
	napps = db_read_applications_query(&apps, buffer);
	
	while (!finished)
	{
		nsets = 0;
	
		for (icol = 0; icol < COLUMNS; ++icol)
		{
			sprintf(tim_proj[icol], "-");
			sprintf(pow_proj[icol], "-");

			for (iapp = 0; iapp < napps; ++iapp)
			{
				if (apps[iapp].job.def_f == cntr->f_dst[icol])
				{
					sprintf(tim_proj[icol], "%0.2lf", apps[iapp].signature.time); 				
					sprintf(pow_proj[icol], "%0.2lf", apps[iapp].signature.DC_power); 				
	
					apps[iapp].job.def_f = 1;
					iapp = napps;
					nsets += 1;
				}
			}	
		}

		finished = (nsets == 0);

		if (!finished) {
			tprintf("-||-||-|| | %s||%s||%s||%s||%s||%s|| | %s||%s||%s||%s||%s||%s",
				tim_proj[0], tim_proj[1], tim_proj[2], tim_proj[3], tim_proj[4], tim_proj[5],
				pow_proj[0], pow_proj[1], pow_proj[2], pow_proj[3], pow_proj[4], pow_proj[5]);
		}
	}

	free(apps);
}

void evaluate(control_t *cntr)
{
	double cpi_sign, tpi_sign, tim_sign, pow_sign;
	double cpi_proj[COLUMNS];
	double tpi_proj[COLUMNS];
	double tim_proj[COLUMNS];
	double pow_proj[COLUMNS];
	
	application_t *m;
	coefficient_t *c;
	int i, j, k, n, r;

	tprintf_init(stderr, "10 5 8 12 10 10 10 10 10 12 10 10 10 10 10");

	tprintf("%s||Coe.||@%u|| | T. Real||T. 1||T. 2||T. 3||T. 4||T. 5|| | P. Real||P. 1||P. 2||P. 3||P. 4||P. 5",
			cntr->mrgd[0].job.app_id, cntr->mrgd[0].signature.def_f);

	for(i = 0; i < cntr->mrgd_n; ++i)
	{
		m = &cntr->mrgd[i];
		c = cntr->cofs[i];
		n = cntr->cofs_n[i];

		cpi_sign = m->signature.CPI;
		tpi_sign = m->signature.TPI;
		tim_sign = m->signature.time;
		pow_sign = m->signature.DC_power;

		tim_proj[0] = tim_sign;
		pow_proj[0] = pow_sign;

		// Coefficients format
		for (j = 0, r = 1; j < n; j++)
		{
			if (c[j].available && c[j].pstate_ref == cntr->f_src)
			{
				for (k = 1; k < COLUMNS; ++k)
				{
					if (c[j].pstate == cntr->f_dst[k])
					{
						//fprintf(stderr, "r %d ref %lu pstate %lu\n", r, c[j].pstate_ref, c[j].pstate);
						cpi_proj[r] = coeff_project_cpi(cpi_sign, tpi_sign, &c[j]);
						tim_proj[r] = coeff_project_tim(tim_sign, cpi_proj[r], cpi_sign, cntr->f_src, c[j].pstate);
						pow_proj[r] = coeff_project_pow(pow_sign, tpi_sign, &c[j]);
						r += 1;
					}
				}
			}
		}

		tprintf("%s||%s||%lu|| | %0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf|| | %0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf||%0.2lf",
 			m->node_id, cofs_str[cntr->cofs_s[i]], m->signature.avg_f,
			tim_proj[0], tim_proj[1], tim_proj[2], tim_proj[3], tim_proj[4], tim_proj[5],
			pow_proj[0], pow_proj[1], pow_proj[2], pow_proj[3], pow_proj[4], pow_proj[5]);

		if (cntr->compare) {
			print_similars(cntr, m);
		}
	}
	

	// Print legend
	fprintf(stderr, "-------------------------\n");
	
	tprintf_init(stderr, "5 12 12");
	tprintf("Idx||Freq. from||Freq. to");	
	for (k = 1; k < COLUMNS; ++k)
    {
		tprintf("%d||%lu||%lu", k, cntr->f_src, cntr->f_dst[k]);
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
	cntr->mrgd_n = db_read_applications_query(&apps, buffer);

	if (cntr->mrgd_n == 0) {
		fprintf(stderr, "No apps found for the job_id '%lu'\n", cntr->job_id);
		exit(1);
	}
	
	//
	cntr->mrgd = calloc(cntr->mrgd_n, sizeof(application_t));
	memcpy(cntr->mrgd, apps, cntr->mrgd_n * sizeof(application_t));
	free(apps);

	//
	cntr->f_src = cntr->mrgd[0].signature.def_f;
}

void read_coefficients(cluster_conf_t *conf, control_t *cntr)
{
	coefficient_t *c;
	char *node;
	int island;
	int i, j;

	cntr->cofs     = calloc(cntr->mrgd_n, sizeof(coefficient_t *));
	cntr->cofs_n   = calloc(cntr->mrgd_n, sizeof(int));
	cntr->cofs_s   = calloc(cntr->mrgd_n, sizeof(int));
	cntr->f_dst = calloc(COLUMNS, sizeof(unsigned long));

	for (i = 0; i < cntr->mrgd_n; ++i)
	{
		node = cntr->mrgd[i].node_id;
		island = get_node_island(conf, node);

		if (island == EAR_ERROR) {
			fprintf(stderr, "no island found for node %s, exiting\n", node);
			exit(1);
		}

		//
		sprintf(buffer, "%s/island%d/coeffs.%s",
			conf->earlib.coefficients_pathname, island, cntr->mrgd[i].node_id);
		
		// Reading the coefficient
		cntr->cofs_n[i] = coeff_file_read(buffer, &cntr->cofs[i]);

		if (cntr->cofs_n[i] <= 0)
		{
			//
			sprintf(buffer, "%s/island%d/coeffs.default",
            	conf->earlib.coefficients_pathname, island);
        
			//
			cntr->cofs_n[i] = coeff_file_read(buffer, &cntr->cofs[i]);
			
			if (cntr->cofs_n[i] <= 0) {
				fprintf(stderr, "no coefficients found for node %s, exiting\n", node);
				exit(1);
			}

			cntr->cofs_s[i] = 1;
		}
		else {
			cntr->cofs_s[i] = 0;
		}
	}


	// Getting coefficients format
	c = cntr->cofs[0];
	cntr->f_dst[0] = cntr->f_src;

	for(i = 0, j = 1; i < cntr->cofs_n[0] && j < COLUMNS; ++i)
	{
		if (c[i].available && c[i].pstate_ref == cntr->f_src && c[i].pstate != cntr->f_src)
		{
			cntr->f_dst[j] = c[i].pstate;
			j += 1;
		}
	}
}

void usage(int argc, char *argv[], control_t *cntr)
{
	int i = 0;

	if (argc < 3)
	{
		fprintf(stdout, "Usage: %s job.id step.id\n\n", argv[0]);
		fprintf(stdout, "  The job.id of the job to project.\n");
		fprintf(stdout, "  The step.id of the job to project.\n");
		fprintf(stdout, "\nOptions:\n");
		fprintf(stdout, "\t-C, --compare\tShows other jobs of the same application,\n");
		fprintf(stdout, "\t\t\tnode, policy and number of processes.\n");
		exit(1);
	}

	// Basic parametrs
	cntr->job_id = (unsigned long) atoi(argv[1]);
	cntr->step_id = (unsigned long) atoi(argv[2]);

	for (i = 3; i < argc; ++i)
	{
		if (!cntr->compare)
			cntr->compare = ((strcmp(argv[i], "-C") == 0) ||
						 	  (strcmp(argv[i], "--compare") == 0));
	}
}

void init(cluster_conf_t *conf)
{
	// Initialization
	get_ear_conf_path(buffer);

	if (read_cluster_conf(buffer, conf) != EAR_SUCCESS) {
		fprintf(stderr, "Error reading cluster configuration.\n");
		exit(1);
	}

	init_db_helper(&conf->database);
}

static leave(cluster_conf_t *conf)
{

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

	//
	leave(&conf);

	return 0;
}
