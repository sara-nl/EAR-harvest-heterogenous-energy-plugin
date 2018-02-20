/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <papi.h>

#include <ear_metrics/ear_metrics.h>
#include <ear_daemon_client.h>
#include <ear_verbose.h>

#include <types/application.h>
#include <metrics/papi/instructions.h>
#include <metrics/papi/generics.h>
#include <metrics/papi/cache.h>
#include <metrics/papi/flops.h>
#include <states.h>

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
 * ear_daemon_client_get_data_size_rapl           | DATA_SIZE_RAPL      | eard_rapl
 * ear_daemon_client_start_rapl                   | START_RAPL          |
 * ear_daemon_client_read_rapl                    | READ_RAPL           |
 * ear_daemon_client_reset_rapl                   | RESET_RAPL          |
 * ---------------------------------------------- | ------------------- | ----------------
 * ear_daemon_client_get_data_size_uncore         | DATA_SIZE_UNCORE    | eard_uncore
 * ear_daemon_client_start_uncore                 | START_UNCORE        |
 * ear_daemon_client_read_uncore                  | READ_UNCORE         |
 * ear_daemon_client_reset_uncore                 | RESET_UNCORE        |
 * ---------------------------------------------- | ------------------- | ----------------
 * ear_daemon_client_begin_compute_turbo_freq     | START_GET_FREQ      | eard_freq
 * ear_daemon_client_end_compute_turbo_freq       | END_GET_FREQ        |
 * ear_daemon_client_begin_app_compute_turbo_freq | START_APP_COMP_FREQ |
 * ear_daemon_client_end_app_compute_turbo_freq   | END_APP_COMP_FREQ   |
 * ---------------------------------------------- | ------------------- | ----------------
 * ear_daemon_client_node_dc_energy               | READ_DC_ENERGY      | eard_node_energy
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
// Hardware
static const PAPI_hw_info_t *hw_general = NULL;
static double hw_cache_line_size;
static int hw_node_size;

// Options
static int papi_flops_supported;
static int bandwith_elements;
static int flops_elements;
static int rapl_elements;

// Registers
#define LOO 0 // loop
#define APP 1 // application

static long long *metrics_flops[2]; // (vec)
static int *metrics_flops_weights; // (vec)
static ull *metrics_bandwith[2]; // ops (vec)
static ull *metrics_rapl[2]; // nJ (vec)
static ulong metrics_ipmi[2]; // mJ
static ulong metrics_avg_frequency[2]; // MHz
static long long metrics_instructions[2];
static long long metrics_cycles[2];
static long long metrics_usecs[2]; // uS
static long long metrics_l1[2];
static long long metrics_l2[2];
static long long metrics_l3[2];

//TODO: remove when all metrics were unified
#define RAPL_DRAM0 		0
#define RAPL_DRAM1 		1
#define RAPL_PACKAGE0 		2
#define RAPL_PACKAGE1 		3

long long metrics_time()
{
	return PAPI_get_real_usec();
}

static void metrics_global_start()
{
	//
	ear_daemon_client_begin_app_compute_turbo_freq();
}

static void metrics_global_stop()
{
	//
	metrics_avg_frequency[APP] = ear_daemon_client_end_app_compute_turbo_freq();

	// Accum calls
	get_cache_metrics(&metrics_l1[APP], &metrics_l2[APP], &metrics_l3[APP]);
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
	metrics_usecs[LOO] = metrics_usecs[APP];
	metrics_ipmi[LOO] = metrics_ipmi[APP];

	ear_daemon_client_begin_compute_turbo_freq();
	ear_daemon_client_start_uncore();
	ear_daemon_client_start_rapl();

	start_basic_metrics();
	start_cache_metrics();
	start_flops_metrics();
}

static void metrics_partial_stop()
{
	long long aux_time, aux_flops;
	ulong aux_energy;
	int i;

	metrics_avg_frequency[LOO] = ear_daemon_client_end_compute_turbo_freq();
	ear_daemon_client_read_uncore(metrics_bandwith[LOO]);
	ear_daemon_client_read_rapl(metrics_rapl[LOO]);

	stop_cache_metrics(&metrics_l1[LOO], &metrics_l2[LOO], &metrics_l3[LOO]);
	stop_basic_metrics(&metrics_cycles[LOO], &metrics_instructions[LOO]);
	stop_flops_metrics(&aux_flops, metrics_flops[LOO]);

	// Manual bandwith accumulation
	for (i = 0; i < bandwith_elements; i++) {
		metrics_bandwith[APP][i] += metrics_bandwith[LOO][i];
	}

	// Manual RAPL accumulation
	for (i = 0; i < rapl_elements; i++) {
		metrics_rapl[APP][i] += metrics_rapl[LOO][i];
	}

	// Manual IPMI accumulation
	ear_daemon_client_node_dc_energy(&aux_energy);
	metrics_ipmi[LOO] = aux_energy - metrics_ipmi[LOO];
	metrics_ipmi[APP] += metrics_ipmi[LOO];

	// Manual time accumulation
	aux_time = metrics_time();
	metrics_usecs[LOO] = metrics_usecs_diff(aux_time, metrics_usecs[LOO]);
	metrics_usecs[APP] += metrics_usecs[LOO];
}

static void metrics_reset()
{
	ear_daemon_client_reset_uncore();
	ear_daemon_client_reset_rapl();

	reset_basic_metrics();
	reset_flops_metrics();
	reset_cache_metrics();
}

