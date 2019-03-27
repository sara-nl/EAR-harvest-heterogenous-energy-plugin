#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cpufreq.h>
#include <common/file.h>
#include <library/dynais/dynais.h>
#include <library/micro_library/micro.h>

char file[1024];
char host[1024];

int fd_master_lock;
int disabled;
int power;

unsigned int met_calls;
unsigned int met_loops;

void micro_init()
{
	int res = 0;
	int cpu = 0;
	char *var = NULL;

	// Dynais
	var = getenv("MICRO_DISABLED");
	disabled = (var != NULL) && (var[0] == '1');;

	if (disabled) {
		return;
	}

	gethostname(host, 1024);
	sprintf(file, "/home/xjaneas/lock.%s", host);

	if ((fd_master_lock = file_lock_master(file)) < 0) {
		disabled = 1;
	} else {
		disabled = 0;
	}
	
	//fprintf(stderr, "MICRO INITIALIZING (disabled -> %u)\n", disabled);

    if (disabled) {
        return;
    }

	dynais_init(100, 10);

	// CPUPower performance
	var = getenv("MICRO_POWER");
	power = (var != NULL) && (var[0] == '1');

	if (power) {
		cpu = sched_getcpu();
		res = cpufreq_modify_policy_governor(cpu, "performance");
	}

	// Summary
	fprintf(stderr, "MICRO INIT, host '%s' ('%up','%uc','%dr')\n", host, power, cpu, res);
}

void micro_call(mpi_call call_type, p2i buf, p2i dest)
{
	unsigned long  event_l;
	unsigned short event_s;
	unsigned short result;
	unsigned short level;
	unsigned short size;

	if (disabled) {
		return;
	}

	event_l = (unsigned long) ((((buf >> 5) ^ dest) << 5) | call_type);
	event_s = dynais_sample_convert(event_l);
	result = dynais(event_s, &size, &level);

	met_calls += 1;
	met_loops += result >= 3;
}

void micro_end()
{
	if (disabled) {
        return;
    }

	file_unlock_master(fd_master_lock, file);
	fprintf(stderr, "MICRO END, %u calls and %u loops detected\n", met_calls, met_loops);
}
