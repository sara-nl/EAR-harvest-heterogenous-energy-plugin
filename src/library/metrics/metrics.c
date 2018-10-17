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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>

#include <common/config.h>
#include <metrics/metrics.h>
#if 0
#include <metrics/papi/flops.h>
#include <metrics/papi/cache.h>
#include <metrics/papi/generics.h>
#include <metrics/papi/instructions.h>
#endif
#include <library/metrics/metrics.h>
#include <common/types/signature.h>
#include <daemon/eard_api.h>
#include <common/ear_verbose.h>
#include <common/states.h>
#include <common/math_operations.h>
#include <library/common/externs.h>

/*
 * Low level reading
 ********************
 *            | P?   | D | Type              | Size F.| Accum/Int. | Unit | Notes
 * ---------- | ---- | - | ----------------- | ------ | ---------- | ---- | ----------------------------
 * Instructs  | PAPI | x | * long long (x2)  | No     | Yes/Yes    | num  | No read (just accum)
 * Cache      | PAPI | x | * long long (x3)  | No     | Yes/Yes    | num  | No read (just accum)
 * Stalls     | PAPI | x | * long long       | No     | Yes/Yes    | num  | No read (just accum)
 * Flops      | PAPI | x | * long long (vec) | Yes    | Yes/Yes    | ops  | No read (just accum)
 * RAPL       | PAPI | v | * ull (vec)       | Yes*   | Yes/Yes**  | nJ   | *Macro, **no daemon service
 * IPMI       | IPMI | v | * ulong           | No     | Yes/No     | mJ   | Water reads J, IBMAEM reads uJ
 * Bandwith   | Cstm | v | * ull (vec)       | Yes    | No/No      | ops  | Vector (r,w,r,w.. so on)
 * Avg. Freq. | Cstm | v | * ulong           | No     | No/Yes*    | MHz? | No start/stop architecture
 *
 * Daemon services
 ******************
 * Client                                         | Req                 | Daemon
 * ---------------------------------------------- | ------------------- | -----------
 * eards_get_data_size_rapl           | DATA_SIZE_RAPL      | eard_rapl
 * eards_start_rapl                   | START_RAPL          |
 * eards_read_rapl                    | READ_RAPL           |
 * eards_reset_rapl                   | RESET_RAPL          |
 * ---------------------------------------------- | ------------------- | ----------------
 * eards_get_data_size_uncore         | DATA_SIZE_UNCORE    | eard_uncore
 * eards_start_uncore                 | START_UNCORE        |
 * eards_read_uncore                  | READ_UNCORE         |
 * eards_reset_uncore                 | RESET_UNCORE        |
 * ---------------------------------------------- | ------------------- | ----------------
 * eards_begin_compute_turbo_freq     | START_GET_FREQ      | eard_freq
 * eards_end_compute_turbo_freq       | END_GET_FREQ        |
 * eards_begin_app_compute_turbo_freq | START_APP_COMP_FREQ |
 * eards_end_app_compute_turbo_freq   | END_APP_COMP_FREQ   |
 * ---------------------------------------------- | ------------------- | ----------------
 * eards_node_dc_energy               | READ_DC_ENERGY      | eard_node_energy
 *
 *
 * Files
 ********
 *            | File                          | Notes
 * ---------- | ----------------------------- | ----------------------------------------------------
 * Basics     | instructions.c                |
 * Cache      | cache.c                       |
 * Cache      | stalls.c                      |
 * Flops      | flops.c                       | Los weights se pueden descuadrar si falla un evento.
 * RAPL       | ear_rapl_metrics.c            | Las posiciones se pueden descuadrar si falla un evento.
 * IPMI       | ear_node_energy_metrics.c     | Tiene una función que devuelve el size, para que?
 * Bandwith   | ear_uncores.c                 | Per Integrated Memory Controller, PCI bus.
 * Avg. Freq. | ear_turbo.c (ear_frequency.c) | Per core, MSR registers. Hay que ver si acumula bien.
 *
 */

// Verbosity
static const char *__NAME__ = "ear/metrics";
extern char *__HOST__ ;

// Hardware
static double hw_cache_line_size;

// Options
static int papi_flops_supported;
static int bandwith_elements;
static int flops_elements;
static int rapl_elements;

// Registers
#define LOO 0 // loop
#define APP 1 // application

#define SIG_BEGIN 	0
#define SIG_END		1

