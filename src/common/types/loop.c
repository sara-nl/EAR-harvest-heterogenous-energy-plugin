#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "loop.h"
#include "states.h"


#define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OPTIONS O_WRONLY | O_CREAT | O_TRUNC | O_APPEND


loop_id_t *create_loop_id(ulong event, ulong size, ulong level)
{
    loop_id_t *loop_id = malloc(sizeof(loop_id_t));
    loop_id->event = event;
    loop_id->size = size;
    loop_id->level = level;

    return loop_id;
}

loop_t *create_loop(loop_id_t loop_id)
{
    loop_t *loop = calloc(1, sizeof(loop));
    loop->id = loop_id;
    return loop;
}

void loop_init(loop_t *loop)
{
    memset(loop, 0, sizeof(loop_t));
}


void add_loop_signature(loop_t *loop,  signature_t *sig)
{
    loop->signatures = *sig;
}

void end_loop(loop_t *loop, ulong iterations)
{
    loop->total_iterations = iterations;
}

void copy_loop(loop_t *destiny, loop_t *source)
{
    memcpy(destiny, source, sizeof(loop_t));
}

void print_loop_id_fd(int fd, loop_id_t *loop_id)
{
    dprintf(fd, "%lu;%lu;%lu;", loop_id->event, loop_id->level, loop_id->size);
}

void print_loop_fd(int fd, loop_t *loop)
{
    //tbd
}

int append_loop_text_file(char *path, loop_t *loop)
{
    if (path == NULL) {
        return EAR_ERROR;
    }

    char *HEADER = "NODENAME;USERNAME;JOB_ID;APPNAME;POLICY;POLICY_TH;AVG.FREQ;DEF.FREQ;" \
        "TIME;CPI;TPI;GBS;DC-NODE-POWER;DRAM-POWER;PCK-POWER;CYCLES;INSTRUCTIONS;L1_MISSES;" \
        "L2_MISSES;L3_MISSES;GFLOPS;SP_SINGLE;SP_128;SP_256;SP_512;DP_SINGLE;DP_128;DP_256;" \
        "DP_512;FIRST_EVENT;LEVEL;SIZE;ITERATIONS";
	
    int fd, ret;

    fd = open(path, O_WRONLY | O_APPEND);

    if (fd < 0) 
    {
		if (errno == ENOENT)
		{
			fd = open(path, OPTIONS, PERMISSION);

			// Write header
			if (fd >= 0) {
				ret = dprintf(fd, "%s\n", HEADER);
			}
		}
	}

	if (fd < 0) {
		return EAR_ERROR;
	}

    dprintf(fd, "%s;", loop->node_id);
    print_job_fd(fd, &loop->job);
    print_signature_fd(fd, &loop->signatures);
    print_loop_id_fd(fd, &loop->id);
    dprintf(fd, "%lu\n", loop->total_iterations);

	close(fd);

	if (ret < 0) return EAR_ERROR;
	return EAR_SUCCESS;
    return 0;
}
