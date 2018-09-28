//
// Created by xgomez on 24/09/18.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/states.h>
#include <common/types/projection.h>
#include <common/types/application.h>
#include <common/types/coefficient.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/database/db_helper.h>
#include <tools/coefficients_multitool.h>

int EAR_VERBOSE_LEVEL = 0;

typedef struct control
{
	coefficient_v3_t *coeffs;
	application_t *apps;
	application_t *apps_merged;
	projection_t *projs;
	int n_coeffs;
	int n_apps;
	int n_apps_merged;
	uint f0_mhz;
} control_t;

/*
 *
 * Work bench
 *
 */

double power_proj(double power0, double tpi0, coefficient_v3_t *coeffs)
{
	return coeffs->A * power0 + coeffs->B * tpi0 + coeffs->C;
}

double cpi_proj(double cpi0, double tpi0, coefficient_v3_t *coeffs)
{
	return coeffs->D * cpi0 + coeffs->E * tpi0 + coeffs->F;
}

double time_proj(double time0, double cpip, double cpi0, ulong f0, ulong fn)
{
	return (time0 * cpip / cpi0) * ((double) f0 / (double) fn);
}

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
	char buffer[32];
	double cpi0, tpi0, tim0, pow0;
	double cpip, tpip, timp, powp;
	double cpie, tpie, time, powe;

	application_t *apps, *m;
	coefficient_v3_t *coeffs;
	int n_apps, n_coeffs;
	int i, j, k;
	uint f0_mhz;

	buffer[0] = '\0';
	f0_mhz   = control->f0_mhz;
	coeffs   = control->coeffs;
	apps     = control->apps_merged;
	n_coeffs = control->n_coeffs;
	n_apps   = control->n_apps_merged;
			
	tprintf_init(stderr, "18 11 15 12 12 15 12 12");

	for (j = 0; j < n_apps; ++j)
	{
		if (apps[j].signature.def_f == f0_mhz)
		{
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
					cpip = cpi_proj(cpi0, tpi0, &coeffs[i]);
					timp = time_proj(tim0, cpip, cpi0, f0_mhz, coeffs[i].pstate);
					powp = power_proj(pow0, tpi0, &coeffs[i]);

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
						tprintf(buffer, "->||%lu|| | -||-||-|| | -||-||-", coeffs[i].pstate);
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	char paths[512];
	int n_apps = 1000;
	int i, j, n, cmp;
	unsigned int learning = 0;
	char *node;

	cluster_conf_t conf;
	application_t *apps;
	control_t cntr;

	// argv[1]: node
	// argv[2]: f0_mhz
	// argv[3]: learning
	cntr.f0_mhz = (unsigned long) atoi(argv[2]);
	learning = (unsigned int) atoi(argv[3]);
	node = argv[1];

	// Reading ear.conf
	get_ear_conf_path(paths);

	if (read_cluster_conf(paths, &conf) != EAR_SUCCESS){
		fprintf(stderr, "Error reading cluster configuration.\n");
		return 0;
	}

	// Initializing database
	fprintf(stderr, "Reading applications: '%s' '%s' '%s' '%u'\n", paths, conf.database.database, node, learning);

	// Reading applications
	init_db_helper(&conf.database);
	cntr.n_apps = db_read_applications(&apps, learning, n_apps, node);

	if (cntr.n_apps == 0) {
		fprintf(stderr, "No apps found\n");
		return 0;
	}

	// Allocating applications
	cntr.apps = calloc(cntr.n_apps, sizeof(application_t));

	for (i = 0; i < cntr.n_apps; ++i) {
		memcpy(&cntr.apps[i], &apps[i], sizeof(application_t));
	}

    for (i = 0; i < cntr.n_apps; ++i)
    {
        fprintf(stderr, "%d: '%s' '%s' '%lu'\n",
                i, cntr.apps[i].node_id, cntr.apps[i].job.app_id, cntr.apps[i].signature.def_f);
    }

	// Reading coefficients
	cntr.coeffs = malloc(2592);
	sprintf(paths, "%s/island0/coeffs.%s", conf.earlib.coefficients_pathname, node);
	cntr.n_coeffs = read_coefficients_file_v3(paths, cntr.coeffs, 2592);
	fprintf(stderr, "Reading coefficients: '%s' '%d'\n", paths, cntr.n_coeffs);

    for (i = 0; i < cntr.n_coeffs; ++i)
    {
		print_coefficient_v3(&cntr.coeffs[i]);
    }


	// Merging
	merge(&cntr);

	// Evaluating
	evaluate(&cntr);

	// Leaving
	free(cntr.coeffs);
	free(cntr.apps_merged);
	free(cntr.apps);

	return 0;
}
