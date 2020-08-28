#include <stdio.h>
#include <unistd.h>
#include <metrics/temperature/temperature.h>

#define ret(fun) \
    retval = fun; \
    printf("------- " #fun " returned %d\n", retval);

void stress()
{
	sleep(1);
}

ullong vals1[512];
ullong vals2[512];
ullong valsD[512];

int main(int argc, char *argv[])
{
	int fds[512];
	int retval;
	int i;

	ret(init_temp_msr(fds));
	if (retval == -1) return 0;

	ret(read_temp_msr(fds, vals1));

	stress();

	ret(read_temp_msr(fds, vals2));

	for (i = 0; i < 64; ++i)
	{
		printf("%llu ", vals2[i]);
	}
	printf("\n");

	return 0;
}
