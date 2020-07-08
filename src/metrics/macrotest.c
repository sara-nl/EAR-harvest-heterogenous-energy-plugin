#include <stdio.h>
#include <unistd.h>
#include <metrics/bandwidth/bandwidth.h>

// /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc -I ../ -g -o macrotest macrotest.c libmetrics.a ../common/libcommon.a -lpthread -ldl

#define ret(fun) \
	i = fun; \
	printf("FUN " #fun "returned %d\n", i);

int main(int argc, char *argv[])
{
	llong   ops[100];
	llong flops[100];
	ullong cas[100];
	ullong tfs;
	ullong gbs;

	state_t s;
	int igbs;
	int itfs;
	int i;

	ret(init_uncores(0));
	ret(init_flops_metrics());

	ret(count_uncores());
	igbs = i;
	ret(get_number_fops_events());
	itfs = i;

	ret(reset_uncores());
	ret(start_uncores());

	reset_flops_metrics();
	start_flops_metrics();

	sleep(4);
	
	ret(stop_uncores(cas));
	stop_flops_metrics(flops, ops);

	fprintf(stderr, "BANDWIDTH: ");
	for (i = 0, gbs = 0; i < igbs; ++i) {
		fprintf(stderr, "%llu ", cas[i]);
		gbs += cas[i];
	}
	gbs = (gbs*64LLU) / (1024LLU*1024LLU*1024LLU*4LLU);
	fprintf(stderr, "\nbandwidth %0.4lf GB/s\n", (double) gbs);

	fprintf(stderr, "FLOPS: ");
	for (i = 0, tfs = 0; k < itfs; ++i) {
		fprintf(stderr, "%llu ", flops[i]);
		tfs += flops[i];
	}
	tfs = (tfs) / (1024LLU*1024LLU*1024LLU*1024LLU);
	fprintf(stderr, "\nflops %0.4lf TF/s\n", (double) tfs);

	return 0;
}
