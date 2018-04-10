#ifndef _EAR_TYPES_JOB
#define _EAR_TYPES_JOB
#include <stdint.h>
#include <time.h>
#include <common/types/generic.h>

typedef uint8_t	job_type;
typedef ulong	job_id;

#define GENERIC_NAME 256
#define POLICY_NAME 32

typedef struct job
{
	job_id 	id;
	char 	user_id[GENERIC_NAME];
	char 	app_id[GENERIC_NAME];
	time_t 	start_time;
	time_t	end_time;
	time_t 	start_mpi_time;
	time_t	end_mpi_time;
	char 	policy[POLICY_NAME];
	double  th;
	ulong 	procs; 
	job_type	type;	
	ulong 		def_f;
} job_t;



// Function declarations

// MANAGEMENT


// Must be called just once. memory is allocated if needed. values automatically initialized are (job_id, user_id, start_time,end_time,type). 
void init_job(job_t *job, ulong def_f, char *policy, double th, ulong procs);

ulong start_mpi(job_t *job);
ulong end_mpi(job_t *job);

//
void copy_job(job_t *destiny, job_t *source);
// ?? Really needed
void end_job(job_t *job);

void print_job_fd(int fd, job_t *job);

#endif
