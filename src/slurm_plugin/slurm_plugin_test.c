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

#include <common/colors.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_test.h>
#include <slurm_plugin/slurm_plugin_options.h>
#include <slurm_plugin/slurm_plugin_environment.h>

char *fake = "FAKE";
char buffer1[SZ_PATH];
char buffer2[SZ_PATH];

typedef struct test_s {
	char *comp_libr;
	char *comp_plug;
	char *comp_moni;
	char *comp_test;
	char *comp_verb;
	char *policy;
	char *policy_th;
	char *perf_pen;
	char *eff_gain;
	char *frequency;
	char *p_state;
	char *learning;
	char *tag;
	char *path_usdb;
	char *path_trac;
	char *path_temp;
	char *name_app;
	char *verbose;
} test_t;

#define TEST_FREQ	"2400000"

test_t test1 = { .comp_libr = "ON", .verbose = "2" };
test_t test2 = { .comp_libr = "ON", .verbose = "3", .policy = "MONITORING_ONLY",        .frequency = TEST_FREQ };
test_t test3 = { .comp_libr = "ON", .verbose = "4", .policy = "MIN_TIME_TO_SOLUTION",   .frequency = TEST_FREQ };
test_t test4 = { .comp_libr = "ON", .verbose = "2", .policy = "MIN_ENERGY_TO_SOLUTION", .frequency = TEST_FREQ };
test_t test5 = { .comp_libr = "ON", .path_trac = "/tmp", .path_usdb = "/tmp" };

static void fake_build(spank_t sp)
{
	setenv_agnostic(sp, Var.policy.ear, fake, 1);
	setenv_agnostic(sp, Var.policy_th.ear, fake, 1);
	setenv_agnostic(sp, Var.perf_pen.ear, fake, 1);
	setenv_agnostic(sp, Var.eff_gain.ear, fake, 1);
	setenv_agnostic(sp, Var.frequency.ear, fake, 1);
	setenv_agnostic(sp, Var.p_state.ear, fake, 1);
	setenv_agnostic(sp, Var.learning.ear, fake, 1);
	setenv_agnostic(sp, Var.tag.ear, fake, 1);
	setenv_agnostic(sp, Var.path_usdb.ear, fake, 1);
	setenv_agnostic(sp, Var.path_trac.ear, fake, 1);
	setenv_agnostic(sp, Var.path_temp.ear, fake, 1);
	setenv_agnostic(sp, Var.name_app.ear, fake, 1);
	setenv_agnostic(sp, Var.verbose.ear, fake, 1);
}

static int fake_test(spank_t sp)
{
	if (isenv_agnostic(sp, Var.policy.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.policy_th.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.perf_pen.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.eff_gain.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.frequency.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.p_state.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.learning.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.tag.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.path_usdb.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.path_trac.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.path_temp.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.name_app.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.verbose.ear, fake)) return ESPANK_ERROR;
}

static void option_build(spank_t sp, test_t *test)
{
	if (test->comp_libr != NULL) {
		_opt_ear (0, test->comp_libr, 0);
	}
	if (test->verbose != NULL) {
		_opt_ear_verbose(0, test->verbose, 0);
		setenv_agnostic(sp, Var.verbose.tes, test->verbose, 1);
	}
	if (test->policy != NULL) {
 		_opt_ear_policy (0, test->policy, 0);
		setenv_agnostic(sp, Var.policy.tes, test->policy, 1);
	}
	if (test->frequency != NULL) {
 		_opt_ear_frequency(0, test->frequency, 0);
		setenv_agnostic(sp, Var.frequency.tes, test->frequency, 1);
	}
	if (test->path_usdb != NULL) {
		_opt_ear_user_db (0, test->path_usdb, 0);
		setenv_agnostic(sp, Var.path_usdb.tes, test->path_usdb, 1);
	}
	if (test->path_trac != NULL) {
 		_opt_ear_traces (0, test->path_trac, 0);
		setenv_agnostic(sp, Var.path_trac.tes, test->path_trac, 1);
	}

/*
 * _opt_ear_learning (0, "1", 0);
 * _opt_ear_threshold (0, "0.10", 0);
 * _opt_ear_tag (0, "", 0);
 * _opt_ear_mpi_dist (0, "openmpi", 0);
 *  */
}

static int option_result(spank_t sp, test_t *test)
{
	if (test->comp_libr != NULL) {
		if (!plug_component_isenabled(sp, Component.library)) return ESPANK_ERROR;
	}
	if (test->verbose != NULL) {
		if (!isenv_agnostic(sp, Var.verbose.tes, test->verbose)) return ESPANK_ERROR;
	}
	if (test->policy != NULL) {
		if (!isenv_agnostic(sp, Var.policy.tes, test->policy)) return ESPANK_ERROR;
        }
        if (test->frequency != NULL) {
		if (!isenv_agnostic(sp, Var.frequency.tes, test->frequency)) return ESPANK_ERROR;
        }
        if (test->path_usdb != NULL) {
		if (!isenv_agnostic(sp, Var.path_usdb.tes, test->path_usdb)) return ESPANK_ERROR;
        }
        if (test->path_trac != NULL) {
		if (!isenv_agnostic(sp, Var.path_trac.tes, test->path_trac)) return ESPANK_ERROR;
        }

	return ESPANK_SUCCESS;
}

void plug_test_build(spank_t sp)
{
	plug_verbose(sp, 2, "function plug_test_build");

	int test;

	//
	test = plug_component_isenabled(sp, Component.test);
	
	// Simulating sbatch variables inheritance
	fake_build(sp);

	// Simulating option variables
	switch (test) {
		case 1: option_build(sp, &test1); break;
		case 2: option_build(sp, &test2); break;
		case 3: option_build(sp, &test3); break;
		case 4: option_build(sp, &test4); break;
		case 5: option_build(sp, &test5); break;
		default: return;
	}
}

void plug_test_result(spank_t sp)
{
	int result;
	int test;

	//
	test = plug_component_isenabled(sp, Component.test);

	//
	if (fake_test(sp) != ESPANK_SUCCESS) {
		plug_verbose(sp, 0, "plugin test %sFAILED%s (fakes)", COL_RED, COL_CLR);
		return;
	}

	//
	switch (test) {
		case 1: result = option_result(sp, &test1); break;
		case 2: result = option_result(sp, &test2); break;
		case 3: result = option_result(sp, &test3); break;
		case 4: result = option_result(sp, &test4); break;
		case 5: result = option_result(sp, &test5); break;
		default: return;
	}

	if (result != ESPANK_SUCCESS) {
		plug_verbose(sp, 0, "plugin test FAILED (options)", COL_RED, COL_CLR);
		return;
	}

	plug_verbose(sp, 0, "plugin option test %sSUCCESS%s (%d)", COL_GRE, COL_CLR, test);
}
