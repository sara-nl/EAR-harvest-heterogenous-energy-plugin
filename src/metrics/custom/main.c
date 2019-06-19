#include <metrics/custom/energy.h>

//gcc -I ../../ -I/hpc/opt/freeipmi/include -o main main.c energy.o hardware_info.o ../common/cpuid.o ../../common/libcommon.a energy/finder.o -ldl -L/hpc/opt/freeipmi/lib -Wl,-rpath,/hpc/opt/freeipmi/lib -lfreeipmi

int main(int argc, char *argv)
{
	ulong energy;
	ehandler_t handler;
	energy_init(&handler);
	energy_dc_read(&handler, &energy);
	fprintf(stderr, "ENERGY %s MJ\n", energy);

	return 0;
}