static long long *metrics_flops[2]; // (vec)
static int *metrics_flops_weights; // (vec)
static ull *metrics_bandwith[2]; // ops (vec)
static ull *metrics_bandwith_init[2]; // ops (vec)
static ull *metrics_bandwith_end[2]; // ops (vec)
static ull *metrics_rapl[2]; // nJ (vec)
static ull *aux_rapl; // nJ (vec)
static ull *last_rapl; // nJ (vec)
static long long aux_time;
static ulong aux_energy;
static ulong metrics_ipmi[2]; // mJ
static ulong metrics_avg_frequency[2]; // MHz
static long long metrics_instructions[2];
static long long metrics_cycles[2];
static long long metrics_usecs[2]; // uS
static long long metrics_l1[2];
static long long metrics_l2[2];
static long long metrics_l3[2];

static int NI=0;

//TODO: remove when all metrics were unified
#define RAPL_DRAM0 			0
#define RAPL_DRAM1 			1
#define RAPL_PACKAGE0 		2
#define RAPL_PACKAGE1 		3

long long metrics_time()
{
	return PAPI_get_real_usec();
}

static void metrics_global_start()
{
	//
	eards_begin_app_compute_turbo_freq();
	// New
    eards_node_dc_energy(&aux_energy);
    aux_time = metrics_time();
    eards_read_rapl(aux_rapl);
	eards_read_uncore(metrics_bandwith_aux[APP]);

}

static void metrics_global_stop()
{
	//
	metrics_avg_frequency[APP] = eards_end_app_compute_turbo_freq();

	// Accum calls
	#if 0
	get_cache_metrics(&metrics_l1[APP], &metrics_l2[APP], &metrics_l3[APP]);
	#endif
	get_basic_metrics(&metrics_cycles[APP], &metrics_instructions[APP]);
	get_total_fops(metrics_flops[APP]);
}

/*           | Glob | Part || Start | Stop | Read | Get accum | Reset | Size
* ---------- | ---- | ---- || ----- | ---- | ---- | --------- | ----- | ----
* Instructs. | v    | v    || v     | v    | x    | v         | v     | x
* Cache      | v    | v    || v     | v    | x    | v         | v     | x
* Flops      | v    | v    || v     | v    | x    | v         | v     | x
* RAPL       | v    | v    || v     | v    | x    | x         | x     | v* (daemon)
* IPMI       | v    | v    || x     | x    | v    | x         | x     | v
* Bandwith   | v    | v    || v     | v    | v    | x         | v     | v
* Avg. Freq. | v    | v    || v     | v    | x    | x         | x     | x
*/

static void metrics_partial_start()
{
	#if 0
	eards_node_dc_energy(&metrics_ipmi[LOO]);
	metrics_usecs[LOO] = metrics_time();
	eards_read_rapl(aux_rapl);
	#endif
	int i;
	metrics_ipmi[LOO]=aux_energy;
	metrics_usecs[LOO]=aux_time;
	
	eards_begin_compute_turbo_freq();
	eards_start_uncore();
	for (i = 0; i < rapl_elements; i++) {
		last_rapl[i]=aux_rapl[i];
	}

	start_basic_metrics();
	#if 0
	start_cache_metrics();
	#endif
	start_flops_metrics();
}

static int metrics_partial_stop(uint where)
{
	long long aux_flops;
	int i;
	ulong c_energy;
	long long c_time;
	float c_power;

	// Manual IPMI accumulation
	eards_node_dc_energy(&aux_energy);
	if ((where==SIG_END) && (aux_energy==metrics_ipmi[LOO])){ 
	#if 0
		earl_verbose(1,"EAR_NOT_READY because of energy %u\n",aux_energy);
	#endif
		return EAR_NOT_READY;
	}
	aux_time = metrics_time();
	/* Sometimes energy is not zero but power is not correct */
	c_energy=aux_energy - metrics_ipmi[LOO];
	c_time=metrics_usecs_diff(aux_time, metrics_usecs[LOO]);
	/* energy is computed in mJ and time in usecs */
	c_power=(float)(c_energy*1000.0)/(float)c_time;

	if ((where==SIG_END) && (c_power<MIN_SIG_POWER)){ 
	#if 0
		earl_verbose(1,"EAR_NOT_READY because of power %f\n",c_power);
	#endif
		return EAR_NOT_READY;
	}

	metrics_ipmi[LOO] = c_energy;
	metrics_ipmi[APP] += metrics_ipmi[LOO];
	// Manual time accumulation
	metrics_usecs[LOO] = c_time;
	metrics_usecs[APP] += metrics_usecs[LOO];
	
	// Daemon metrics
	metrics_avg_frequency[LOO] = eards_end_compute_turbo_freq();
	/* This code needs to be adapted to read , compute the difference, and copy begin=end 
 	* diff_uncores(values,values_end,values_begin,num_counters);
 	* copy_uncores(values_begin,values_end,num_counters);
 	*/
	eards_read_uncore(metrics_bandwith[LOO]);
	eards_read_rapl(aux_rapl);

	// Manual bandwith accumulation
	for (i = 0; i < bandwith_elements; i++) {
			metrics_bandwith[APP][i] += metrics_bandwith[LOO][i];
	}
	// We read acuumulated energy
	for (i = 0; i < rapl_elements; i++) {
		if (aux_rapl[i] < last_rapl[i])
		{
			metrics_rapl[LOO][i] = ullong_diff_overflow(last_rapl[i], aux_rapl[i]);
		}
		else {
			metrics_rapl[LOO][i]=aux_rapl[i]-last_rapl[i];		
		}
	}

	// Manual RAPL accumulation
	for (i = 0; i < rapl_elements; i++) {
			metrics_rapl[APP][i] += metrics_rapl[LOO][i];
	}


	// Local metrics
	#if 0
	stop_cache_metrics(&metrics_l1[LOO], &metrics_l2[LOO], &metrics_l3[LOO]);
	#endif
	stop_basic_metrics(&metrics_cycles[LOO], &metrics_instructions[LOO]);
	stop_flops_metrics(&aux_flops, metrics_flops[LOO]);

	return EAR_SUCCESS;
}

