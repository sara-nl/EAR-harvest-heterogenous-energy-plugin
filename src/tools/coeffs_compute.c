/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#include <common/output/verbose.h>
#include <common/string_enhanced.h>
#include <common/hardware/topology.h>
#include <common/types/application.h>
#include <common/types/configuration/cluster_conf.h>
#include <common/database/db_helper.h>
#include <management/cpufreq/cpufreq.h>

// Temporal buffers
static char buffer1[SZ_PATH];
static char buffer2[SZ_PATH];

//
typedef struct matrix_s {
	application_t *app_list;
	uint           app_count;
	uint           a;
	pstate_t       pstate;
	coefficient_t *coef_list;
} matrix_t;


static int find_app(marix_t *matrix, char *app_name)
{
	for (a = 0; a < matrix->app_count; ++a) {
		if (strcmp(matrix->app_list[a].job.app_id, app_name) == 0) {
			return a;
		}
	}
	return -1;
}

static state_t compute_time()
{
	
}

static void compute_power(matrix_t *matrix_base, matrix_t *matrix_target)
{
	int n, a, b;
	//
	n = matrix_target->app_count;
	//
	gsl_matrix *signature_base = gsl_matrix_calloc(n, 3);
	gsl_vector *power_target   = gsl_vector_alloc(n);
	gsl_vector *coefficients   = gsl_vector_alloc(3);
	//
	for (a = 0; a < n; ++a) {
		// Initializations
		gsl_vector_set(power_target, a, 0.0);
		gsl_matrix_set(signature_base, a, 0, 1.0);
		gsl_matrix_set(signature_base, a, 1, 0.0);
		gsl_matrix_set(signature_base, a, 2, 0.0);
		// Trying to find the base application.
		if ((b = find_app()) != -1) {
			// Target applications power
			gsl_vector_set(power_target, a, matrix_target->app_list[a].signature.DC_power);
			// Base applications bias, power and TPI
			gsl_matrix_set(signature_base, a, 0, 1.0);
			gsl_matrix_set(signature_base, a, 1, matrix_base->app_list[b].signature.DC_power);
			gsl_matrix_set(signature_base, a, 2, matrix_base->app_list[b].signature.TPI);
		}
	}
	
	// Linear least squares fitting (SVD algorithm)
	gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, 3);
	// Variance-covariance matrix
	gsl_matrix *cov = gsl_matrix_alloc(3, 3);
	// Sum of squares of the residuals from the best-fit
	double chisq;
	// Best fit of power_target = signature_base*coefficients
	gsl_multifit_linear(signature_base, power_target, coefficients, cov, &chisq, work);
	// Saving coefficient values
	matrix_base->coef_list.A = gsl_vector_get(coefficients, 1);
	matrix_base->coef_list.B = gsl_vector_get(coefficients, 2);
	matrix_base->coef_list.C = gsl_vector_get(coefficients, 0);
	// Freeing space
	gsl_vector_free(coefficients);
	gsl_vector_free(power_target);
	gsl_matrix_free(signaure_base);
	gsl_matrix_free(cov);
	gsl_multifit_linear_free(work);
}

static state_t compute_coefficients(matrix_t *matrix, uint matrix_count)
{
	int m, p;
	// Allocating coefficients
	for (m = 0; m < matrix_count; ++m) {
		if (matrix[m].app_count == 0) {
			continue;
		}
		matrix[m].coef_list = calloc(matrix_count, sizeof(coefficient_t));
		// Giving start values to coefficient
		for (p = 0; p < matrix_count; ++p) {
			// pstate_ref is the P_STATE base (from what frequency)
			// pstate is the P_STATE target (to what frequency)
			matrix[m].coef_list[p].pstate_ref = matrix[m].pstate.khz;
			matrix[m].coef_list[p].pstate     = matrix[p].pstate.khz;
			matrix[m].coef_list[p].available  = (matrix[p].app_count > 0);
			// If it is the same frequency, it is identity
			if (matrix[m].pstate.idx == matrix[p].pstate.idx) {
				matrix[m].coef_list[p].A = 1;
				matrix[m].coef_list[p].D = 1;
			}
		}
	}
	// Computing coefficients
	for (m = 0; m < matrix_count; ++m) {
		if (matrix[m].app_count == 0) {
			continue;
		}
		for (p = 0; p < matrix_count; ++p) {
			if (matrix[p].app_count > 0) {
				compute_power(&matrix[m], &matrix[p]);
				compute_time(&matrix[m], &matrix[p]);
			}
		}
	}
}

