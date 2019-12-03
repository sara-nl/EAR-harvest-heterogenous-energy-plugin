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
	char *cmp;
	char *hck;
} varnames_t;

struct variables_s {
	varnames_t comp_libr;
	varnames_t comp_plug;
	varnames_t comp_moni;
	varnames_t comp_test;
	varnames_t comp_verb;
	varnames_t hack_libr;
	varnames_t verbose;
	varnames_t policy;
	varnames_t policy_th;
	varnames_t frequency;
	varnames_t p_state;
	varnames_t learning;
	varnames_t tag;
	varnames_t path_usdb;
	varnames_t path_trac;
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
	varnames_t version;
	varnames_t gm_secure;
}
	Var __attribute__((weak)) =
{
.comp_libr = { .cmp = "SLURM_COMP_LIBRARY" },
.comp_plug = { .cmp = "SLURM_COMP_PLUGIN"  },
.comp_moni = { .cmp = "SLURM_COMP_MONITOR" },
.comp_test = { .cmp = "SLURM_COMP_TEST"    },
.comp_verb = { .cmp = "SLURM_COMP_VERBOSE" },
.hack_libr = { .hck = "SLURM_HACK_LIBRARY" },
.verbose   = { .loc = "SLURM_LOC_VERB", .ear = "EAR_VERBOSE"          },
.policy    = { .loc = "SLURM_LOC_POLI", .ear = "EAR_POWER_POLICY"     },
.policy_th = { .loc = "SLURM_LOC_POTH", .ear = "EAR_POWER_POLICY_TH"  },
.frequency = { .loc = "SLURM_LOC_FREQ", .ear = "EAR_FREQUENCY"        },
.p_state   = { .loc = "SLURM_LOC_PSTA", .ear = "EAR_P_STATE"          },
.learning  = { .loc = "SLURM_LOC_LERN", .ear = "EAR_LEARNING_PHASE"   },
.tag       = { .loc = "SLURM_LOC_ETAG", .ear = "EAR_ENERGY_TAG"       },
.path_usdb = { .loc = "SLURM_LOC_USDB", .ear = "EAR_USER_DB_PATHNAME" },
.path_trac = { .loc = "SLURM_LOC_TRAC", .ear = "SLURM_EAR_TRACE_PATH" },
.gm_secure = { .loc = "SLURM_LOC_GMSC", .ear = ""                     },
.perf_pen  = { .ear = "EAR_PERFORMANCE_PENALTY"                       },
.eff_gain  = { .ear = "EAR_MIN_PERFORMANCE_EFFICIENCY_GAIN"           },
.name_app  = { .rem = "SLURM_JOB_NAME",      .ear = "EAR_APP_NAME"    },
.user      = { .rem = "SLURM_ERUSER",        .ear = "" },
.group     = { .rem = "SLURM_ERGRUP",        .ear = "" },
.account   = { .rem = "SLURM_JOB_ACCOUNT",   .ear = "" },
.path_temp = { .rem = "SLURM_ERTEMP",        .ear = "EAR_TMP"         },
.path_inst = { .rem = "SLURM_ERINST",        .ear = "" },
.node_list = { .rem = "SLURM_STEP_NODELIST", .ear = "" },
.context   = { .rem = "SLURM_ERCNTX",        .ear = "" },
.ld_prel   = { .rem = "",                    .ear = "LD_PRELOAD"      }s,
.ld_libr   = { .rem = "",                    .ear = "LD_LIBRARY_PATH" },
.node_num  = { .loc = "SLURM_NNODES",        .ear = "" },
.version   = { .loc = "SLURM_EAR_MPI_VERSION", .ear = ""              }
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

int valenv_agnostic(spank_t sp, char *var, int *val);

void printenv_agnostic(spank_t sp, char *var);

/*
 * Others
 */
int plug_component_setenabled(spank_t sp, plug_component_t comp, int enabled);

int plug_component_isenabled(spank_t sp, plug_component_t comp);

int plug_context_is(spank_t sp, plug_context_t ctxt);

int plug_verbosity_test(spank_t sp, int level);

char *plug_acav_get(int ac, char *av[], char *string);

#endif //EAR_SLURM_PLUGIN_ENVIRONMENT_H
