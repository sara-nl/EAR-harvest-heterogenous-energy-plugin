#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cpufreq.h>
#include <common/file.h>
#if 1
#include <library/dynais/dynais_old.h>
#else
#include <library/dynais/dynais.h>
#endif
#include <library/micro_library/micro.h>

char file[1024];
char host[1024];
unsigned int met_calls;
unsigned int met_loops;
unsigned int met_cpu;
int fd_lock;
int mode;

void micro_init()
{
	char *var = NULL;
	int res = 0;
	int cpu = 0;
	int win = 200;

	// Mode
	//mode = 0; // Disabled
	  mode = 1; // DynAIS master
	//mode = 2; // DynAIS all

	// Dynais
	var = getenv("MICRO_MODE");

	if (var != NULL) {
		mode = atoi(var);
	}

	if (!mode) {
		return;
	}

	gethostname(host, 1024);
	sprintf(file, "/home/xjaneas/lock.%s", host);


	if (mode == 1)
	{
		if ((fd_lock = file_lock_master(file)) < 0) {
			mode = 0;
		}
	}
	
    if (!mode) {
        return;
    }

	//
	var = getenv("MICRO_WINDOW");

	if (var != NULL) {
		win = atoi(var);
	}

	dynais_init(win, 10);

	met_cpu = sched_getcpu();
	fprintf(stderr, "MICRO INIT, host '%s'-'%u', dynais %u\n", host, met_cpu, win);

#if 0
	// CPUPower performance
	var = getenv("MICRO_POWER");
	power = (var != NULL) && (var[0] == '1');

	if (power) {
		cpu = sched_getcpu();
		res = cpufreq_modify_policy_governor(cpu, "performance");
	}

	// Summary
	fprintf(stderr, "MICRO INIT, host '%s' ('%up','%uc','%dr')\n", host, power, cpu, res);
#endif
}

void micro_call(mpi_call call_type, p2i buf, p2i dest)
{
	unsigned long  event_l;
	#if 1
	unsigned long event_s;
    int result;
    unsigned int level;
    unsigned int size;
	#else
	unsigned short event_s;
	short result;
	unsigned short level;
	unsigned short size;
	#endif

	if (!mode) {
		return;
	}

	#if 1
	event_l = (unsigned long) ((((buf >> 5) ^ dest) << 5) | call_type);
	event_s = event_l;
	#else
	event_l = (unsigned long) ((((buf >> 5) ^ dest) << 5) | call_type);
	event_s = dynais_sample_convert(event_l);
	#endif
	result = dynais(event_s, &size, &level);

	met_calls += 1;
	met_loops += result >= 3;

	if (result >= 3) {
		fprintf(stderr, "%lu %u\n", event_s, size);
	}
}

void micro_end()
{
	if (!mode) {
        return;
    }

	file_unlock_master(fd_lock, file);

	fprintf(stderr, "MICRO END, host '%s'-'%u'-'%u' (calls: %u, loops: %u)\n",
		host, sched_getcpu(), met_cpu, met_calls, met_loops);
}