static state_t matrix_fill(application_t *apps, uint apps_count, matrix_t *matrix, uint matrix_count)
{
	int m, a, f;
	// Counting applications per frequency
	for(m = 0; m < matrix_count; ++m) {
		for(a = 0; a < apps_count; ++a) {
			matrix[m].app_count += (matrix[m].pstate.khz == apps[a].signature.def_f);
		}
	}
	verbose(0, "matrix count %d", matrix_count);
	// Filling applications in the matrix
	for(m = 0; m < matrix_count; ++m) {
		verbose(0, "P_STATE %d (%llu KHz), %u apps", matrix[m].pstate.idx, matrix[m].pstate.khz, matrix[m].app_count);
		if (matrix[m].app_count == 0) {
			continue;
		}
		matrix[m].app_list = calloc(matrix[m].app_count, sizeof(application_t));
		for(a = 0; a < apps_count; ++a) {
			if (matrix[m].pstate.khz == apps[a].signature.def_f) {	
				copy_application(&matrix[m].app_list[matrix[m].a], &apps[a]);
				matrix[m].a += 1;
			}
		}
	}
	return EAR_SUCCESS;
}

static state_t matrix_init(pstate_t *pstate_list, uint pstate_count, matrix_t **matrix, uint *matrix_count)
{
	int p;
	// Allocating space for the matrix
	*matrix = calloc(pstate_count, sizeof(matrix_t));
	*matrix_count = pstate_count;
	// Copying each P_STATE 
	for(p = 0; p < pstate_count; ++p) {
		(*matrix)[p].pstate = pstate_list[p];
	}
	return EAR_SUCCESS;
}

static void average(signature_t *sig)
{
	double samples_count = (double) sig->cycles;
	sig->time     = (sig->time / samples_count);
	sig->GBS      = (sig->GBS  / samples_count);
	sig->DC_power = (sig->DC_power / samples_count);
	sig->CPI      = (sig->CPI  / samples_count);
	sig->TPI      = (sig->TPI  / samples_count);
}

static void accum(signature_t *d, signature_t *s)
{
	d->cycles   += 1; // Using cycles as counter, genius
	d->time     += s->time;
	d->GBS      += s->GBS;
	d->DC_power += s->DC_power;
	d->TPI      += s->TPI;
	d->CPI      += s->CPI;
}

static state_t apps_merge(application_t **apps, uint *apps_count)
{
	application_t *apps_o; // Original apps array
	application_t *apps_n; // Apps by name accumulated
	uint apps_n_count = 0;
	int o, b, n;

	//
	apps_o = *apps;
	//
	apps_n = calloc(*apps_count, sizeof(application_t));
	// Copying the first ocurrence of each app in apps_n
	for(o = 0; o < *apps_count; ++o) {
		for (b = 0; b < o; ++b) {
			if ((strcmp(apps_o[o].job.app_id, apps_o[b].job.app_id) == 0) &&
			    (apps_o[o].signature.def_f == apps_o[b].signature.def_f)) {
					break;
			}
		}
		// If equal means that didn't found
		if (o == b) {
			// Copying just the name and frequency
			strcpy(apps_n[apps_n_count].job.app_id, apps_o[o].job.app_id);
			apps_n[apps_n_count].signature.def_f  = apps_o[o].signature.def_f;
			apps_n[apps_n_count].signature.cycles = 1; // Using cycles as counter, genius
			apps_n_count += 1;
		}
	}
	// Accumulating signature values
	for(o = 0; o < *apps_count; ++o) {
		for (n = 0; n < apps_n_count; ++n) {
			if ((strcmp(apps_o[o].job.app_id, apps_n[n].job.app_id) == 0) &&
                (apps_o[o].signature.def_f == apps_n[n].signature.def_f)) {
				accum(&apps_n[n].signature, &apps_o[o].signature);
				break;
            }
		}
	}
	// Averaging
	for (n = 0; n < apps_n_count; ++n) {
		average(&apps_n[n].signature);
	}
	// Converting apps to apps_n
	free(*apps);
	*apps = apps_n;
	*apps_count = apps_n_count;

	return EAR_SUCCESS;	
}