static void metrics_reset()
{
	//eards_reset_uncore();
	//eards_reset_rapl();

	reset_basic_metrics();
	reset_flops_metrics();
	#if 0
	reset_cache_metrics();
	#endif
}

static void metrics_compute_signature_data(uint global, signature_t *metrics, uint iterations, ulong procs)
{
	double time_s, cas_counter, aux;
	int i, s;

	// If global is 1, it means that the global application metrics are required
	// instead the small time metrics for loops. 's' is just a signature index.
	s = global;

	// Time
	time_s = (double) metrics_usecs[s] / 1000000.0;

	// Basics
	metrics->time = time_s / (double) iterations;
	metrics->avg_f = metrics_avg_frequency[s];

	#if 0
	metrics->L1_misses = metrics_l1[s];
	metrics->L2_misses = metrics_l2[s];
	metrics->L3_misses = metrics_l3[s];
	#endif

	// FLOPS
	if (papi_flops_supported)
	{
		metrics->Gflops = 0.0;

		for (i = 0; i < flops_elements; i++) {
			metrics->FLOPS[i] = metrics_flops[s][i] * metrics_flops_weights[i];
			metrics->Gflops += (double) metrics->FLOPS[i];
		}

		metrics->Gflops = metrics->Gflops / time_s; // Floating ops to FLOPS
		metrics->Gflops = metrics->Gflops / 1000000000.0; // FLOPS to GFLOPS
		metrics->Gflops = metrics->Gflops * (double) procs; // Core GFLOPS to node GFLOPS
	}

	// Transactions and cycles
	aux = time_s * (double) (1024 * 1024 * 1024);
	cas_counter = 0.0;

	for (i = 0; i < bandwith_elements; ++i) {
		cas_counter += (double) metrics_bandwith[s][i];
	}

	// Cycles, instructions and transactions
	metrics->cycles = metrics_cycles[s];
	metrics->instructions = metrics_instructions[s];

	metrics->GBS = cas_counter * hw_cache_line_size / aux;
	metrics->CPI = (double) metrics_cycles[s] / (double) metrics_instructions[s];
	metrics->TPI = cas_counter * hw_cache_line_size / (double) metrics_instructions[s];

	// Energy IPMI
	metrics->DC_power = (double) metrics_ipmi[s] / (time_s * 1000.0);
	metrics->EDP = time_s * time_s * metrics->DC_power;

	// Energy RAPL (TODO: ask for the two individual energy types separately)
	metrics->PCK_power   = (double) metrics_rapl[s][RAPL_PACKAGE0];
	metrics->PCK_power  += (double) metrics_rapl[s][RAPL_PACKAGE1];
	metrics->PCK_power   = (metrics->PCK_power / 1000000000.0) / time_s;
	metrics->DRAM_power  = (double) metrics_rapl[s][RAPL_DRAM0];
	metrics->DRAM_power += (double) metrics_rapl[s][RAPL_DRAM1];
	metrics->DRAM_power  = (metrics->DRAM_power / 1000000000.0) / time_s;
}

