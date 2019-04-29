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

#ifndef EAR_SLURM_PLUGIN_ENVIRONMENT_H
#define EAR_SLURM_PLUGIN_ENVIRONMENT_H

// Verbosity
#define plug_verbose(sp, level, ...) \
        if (plug_verbosity_test(sp, level) == 1) { \
                slurm_error("EARPLUG, " __VA_ARGS__); \
        }
#define plug_error(sp, ...) \
        if (plug_verbosity_test(sp, 1) == 1) { \
                slurm_error("EARPLUG ERROR, " __VA_ARGS__); \
        }

typedef char *plug_component_t;
typedef int   plug_context_t;

struct component_s {
	plug_component_t plugin;
	plug_component_t library;
	plug_component_t monitor;
	plug_component_t test;
	plug_component_t verbose;
} Component __attribute__((weak)) = {
	.plugin  = "SLURM_ECPLUG",
	.library = "SLURM_ECLIBR",
	.monitor = "SLURM_ECMONI",
	.test    = "SLURM_ECTEST",
	.verbose = "SLURM_ECVERB"
};

struct context_s {
	plug_context_t srun;
	plug_context_t sbatch;
	plug_context_t remote;
	plug_context_t local;
} Context __attribute__((weak)) = {
	.srun   = S_CTX_LOCAL,
	.sbatch = S_CTX_ALLOCATOR,
	.remote = S_CTX_REMOTE,
	.local  = -1
};

typedef struct varname_s {
	char *loc;
	char *rem;
	char *ear;
} varnames_t;

struct variables_s {
	varnames_t comp_libr;
	varnames_t comp_plug;
	varnames_t comp_moni;
	varnames_t comp_test;
	varnames_t comp_verb;
	varnames_t verbose;
	varnames_t policy;
	varnames_t policy_th;
	varnames_t frequency;
	varnames_t p_state;
	varnames_t learning;
	varnames_t tag;
	varnames_t path_usdb;
	varnames_t path_trac;
	varnames_t mpi_dist;
	varnames_t perf_pen;
	varnames_t eff_gain;
	varnames_t name_app;
	varnames_t user;
	varnames_t group;
	varnames_t account;
	varnames_t path_temp;
	varnames_t path_inst;
	varnames_t node_list;
	varnames_t context;
	varnames_t ld_prel;
	varnames_t ld_libr;
	varnames_t node_num;
}
	Var __attribute__((weak)) =
{
.comp_libr = { .loc = "SLURM_COMP_LIBRARY", .rem = "", .ear = "" },
.comp_plug = { .loc = "SLURM_COMP_PLUGIN",  .rem = "", .ear = "" },
.comp_moni = { .loc = "SLURM_COMP_MONITOR", .rem = "", .ear = "" },
.comp_test = { .loc = "SLURM_COMP_TEST",    .rem = "", .ear = "" },
.comp_verb = { .loc = "SLURM_COMP_VERBOSE", .rem = "", .ear = "" },
.verbose   = { .loc = "SLURM_EOVERB", .rem = "", .ear = "EAR_VERBOSE" },
.policy    = { .loc = "SLURM_EOPOLI", .rem = "", .ear = "EAR_POWER_POLICY" },
.policy_th = { .loc = "SLURM_EOPOTH", .rem = "", .ear = "EAR_POWER_POLICY_TH" },
.perf_pen  = { .loc = "",             .rem = "", .ear = "EAR_PERFORMANCE_PENALTY" },
.eff_gain  = { .loc = "",             .rem = "", .ear = "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN" },
.frequency = { .loc = "SLURM_EOFREQ", .rem = "", .ear = "EAR_FREQUENCY" },
.p_state   = { .loc = "SLURM_EOPSTA", .rem = "", .ear = "EAR_P_STATE" },
.learning  = { .loc = "SLURM_EOLERN", .rem = "", .ear = "EAR_LEARNING_PHASE" },
.tag       = { .loc = "SLURM_EOETAG", .rem = "", .ear = "EAR_ENERGY_TAG" },
.path_usdb = { .loc = "SLURM_EOUSDB", .rem = "", .ear = "EAR_USER_DB_PATHNAME" },
.path_trac = { .loc = "SLURM_EOTRAC", .rem = "", .ear = "EAR_PATH_TRACE" },
.mpi_dist  = { .loc = "SLURM_EOMPID", .rem = "SLURM_ERMPID", .ear = "" },
.name_app  = { .loc = "", .rem = "SLURM_JOB_NAME",      .ear = "EAR_APP_NAME" },
.user      = { .loc = "", .rem = "SLURM_ERUSER",        .ear = "" },
.group     = { .loc = "", .rem = "SLURM_ERGRUP",        .ear = "" },
.account   = { .loc = "", .rem = "SLURM_JOB_ACCOUNT",   .ear = "" },
.path_temp = { .loc = "", .rem = "SLURM_ERTEMP",        .ear = "EAR_TMP" },
.path_inst = { .loc = "", .rem = "SLURM_ERINST",        .ear = "" },
.node_list = { .loc = "", .rem = "SLURM_STEP_NODELIST", .ear = "" },
.context   = { .loc = "", .rem = "SLURM_ERCNTX",        .ear = "" },
.ld_prel   = { .loc = "", .rem = "", .ear = "LD_PRELOAD" },
.ld_libr   = { .loc = "", .rem = "", .ear = "" },
.node_num  = { .loc = "SLURM_NNODES", .rem = "",        .ear = "" }
};

/*
 * Agnostic environment manipulation
 */
int unsetenv_agnostic(spank_t sp, char *var);

int setenv_agnostic(spank_t sp, char *var, const char *val, int ow);

int getenv_agnostic(spank_t sp, char *var, char *buf, int len);

int isenv_agnostic(spank_t sp, char *var, char *val);

int repenv_agnostic(spank_t sp, char *var_old, char *var_new);

int apenv_agnostic(char *dst, char *src, int len);

int exenv_agnostic(spank_t sp, char *var);

void printenv_agnostic(spank_t sp, char *var);

/*
 * Components
 */
int plug_component_setenabled(spank_t sp, plug_component_t comp, int enabled);

int plug_component_isenabled(spank_t sp, plug_component_t comp);

/*
 * Context
 */
int plug_context_is(spank_t sp, plug_context_t ctxt);

/*
 * Verbosity
 */
int plug_verbosity_test(spank_t sp, int level);

#endif //EAR_SLURM_PLUGIN_ENVIRONMENT_H