static state_t db_query(int argc, char *argv[], cluster_conf_t *conf, char *host_name, application_t **apps, uint *apps_count)
{
    application_t *apps_temp;
	char *p_list = buffer1;
	char *p_elem = buffer2;
	int job_list;
    int t, a;
    //
    init_db_helper(&conf->database);
    //
    if ((*apps_count = get_num_applications(1, host_name)) <= 0) {
        return_msg(EAR_ERROR, "while reading applications from this hostname");
	}
    if ((*apps_count = db_read_applications(&apps_temp, 1, *apps_count, host_name)) <= 0) {
        return_msg(EAR_ERROR, "while reading applications from database");
    }
	//
	job_list = strinargs(argc, argv, "job-list:", p_list);
    //
    for (t = 0, a = 0; t < *apps_count; t++){
		if (job_list) {
			sprintf(buffer2, "%lu", apps_temp[t].job.id);
			if (!strinlist(p_list, ",", p_elem)) {
				apps_temp[t].job.id = (ulong) -1L;
				continue;
			} 
		}
        a += 1;
    }
	verbose(0, "total apps:    %d", *apps_count);
	*apps = calloc(a, sizeof(application_t));
    for (t = 0, a = 0; t < *apps_count; t++){
        if (apps_temp[t].job.id != (ulong) -1L) {
            copy_application(&((*apps)[a]), &apps_temp[t]);
			a += 1;
		}
	}
	*apps_count = a;	
    free(apps_temp);
	verbose(0, "selected apps: %d", *apps_count);
	verbose(0, "-----------------------------------------------------------------------------------"); 

    return EAR_SUCCESS;
}

static state_t hostname(char *name, char *alias)
{
    char aux[SZ_PATH];
    // Hostname processing
    if (gethostname(aux, SZ_PATH) < 0) {
		return_msg(EAR_ERROR, strerror(errno));
    }
    if (name != NULL) {
        strcpy(name, aux);
    }
    if (alias != NULL) {
        strtok(aux, ".");
        strcpy(alias, aux);
    }
    return EAR_SUCCESS;
}

static state_t configuration(int argc, char *argv[], cluster_conf_t *conf, my_node_conf_t **node, char *host_name)
{
    char aux_alias[SZ_PATH];
    char aux_name[SZ_PATH];
    char path[SZ_PATH];
	char *p = aux_name;

    if (state_fail(get_ear_conf_path(path))) {
       	return_msg(EAR_ERROR, "error getting ear.conf path");
    }
    if (state_fail(read_cluster_conf(path, conf))) {
        return_msg(EAR_ERROR, "reading cluster conf");
    }
	// Null terminated alias to prevent problems
	aux_alias[0] = '\0';
	//
	if (!strinargs(argc, argv, "node-name:", aux_name)) {
    	if (state_fail(hostname(aux_name, aux_alias))) {
        	return EAR_ERROR;
    	}
	}
    if ((*node = get_my_node_conf(conf, aux_name)) == NULL) {
        // Testing alias
        if ((*node = get_my_node_conf(conf, aux_alias)) == NULL) {
       		return_xmsg(EAR_ERROR, "host name '%s' and alias '%s' not found", aux_name, aux_alias);
		}
		p = aux_alias;
	}
	strcpy(host_name, p);
	verbose(0, "-----------------------------------------------------------------------------------"); 
	verbose(0, "host name:     %s", host_name); 
	verbose(0, "coeff file:    %s", (*node)->coef_file);
	verbose(0, "file path:     %s", path);
    return EAR_SUCCESS;
}

