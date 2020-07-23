#include <stdio.h>
#include <unistd.h>
#include <immintrin.h>
#include <metrics/cpi/cpi.h>
#include <metrics/flops/flops.h>
#include <metrics/frequency/cpu.h>
#include <metrics/bandwidth/bandwidth.h>

// /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -I ../ -g -o macrotest macrotest.c libmetrics.a ../common/libcommon.a -lpthread -ldl

#define ret(fun) \
	retval = fun; \
	printf(#fun " returned %d\n", i);

void stress()
{
	sleep(2);
}

int main(int argc, char *argv[])
{
	ullong *cas2;
	ullong *cas1;
	double gbs;
	int retval;

	// Init también inicializa su propia topologia
	ret(init_uncores(0));

	if (retval == 0) {
		return 0;
	}

	ret(alloc_array_uncores(&cas2));
	ret(alloc_array_uncores(&cas1));

	ret(reset_uncores());
	ret(start_uncores());

	ret(read_uncores(cas1));
	stress();
	ret(stop_uncores(cas2));

	ret(compute_uncores(cas2, cas1, &gbs, BW_GBS));

	fprintf(stderr, "Bandwidth %0.4lf GB/s\n", gbs);

	return 0;
}
