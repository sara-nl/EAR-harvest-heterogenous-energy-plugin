#include <stdio.h>
#include <unistd.h>
#include <immintrin.h>
#include <metrics/cpi/cpi.h>
#include <metrics/flops/flops.h>
#include <metrics/frequency/cpu.h>
#include <metrics/bandwidth/bandwidth.h>

ullong data[8*1024];

// /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -I ../ -g -o macrotest macrotest.c libmetrics.a ../common/libcommon.a -lpthread -ldl

#define ret(fun) \
	retval = fun; \
	printf("------- " #fun " returned %d\n", retval);

void stress()
{
	int i = 0;
	int j = 0;
	for (j = 0; j < 1024*1024; ++j)
	for (i = 0; i < 1024; i+=8) {
		data[i+0] = i+3;
		data[i+1] = i+6;
		data[i+2] = i+9;
		data[i+3] = i+3;
		data[i+4] = i+6;
		data[i+5] = i+9;
		data[i+6] = i+3;
		data[i+7] = i+6;
	}
}

int main(int argc, char *argv[])
{
	llong instructions1;
	llong instructions2;
	llong cycles1;
	llong cycles2;
	int retval;

	// Init también inicializa su propia topologia
	ret(init_basic_metrics());

	if (retval == 0) {
		return 0;
	}

	ret(reset_basic_metrics());
	ret(start_basic_metrics());

	ret(read_basic_metrics(&cycles1, &instructions1));
	stress();
	ret(stop_basic_metrics(&cycles2, &instructions2));

	fprintf(stderr, "instructions %lld\n", instructions2 - instructions1);
	fprintf(stderr, "cycles %lld\n", cycles2 - cycles1);

	return 0;
}
