#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	init_basic_metrics();
	init_cache_metrics();
	init_flops_metrics();
	//init_stall_metrics();
	//init_rapl_metrics();

	reset_basic_metrics();
	reset_cache_metrics();
	reset_flops_metrics();
	//reset_stall_metrics();
	//reset_rapl_metrics();

	long long cycles;
	long long instructions;
	long long stalls;
	long long l1;
	long long l2;
	long long l3;
	long long flops;
	long long ops;
	long long energy;
	double f1;
	double f2;
	double f3;
	long i;
	int n;

	if (argc == 1) {
		n = 10;
	} else {
		n = atoi(argv[1]);

	}

	while (n > 0)
	{
		start_basic_metrics();
		start_cache_metrics();
		start_flops_metrics();
		//start_stall_metrics();
		//start_rapl_metrics();

		for (i = 0; i < 1000000000; ++i)
		{
			f1 = 179425823.0 * (double) i;
			f2 = 8753.0 * (double) i;
			f3 = f3 + (f1 / f2);
		}

		stop_basic_metrics(&cycles, &instructions);
		stop_cache_metrics(&l1, &l2, &l3);
		stop_flops_metrics(&flops, &ops);
		//stop_stall_metrics(&stalls);
		//stop_rapl_metrics(&energy);

		printf("cycles: %lld\n", cycles);
		printf("intrusctions: %lld\n", instructions);
		printf("flops: %lld\n", flops);
		--n;
	}

	return 0;
}
