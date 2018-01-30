#ifndef _EAR_TYPES
#define _EAR_TYPES

typedef unsigned char uchar;
typedef unsigned long long ull;
typedef unsigned long ulong;
typedef unsigned int uint;

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
    // App identification: job;node;user;app
    char job_id[MAX_APP_NAME];
    char node_id[MAX_APP_NAME];
    char user_id[MAX_APP_NAME];
    char app_id[MAX_APP_NAME];
    char policy[MAX_APP_NAME];
    double th;
    unsigned long f;
    unsigned int procs;
    double seconds;
    double GIPS_f0;
    double GBS_f0;
    double POWER_f0;
    double TPI_f0;
    double CPI;
    unsigned int nominal;
    // Extended data
    long long CYCLES;
    long long INSTRUCTIONS;
    double POWER_DC;
    double DRAM_POWER;
    double PCK_POWER;
    double Gflops;
    double EDP;
} application_t;

typedef struct Coefficients_info
{
    unsigned long pstate;
    unsigned int available;
    // For power projection
    double A;
    double B;
    double C;
    // For CPI projection
    double D;
    double E;
    double F;
} coefficient_t;

typedef struct PerfProjection
{
    double Time;
    double Power;
    double CPI;
} projection_t;

// Declarations
int read_summary_file(char *path, application_t **apps);
int read_coefficients_file(char *path, coefficient_t **coeffs, int size);
int append_application_binary_file(char *path, application_t *app);
int append_application_text_file(char *path, application_t *app);

#endif
