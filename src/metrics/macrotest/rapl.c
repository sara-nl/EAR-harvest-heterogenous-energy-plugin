#include <stdio.h>
#include <unistd.h>
#include <metrics/energy/cpu.h>

#define ret(fun) \
    retval = fun; \
    printf("------- " #fun " returned %d\n", retval);

void stress()
{
	int i;
	double d1;
	double d2;
	for (i = 0; i < 1024*1024*1024; ++i)
	{
		d1 = d1 + 3;
		d2 = d2 + d1/((double) i);
	}
	sleep(2);
}

ullong vals1[512];
ullong vals2[512];
ullong valsD[512];

int main(int argc, char *argv[])
{
	int fds[512];
	int retval;
	int i;

	ret(init_rapl_msr(fds));

	ret(read_rapl_msr(fds, vals1));

	stress();

	ret(read_rapl_msr(fds, vals2));
	
	diff_rapl_msr_energy(valsD, vals2, vals1);

	for (i = 0; i < 512; ++i) {
		double val = ((double) valsD[i]) / RAPL_MSR_UNITS;
		//printf("%llu ", vals1[i]);
		//printf("%llu ", vals2[i]);
		//printf("%llu ", valsD[i]);
		//printf("%0.2lf ", val);
	}
	printf("\n");

	return 0;
}
