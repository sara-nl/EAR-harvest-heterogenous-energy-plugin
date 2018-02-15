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

#include <ear_daemon_client.h>
#include <ear_metrics/ear_basic.h>
#include <ear_metrics/ear_cache.h>
#include <ear_metrics/ear_metrics.h>
#include <ear_metrics/ear_flops_metrics.h>
#include <ear_metrics/papi_generics.h>
#include <types/application.h>
#include <states.h>

/*
 * Low level reading
 ********************
 *            | P?   | D | Type              | Size F.| Accum/Int. | Unit | Notes
 * ---------- | ---- | - | ----------------- | ------ | ---------- | ---- | ----------------------------
 * Basics     | PAPI | x | * long long (x2)  | No     | Yes/Yes    | num  | No read (just accum)
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
 * Basics     | ear_basic.c                   |
 * Cache      | ear_cache.c                   |
 * Cache      | ear_stalls.c                  |
 * Flops      | ear_flops_metrics.c           | Los weights se pueden descuadrar si falla un evento.
 * RAPL       | ear_rapl_metrics.c            |
 * IPMI       | ear_node_energy_metrics.c     | Tiene una función que devuelve el size, para que?
 * Bandwith   | ear_uncores.c                 | Per Integrated Memory Controller, PCI bus.
 * Avg. Freq. | ear_turbo.c (ear_frequency.c) | Per core, MSR registers. Hay que ver si acumula bien.
 *
 */
// Hardware
static const PAPI_hw_info_t *hw_general = NULL;
static int hw_cache_line_size;
static int hw_node_size;

// Options
static int bandwith_elements;
static int flops_elements;
static int rapl_elements;

// Metrics
static long long *global_flops; // (vec)
static long long *partial_flops; // (vec)
static ull *global_bandwith; // ops (vec)
static ull *partial_bandwith; // ops (vec)
static ull *global_rapl; // nJ (vec)
static ull *partial_rapl; // nJ (vec)
static ulong global_ipmi; // mJ
static ulong partial_ipmi; // mJ
static ulong global_avg_frequency; // MHz
static ulong partial_avg_frequency; // MHz
static long long global_instructions;
static long long partial_instructions;
static long long global_cycles;
static long long partial_cycles;
static long long partial_l1;
static long long partial_l2;
static long long partial_l3;
static long long global_l1;
static long long global_l2;
static long long global_l3;
static long long global_time; // uS
static long long partial_time; // uS

// TODO: remove, dummy
application_t dummy;

int metrics_init(int my_id)
{
	ulong flops_size;
	ulong uncore_size;
	ulong rapl_size;

	init_application(&dummy);

	if (my_id) return 1;

	PAPI_INIT_TEST(__FILE__);

	// General hardware info by PAPI
	hw_general = PAPI_get_hardware_info();

	if (hw_general != NULL) {
		hw_node_size = hw_general->threads * hw_general->cores;
	}

	// Cache line (using custom hardware scanning)
	hw_cache_line_size = get_cache_line_size();
	ear_debug(2, "(%s): cache line size is %d bytes\n", __FILE__, hw_cache_line_size);

	// Accessable metrics
	init_basic_metrics();
	init_cache_metrics();
	papi_flops_supported = init_flops_metrics();

	// Through daemon metrics
	uncore_size = ear_daemon_client_get_data_size_uncore();
	rapl_size = ear_daemon_client_get_data_size_rapl();
	flops_elements = get_number_fops_events();

	// Allocation
	uncore_elements = uncore_size / sizeof(long long);
	rapl_elements = rapl_size / sizeof(long long);
	flops_size = sizeof(long long) * flops_elements;

	partial_bandwith = malloc(uncore_size);
	global_bandwith = malloc(uncore_size);
	partial_rapl = malloc(rapl_size);
	global_rapl = malloc(rapl_size);

	if (partial_bandwith == NULL || global_bandwith == NULL ||
		partial_rapl == NULL || global_rapl == NULL)
	{
		ear_verbose(0,"EAR: Error allocating memory in %s metrics\n", __FILE__);
		exit(1);
	}

	if (papi_flops_supported)
	{
		global_flops = (long long *) malloc(flops_size);
		partial_flops = (long long *) malloc(flops_size);

		if (partial_flops == NULL || global_flops == NULL)
		{
			ear_verbose(0,"EAR: Error allocating memory in %s metrics\n", __FILE__);
			exit(1);
		}
	}

	metrics_global_start();
	metrics_partial_start();

	return EAR_SUCCESS;
}

