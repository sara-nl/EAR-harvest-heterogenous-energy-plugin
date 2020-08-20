#include <stdio.h>
#include <unistd.h>
#include <metrics/flops/flops.h>

#define ret(fun) \
    retval = fun; \
    printf("------- " #fun " returned %d\n", retval);

void stress()
{
	#if 0
	int i;
	double d1;
	double d2;
	for (i = 0; i < 1024*1024*1024; ++i)
	{
		d1 = d1 + 3;
		d2 = d2 + d1/((double) i);
	}
	#endif
	sleep(1);
}

int main(int argc, char *argv[])
{
	ullong ops[8];
	ullong flops;
	int retval;
	int i;

	ret(init_flops_metrics());
	if (retval == -1) return 0;

	reset_flops_metrics();
	start_flops_metrics();

	stress();

	stop_flops_metrics(&flops, ops);
	
	for (i = 0; i < 8; ++i) {
		printf("%llu ", ops[i]);
	}
	printf("\n");

	return 0;
}
