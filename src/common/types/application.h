/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.

    Copyright (C) 2017
        BSC Contact Julita Corbalan (julita.corbalan@bsc.es)
        Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/
#ifndef _EAR_TYPES_APPLICATION
#define _EAR_TYPES_APPLICATION

#include <common/types/generic.h>

#define GENERIC_NAME	256
#define POLICY_NAME		32
#define FLOPS_EVENTS	8

typedef struct application
{
	char node_id[GENERIC_NAME];
	char user_id[GENERIC_NAME];
	char app_id[GENERIC_NAME];
	char job_id[GENERIC_NAME];
	char policy[POLICY_NAME];
	double DC_power;
	double DRAM_power;
	double PCK_power;
	double EDP; // not printed
	double GBS;
	double TPI;
	double CPI;
	double policy_th;
	double Gflops;
	double time;
	unsigned long long FLOPS[FLOPS_EVENTS];
	unsigned long long L1_misses;
	unsigned long long L2_misses;
	unsigned long long L3_misses;
	unsigned long long instructions;
	unsigned long long cycles;
	unsigned int avg_f; // MHz
	unsigned int def_f; // MHz
	unsigned int procs; // not printed
	unsigned int empty; // 64 bit alignment
} application_t;

typedef struct loop {
	application_t signature;
	ulong first_event;
	uint iterations;
	uint level;
	uint size;
} loop_t;

// Function declarations

/** Replicates the application in *source to *destiny */
void copy_application(application_t *destiny, application_t *source);

/** Appends the information of the application signature to the
*	file given by parameter */
int append_loop_text_file(char *path, loop_t *loop);

/** Initializes the application*//
void init_application(application_t *app);

/** Outputs the application_t's data given by parameter to the file defined
*   in by STDOUT_FILENO (default is stdout) */
int print_application(application_t *app);

/** Outputs the application summary to stdout */
void report_application_data(application_t *app);

/** Reads a file of applications saved in binary format. A memory block is
*	allocated for the read applications, and is returned by the argument 'apps'.
*	The returned integer is the number of applications read. If the integer is
*	negative, one of the following errores ocurred: EAR_ALLOC_ERROR,
*	EAR_READ_ERROR or EAR_FILE_NOT_FOUND. */
int read_application_binary_file(char *path, application_t **apps);

/** Reads a file of applications saved in CSV format. A block of memory is
*	allocated for this read applications, and is returned by the argument 'apps'.
*	The returned integer is the number of applications read. If the integer is
*	negative, one of the following errores ocurred: EAR_ALLOC_ERROR,
*	EAR_READ_ERROR or EAR_FILE_NOT_FOUND. */
int read_application_text_file(char *path, application_t **apps);

/** Appends in a file an application in binary format. The returned integer is
*	one of the following states: EAR_SUCCESS or EAR_ERROR. */
int append_application_binary_file(char *path, application_t *app);

/** Appends in a file an application in CSV format. The returned integer is one
*	of the following states: EAR_SUCCESS or EAR_ERROR. */
int append_application_text_file(char *path, application_t *app);

#endif
