#ifndef _EAR_TYPES_APPLICATION
#define _EAR_TYPES_APPLICATION

#include <types_generic.h>

#define MAX_APP_NAME    1024
#define FLOPS_EVENTS    8

typedef struct App_info_extended
{
	long long L1_misses;
	long long L2_misses;
	long long L3_misses;
	long long FLOPS[FLOPS_EVENTS];
} application_ext_t;

typedef struct App_info
{
    char job_id[MAX_APP_NAME];
    char node_id[MAX_APP_NAME];
    char user_id[MAX_APP_NAME];
    char app_id[MAX_APP_NAME];
    char policy[MAX_APP_NAME];
    double policy_th; // th
    unsigned long avg_f; // f
    unsigned int procs;
    double iter_time; // seconds
    double disabled_2; // GIPS_f0
    double GBS; // GBS_f0
    double DC_power; // POWER_f0
    double TPI; // TPI_f0
    double CPI;
    unsigned int def_f; // nominal
    unsigned long long cycles;
    unsigned long long instructions;
    double disabled_1; // POWER_DC
    double DRAM_power; // DRAM_POWER
    double PCK_power; // PCK_POWER
    double Gflops;
    double EDP;
} application_t;


/*#define GENERIC_NAME	256
#define POLICY_NAME		32
#define FLOPS_EVENTS    16

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
	double EDP;
	double GBS;
	double TPI;
	double CPI;
	double policy_th;
	double Gflops;
	double time;
	double dyn1;
	double dyn2;
	unsigned long long FLOPS[FLOPS_EVENTS];
	unsigned long long L1_misses;
	unsigned long long L2_misses;
	unsigned long long L3_misses;
	unsigned long long instructions;
	unsigned long long cycles;
	unsigned int avg_f;
	unsigned int def_f;
	unsigned int procs;
	unsigned int dyn4; // 64 bit alignment
	long long dyn5;
	long long dyn6;
	void ext1;
	void ext2;
	void ext3;
	void ext4;
	void ext5;
	void ext6;
} application_t;*/

// Declarations
int read_application_text_file(char *path, application_t **apps);
int append_application_binary_file(char *path, application_t *app);
int append_application_text_file(char *path, application_t *app);

#endif