int metrics_init()
{
	ulong flops_size;
	ulong bandwith_size;
	ulong rapl_size;

	// Cache line (using custom hardware scanning)
	hw_cache_line_size = (double) get_cache_line_size();
	DEBUG_F(0, "detected cache line has a size %0.2lf bytes", hw_cache_line_size);

	// Local metrics initialization
	init_basic_metrics();
	#if 0
	init_cache_metrics();
	#endif
	papi_flops_supported = init_flops_metrics();

	if (papi_flops_supported)
	{
		// Fops size and elements
		flops_elements = get_number_fops_events();
		flops_size = sizeof(long long) * flops_elements;

		// Fops metrics allocation
		metrics_flops[APP] = (long long *) malloc(flops_size);
		metrics_flops[LOO] = (long long *) malloc(flops_size);
		metrics_flops_weights = (int *) malloc(flops_size);

		if (metrics_flops[LOO] == NULL || metrics_flops[APP] == NULL)
		{
			VERBOSE_N(0,"error allocating memory, exiting");
			exit(1);
		}

		get_weigth_fops_instructions(metrics_flops_weights);

		DEBUG_F(0, "detected %d FLOP counter", flops_elements);
	}

	// Daemon metrics allocation (TODO: standarize data size)
	rapl_size = eards_get_data_size_rapl();
	rapl_elements = rapl_size / sizeof(long long);

	bandwith_size = eards_get_data_size_uncore();
	bandwith_elements = bandwith_size / sizeof(long long);

	metrics_bandwith[LOO] = malloc(bandwith_size);
	metrics_bandwith[APP] = malloc(bandwith_size);
	metrics_bandwith_init[LOO] = malloc(bandwith_size);
	metrics_bandwith_init[APP] = malloc(bandwith_size);
	metrics_bandwith_end[LOO] = malloc(bandwith_size);
	metrics_bandwith_end[APP] = malloc(bandwith_size);
	metrics_rapl[LOO] = malloc(rapl_size);
	metrics_rapl[APP] = malloc(rapl_size);
	aux_rapl = malloc(rapl_size);
	last_rapl = malloc(rapl_size);


	if (metrics_bandwith[LOO] == NULL || metrics_bandwith[APP] == NULL || metrics_bandwith_init[LOO] == NULL || metrics_bandwith_init[APP] == NULL ||
		metrics_bandwith_end[LOO] == NULL || metrics_bandwith_end[APP] == NULL  ||
			metrics_rapl[LOO] == NULL || metrics_rapl[APP] == NULL || aux_rapl == NULL || last_rapl == NULL)
	{
			VERBOSE_N(0, "error allocating memory in metrics, exiting");
			exit(1);
	}
	memset(metrics_bandwith[LOO], 0, bandwith_size);
	memset(metrics_bandwith[APP], 0, bandwith_size);
	memset(metrics_bandwith_init[LOO], 0, bandwith_size);
	memset(metrics_bandwith_init[APP], 0, bandwith_size);
	memset(metrics_bandwith_end[LOO], 0, bandwith_size);
	memset(metrics_bandwith_end[APP], 0, bandwith_size);
	memset(metrics_rapl[LOO], 0, rapl_size);
	memset(metrics_rapl[APP], 0, rapl_size);
	memset(aux_rapl, 0, rapl_size);
	memset(last_rapl, 0, rapl_size);

	DEBUG_F(0, "detected %d RAPL counter", rapl_elements);
	DEBUG_F(0, "detected %d bandwith counter", bandwith_elements);

	metrics_reset();
	metrics_global_start();
	metrics_partial_start();

	return EAR_SUCCESS;
}

void metrics_dispose(signature_t *metrics, ulong procs)
{
	metrics_partial_stop(SIG_BEGIN);
	metrics_global_stop();

	metrics_compute_signature_data(APP, metrics, 1, procs);
}

void metrics_compute_signature_begin()
{
	//
	metrics_partial_stop(SIG_BEGIN);
	metrics_reset();

	//
	metrics_partial_start();
}

int metrics_compute_signature_finish(signature_t *metrics, uint iterations, ulong min_time_us, ulong procs)
{
    long long aux_time;

	NI=iterations;

	// Time requirements
	aux_time = metrics_usecs_diff(metrics_time(), metrics_usecs[LOO]);

	//
	if ((aux_time<min_time_us) && !equal_with_th((double)aux_time, (double)min_time_us,0.1)) {
        #if 0
                earl_verbose(1,"EAR_NOT_READY because of time %llu\n",aux_time);
        #endif
		metrics->time=0;

		return EAR_NOT_READY;
	}

	//
	if (metrics_partial_stop(SIG_END)==EAR_NOT_READY) return EAR_NOT_READY;
	metrics_reset();

	//
	metrics_compute_signature_data(LOO, metrics, iterations, procs);

	//
	metrics_partial_start();

	return EAR_SUCCESS;
}

long long metrics_usecs_diff(long long end, long long init)
{
	long long to_max;

	if (end < init)
	{
		DEBUG_F(0, "Timer overflow (end: %ll - init: %ll)\n", end, init);
		to_max = LLONG_MAX - init;
		return (to_max + end);
	}

	return (end - init);
}