static state_t usage(int argc, char *argv[])
{
	int expected_args = 1;
	int node_name     = 0;
	int job_list      = 0;
	int path          = 0;
	int help          = 0;
	// More than one means something to process
	if (argc > 1) {
		help      = strinargs(argc, argv, "help", NULL);
		job_list  = strinargs(argc, argv, "job-list:", NULL);
		node_name = strinargs(argc, argv, "node-name:", NULL);
		path      = strinargs(argc, argv, "path:", NULL);
		expected_args = job_list + node_name + path + 1;
	}
	// Something happens in the arguments
	if (argc == expected_args) {
    	return EAR_SUCCESS;
	}
	if (!help) {
    	return EAR_SUCCESS;
	}
	// Showing usage
    verbose(0, "Usage: %s", argv[0]);
    verbose(0, "  Computes a coefficients file for a given node.");
    verbose(0, "\nOptions:");
	verbose(0, "\t--job-list <list>");
    verbose(0, "\t--node-name <name>");
    verbose(0, "\t--path <path>");

    return EAR_ERROR;
}

state_t mgt_cpufreq_alloc_available(ctx_t *c, pstate_t **pstate_list, uint *pstate_count)
{
	mgt_cpufreq_count(c, pstate_count);
	*pstate_list = calloc(*pstate_count, sizeof(pstate_t));
	return EAR_SUCCESS;
}

state_t mgt_cpufreq_alloc_current(ctx_t *c, pstate_t **pstate_list, uint *pstate_count)
{
	return EAR_ERROR;
}

int main(int argc, char *argv[])
{
    char             host_name[SZ_NAME_MEDIUM];
    my_node_conf_t  *node;
    cluster_conf_t   conf;
    topology_t       topo;
    ctx_t            c;
    pstate_t        *pstate_list;
    uint             pstate_count;
    application_t   *app_list;
    uint             app_count;
	state_t          s;
	matrix_t        *matrix;
	uint             matrix_count;

    if (xtate_fail(s, usage(argc, argv))) {
        return 0;
    }
    if (xtate_fail(s, topology_init(&topo))) {
        error("while initializing topology %d (%s)", s, state_msg);
		return 0;
    }
    if (xtate_fail(s, mgt_cpufreq_load(&topo))) {
        error("during cpufreq loading %d (%s)", s, state_msg);
		return 0;
    }
    if (xtate_fail(s, mgt_cpufreq_init(&c))) {
        error("during cpufreq initialization %d (%s)", s, state_msg);
		return 0;
    }
    if (xtate_fail(s, mgt_cpufreq_alloc_available(&c, &pstate_list, &pstate_count))) {
        error("during cpufreq data allocation %d (%s)", s, state_msg);
        return s;
    }
    if (xtate_fail(s, mgt_cpufreq_get_available_list(&c, pstate_list, NULL))) {
        error("during cpufreq list available frequencies %d (%s)", s, state_msg);
    }
	state_assert(s, configuration(argc, argv, &conf, &node, host_name),             return 0);
	state_assert(s, db_query(argc, argv, &conf, host_name, &app_list, &app_count),  return 0);
	state_assert(s, apps_merge(&app_list, &app_count),                              return 0);
	state_assert(s, matrix_init(pstate_list, pstate_count, &matrix, &matrix_count), return 0);
	state_assert(s, matrix_fill(app_list, app_count, matrix, matrix_count),         return 0);

    return 0;
}