static void metrics_compute_signature_data(uint global, application_t *metrics, uint iterations)
{
	double time_s, cas_counter, aux;
	int i, s;

	// If global is 1, it means that the global application metrics are required
	// instead the small time metrics for loops. 's' is just a signature index.
	s = global;

	time_s = (double) metrics_usecs[s] / 1000000.0;

	// Transactions and cycles
	aux = time_s * (double) (1024 * 1024 * 1024);
	cas_counter = 0.0;

	for (i = 0; i < bandwith_elements; ++i) {
		cas_counter += (double) metrics_bandwith[s][i];
	}

	metrics->GBS += cas_counter * hw_cache_line_size / aux;
	metrics->CPI  = (double) metrics_cycles[s] / (double) metrics_instructions[s];
	metrics->TPI  = cas_counter * hw_cache_line_size / (double) metrics_instructions[s];

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

	// Basics
	metrics->time = time_s / (double) iterations;
	metrics->instructions = metrics_instructions[s];
	metrics->cycles = metrics_cycles[s];

	metrics->L1_misses = metrics_l1[s];
	metrics->L2_misses = metrics_l2[s];
	metrics->L3_misses = metrics_l3[s];

	// FLOPS
	for (i = 0; i < flops_elements; i++) {
		metrics->FLOPS[i] = metrics_flops[s][i] * metrics_flops_weights[i];
		metrics->Gflops += (double) metrics->FLOPS[i];
	}

	metrics->Gflops = metrics->Gflops / 1000000000.0;
}

int metrics_init(int my_id)
{
	ulong flops_size;
	ulong bandwith_size;
	ulong rapl_size;

	if (my_id) return 1;

	PAPI_INIT_TEST(__FILE__);
	PAPI_INIT_MULTIPLEX_TEST(__FILE__);

	// General hardware info by PAPI
	hw_general = PAPI_get_hardware_info();

	if (hw_general != NULL) {
		hw_node_size = hw_general->threads * hw_general->cores;
	}

	// Cache line (using custom hardware scanning)
	hw_cache_line_size = (double) get_cache_line_size();
	ear_debug(2, "(%s): cache line size is %0.2lf bytes\n", __FILE__, hw_cache_line_size);
	DEBUG_F(0, "detected cache line has a size %0.2lf bytes", hw_cache_line_size);

	// Accessable metrics
	init_basic_metrics();
	init_cache_metrics();
	papi_flops_supported = init_flops_metrics();

	// Through daemon metrics (TODO: standarize data size)
	rapl_size = ear_daemon_client_get_data_size_rapl();
	bandwith_size = ear_daemon_client_get_data_size_uncore();
	flops_elements = get_number_fops_events();

	// Allocation
	rapl_elements = rapl_size / sizeof(long long);
	bandwith_elements = bandwith_size / sizeof(long long);
	flops_size = sizeof(long long) * flops_elements;

	DEBUG_F(0, "detected %d RAPL counter", rapl_elements);
	DEBUG_F(0, "detected %d FLOP counter", flops_elements);
	DEBUG_F(0, "detected %d bandwith counter", bandwith_elements);

	metrics_bandwith[LOO] = malloc(bandwith_size);
	metrics_bandwith[APP] = malloc(bandwith_size);
	metrics_rapl[LOO] = malloc(rapl_size);
	metrics_rapl[APP] = malloc(rapl_size);

	if (metrics_bandwith[LOO] == NULL || metrics_bandwith[APP] == NULL ||
		metrics_rapl[LOO] == NULL || metrics_rapl[APP] == NULL)
	{
		ear_verbose(0,"EAR: Error allocating memory in %s metrics\n", __FILE__);
		exit(1);
	}

	if (papi_flops_supported)
	{
		metrics_flops[APP] = (long long *) malloc(flops_size);
		metrics_flops[LOO] = (long long *) malloc(flops_size);
		metrics_flops_weights = (int *) malloc(flops_size);

		if (metrics_flops[LOO] == NULL || metrics_flops[APP] == NULL)
		{
			ear_verbose(0,"EAR: Error allocating memory in %s metrics\n", __FILE__);
			exit(1);
		}

		get_weigth_fops_instructions(metrics_flops_weights);
	}

	metrics_global_start();
	metrics_partial_start();

	return EAR_SUCCESS;
}

void metrics_dispose(application_t *metrics)
{
	metrics_partial_stop();
	metrics_global_stop();

	metrics_compute_signature_data(APP, metrics, 1);
}

void metrics_compute_signature_begin()
{
	metrics_usecs[LOO] = metrics_time();

	//
	metrics_partial_stop();
	metrics_reset();

	//
	metrics_partial_start();
}

int metrics_compute_signature_finish(application_t *metrics, uint iterations, ulong min_time_us)
{
	//
	metrics_partial_stop();
	metrics_reset();

	//
	if (metrics_usecs[LOO] < min_time_us) {
		return EAR_NOT_READY;
	}

	//
	metrics_compute_signature_data(LOO, metrics, iterations);

	//
	metrics_partial_start();

	return EAR_SUCCESS;
}

/*
 *
 *
 *
 * LEGACY CODE
 *
 *
 *
 *
 */

long long metrics_usecs_diff(long long end, long long init)
{
	long long to_max;

	//LLONG_MAX
	if (end < init)
	{
		ear_debug(0, "EAR(%s) UsecsDiff TIME END < INIT %ll - %ll\n", __FILE__, end, init);
		to_max = LLONG_MAX - init;
		return (to_max + end);
	}

	return (end-init);
}

void metrics_get_hw_info(int *sockets, int *cores_socket, ulong *max_f, ulong *min_f, ulong *nom_f,
						 char *CPU_model, char *CPU_name)
{
	*sockets = hw_general->sockets;
	*cores_socket = hw_general->cores;
	*max_f = hw_general->cpu_max_mhz;
	*min_f = hw_general->cpu_min_mhz;
	*nom_f = ear_get_nominal_frequency();
	strcpy(CPU_model,hw_general->model_string);
	strcpy(CPU_name,hw_general->vendor_string);
}