void metrics_dispose(application_t *metrics)
{
	metrics_partial_stop();
	metrics_global_stop();

	metrics = &dummy;
}

static void metrics_global_start()
{
	//
	ear_daemon_client_begin_app_compute_turbo_freq();
}

static void metrics_global_stop()
{
	//
	global_avg_frequency = ear_daemon_client_end_app_compute_turbo_freq();

	// Accum calls
	get_cache_metrics(&global_l1, &global_l2, &global_l3);
	get_basic_metrics(&global_cycles, &global_instructions);
	get_total_fops(&temp_total_flops, metrics_flops);
}

/*           | Glob | Part || Start | Stop | Read | Get accum | Reset | Size
* ---------- | ---- | ---- || ----- | ---- | ---- | --------- | ----- | ----
* Basics     | v    | v    || v     | v    | x    | v         | v     | x
* Cache      | v    | v    || v     | v    | x    | v         | v     | x
* Flops      | v    | v    || v     | v    | x    | v         | v     | x
* RAPL       | v    | v    || v     | v    | x    | x         | x     | v* (daemon)
* IPMI       | v    | v    || x     | x    | v    | x         | x     | v
* Bandwith   | v    | v    || v     | v    | v    | x         | v     | v
* Avg. Freq. | v    | v    || v     | v    | x    | x         | x     | x
*/

static void metrics_partial_start()
{
	partial_time = global_time;
	partial_ipmi = global_ipmi;

	ear_daemon_client_begin_compute_turbo_freq();
	ear_daemon_client_start_uncore();
	ear_daemon_client_start_rapl();

	start_basic_metrics();
	start_cache_metrics();
	start_flops_metrics();
}

static void metrics_partial_stop()
{
	long long aux_time;
	ulong aux_energy;
	int i;

	partial_avg_frequency = ear_daemon_client_end_compute_turbo_freq();
	ear_daemon_client_read_uncore(metrics_bandwith);
	ear_daemon_client_read_rapl(metrics_rapl);

	stop_cache_metrics(&partial_l1, &partial_l2, &partial_l3);
	stop_basic_metrics(&partial_cycles, &partial_instructions);
	stop_flops_metrics(&temp_total_flops, metrics_flops);

	// Manual bandwith accumulation
	for (i = 0; i < bandwith_elements; i++) {
		global_bandwith[i] += partial_bandwith[i];
	}

	// Manual RAPL accumulation
	for (i = 0; i < rapl_elements; i++) {
		global_rapl[i] += partial_rapl[i];
	}

	// Manual IPMI accumulation
	ear_daemon_client_node_dc_energy(&aux_energy);
	partial_ipmi = aux_energy - partial_ipmi;
	global_ipmi += partial_ipmi;

	// Manual time accumulation
	aux_time = metrics_time();
	partial_time = metrics_usecs_diff(aux_time, partial_time);
	global_time += metrics_time;
}

static void metrics_reset()
{
	ear_daemon_client_reset_uncore();
	ear_daemon_client_reset_rapl();

	reset_basic_metrics();
	reset_flops_metrics();
	reset_cache_metrics();
}

void metrics_compute_signature_begin()
{
	partial_time = metrics_time();

	//
	metrics_partial_stop();
	metrics_reset();

	//
	metrics_global_start();
}

static void metrics_compute_signature_data(uint iterations)
{
	printf("-------------------------------\n");
	printf("Partial time: %ll\n", partial_time);
	printf("Partial iterations: %ll\n", iterations);
	printf("-------------------------------\n");
}

int metrics_compute_signature_finish(application_t *metrics, uint iterations, ulong min_time_us)
{
	//
	metrics_partial_stop();
	metrics_reset();

	//
	if (partial_time < min_time_us) {
		return EAR_NOT_READY;
	}

	//
	// THINGS
	metrics_compute_signature_data(iterations);
	metrics = NULL;

	//
	metrics_global_start();

	return EAR_NOT_READY;
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
void metrics_get_app_name(char *app_name)
{
	const PAPI_exe_info_t *prginfo = NULL;

	PAPI_INIT_TEST(__FILE__);

	if ((prginfo = PAPI_get_executable_info()) == NULL)
	{
		ear_verbose(0,"EAR(%s): Executable info not available. Exiting\n", __FILE__);
		exit(2);
	}

	strcpy(app_name,prginfo->fullname);
}

long long  metrics_time()
{
	return PAPI_get_real_usec();
}

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