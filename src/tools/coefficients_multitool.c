//
// Created by xgomez on 24/09/18.
//

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
	application_t *apps;
	application_t *apps_merged;
	coefficient_t *coeffs;
	projection_t *projs;
	int n_apps;
	int n_apps_merged;
	int n_coeffs;
	uint f0_mhz;
} control_t;

/*
 *
 * Work bench
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

int find(application_t *apps, int n_apps, char *app_id, uint f0_mhz)
{
	int equal_id = 0;
	int equal_f = 0;
	int i;

	for(i = 0; i < n_apps; ++i)
	{
		equal_id = strcmp(app_id, apps[i].job.app_id) == 0;
		equal_f = f0_mhz == apps[i].signature.def_f;

		if (equal_id && equal_f) {
			return i;
		}
	}

	return -1;
}

application_t *merge(control_t *control)
{
	application_t *apps, *apps_merged;
	int n_apps, equal_id, equal_f, i, j, k;
	double cpi, tpi, power, time, counter;
	char *app_id;
	uint f0_mhz;

	// Allocation
	apps = control->apps;
	n_apps = control->n_apps;

	// Allocation
	apps_merged = (application_t *) malloc(n_apps * sizeof(application_t));
	memset(apps_merged, 0, sizeof(application_t));

	// Initialization
	for(i = 0; i < n_apps; ++i)
	{
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

			power = apps[i].signature.DC_power;
			time = apps[i].signature.time;
			tpi = apps[i].signature.TPI;
			cpi = apps[i].signature.CPI;
			counter = 1.0;

			for(k = i + 1; k < n_apps; ++k)
			{
				equal_id = strcmp(app_id, apps[k].job.app_id) == 0;
				equal_f = f0_mhz == apps[k].signature.def_f;

				if (equal_id && equal_f)
				{
					power += apps[k].signature.DC_power;
					time += apps[k].signature.time;
					tpi += apps[k].signature.TPI;
					cpi += apps[k].signature.CPI;
					counter += 1.0;
				}
			}

			apps_merged[j].signature.DC_power = power / counter;
			apps_merged[j].signature.time = time / counter;
			apps_merged[j].signature.TPI = tpi / counter;
			apps_merged[j].signature.CPI = cpi / counter;

			j += 1;
		}
	}

	control->apps_merged = apps_merged;
	control->n_apps_merged = j;

	return apps_merged;
}

int main(int argc, char *argv[])
{
	char hostname[512];
	char confpath[512];
	cluster_conf_t conf;
	application_t *apps;
	int n_apps = 100;
	int i;

	//
	gethostname(hostname, sizeof(hostname));

	//
	get_ear_conf_path(confpath);
	//conf_path = getenv("EAR_ETC");	

	if (read_cluster_conf(confpath, &conf) != EAR_SUCCESS){
		fprintf(stderr, "Error reading cluster configuration.\n");
		return 0;
	}

	//
	fprintf(stderr, "'%s' '%s' '%s'\n", confpath, conf.database.database, hostname);
	

	//
	init_db_helper(&conf.database);

	//
	db_read_applications(&apps, 0, n_apps, hostname);

	for (i = 0; i < n_apps; ++i) 
	{
		fprintf(stderr, "APP '%s'\n", apps[i].node_id);
	}

	return 0;
}
