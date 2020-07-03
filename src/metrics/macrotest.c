#include <stdio.h>
#include <unistd.h>
#include <metrics/bandwidth/bandwidth.h>

// /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -I ../ -g -o macrotest macrotest.c libmetrics.a ../common/libcommon.a -lpthread -ldl

#define ok(fun) \
    printf("############################# calling " #fun "\n"); \
    if (xtate_fail(s, fun)) { \
        printf(#fun " returned error %d (%s)\n", s, state_msg); \
        return 0; \
    } 

int main(int argc, char *argv[])
{
	ullong cas[100];
	state_t s;
	int i;
	int j;

	i = init_uncores(0);
	
	fprintf(stderr, "number of devices 1: %u\n", i);

	i = count_uncores();
	
	fprintf(stderr, "number of devices 2: %u\n", i);

	reset_uncores();

	start_uncores();

	sleep(4);
	
	stop_uncores(cas);

	for (j = 0; j < i; ++j) {
		fprintf(stderr, "%llu ", cas[j]);
	}
	fprintf(stderr, "\n");

	return 0;
}
