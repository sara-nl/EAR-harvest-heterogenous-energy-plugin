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

#include <common/file.h>
#include <slurm_plugin/slurm_plugin.h>
#include <slurm_plugin/slurm_plugin_test.h>
#include <slurm_plugin/slurm_plugin_environment.h>

const char *fake = "FAKE";
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
	char *name_app;
	char *path_usdb;
	char *path_trac;
	char *path_temp;
	char *verbose;
} test_t;

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
	if (isenv_agnostic(sp, Var.verbose.ear, fake)) return ESPANK_ERROR;
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
	if (isenv_agnostic(sp, Var.name_app.ear, fake)) return ESPANK_ERROR;
	if (isenv_agnostic(sp, Var.path_temp.ear, fake)) return ESPANK_ERROR;
}

static void option_build(spank_t sp, test_t test)
{
	/*
	if (e != NULL) {
		_opt_ear (0, e, 0);
	}
	if (v != NULL) {
		_opt_ear_verbose(0, v, 0);
		setenv_agnostic(sp, Var.verbose.tes, v, 1);
	}
	 */
}

static int option_test(spank_t sp, test_t test)
{
	return ESPANK_SUCCESS;
}

void plug_test_build(spank_t sp)
{
	int test;

	//
	test = 1;

	// Simulating sbatch variables inheritance
	fake_build(sp);

	// Simulating option variables
	switch (test)
	{
		case 1: option_build(sp, NULL); break;
		default: return;
	}
}

/*
_opt_ear_learning (0, "1", 0);
_opt_ear_policy (0, "MONITORING_ONLY", 0);
_opt_ear_frequency (0, "2400000", 0);
_opt_ear_threshold (0, "0.10", 0);
_opt_ear_tag (0, "", 0);
_opt_ear_user_db (0, "/tmp/", 0);
_opt_ear_traces (0, "", 0);
_opt_ear_mpi_dist (0, "openmpi", 0);
_opt_ear_verbose (0, "", 0);
 */

void plug_test_result(spank_t sp)
{
	int result;
	int test;

	//
	test = 1;

	//
	if (fake_test(sp) != ESPANK_SUCCESS) {
		plug_error(sp, "plugin fake test failed");
		return;
	}
	plug_verbose(sp, 0, "plugin fake test success");

	//
	switch (test)
	{
		case 1: result = option_test(sp, NULL); break;
		default: return;
	}

	if (result != ESPANK_SUCCESS) {
		plug_error(sp, "plugin option test failed");
	}
	plug_verbose(sp, 0, "plugin option test success");
}