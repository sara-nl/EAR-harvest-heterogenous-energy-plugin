#include <metrics/custom/energy.h>
#include <metrics/ipmi/energy_node/lenovo_nm.h>

//gcc -I ../../ -I/hpc/opt/freeipmi/include -o main main.c energy.o hardware_info.o ../common/cpuid.o ../../common/libcommon.a energy/finder.o -ldl -L/hpc/opt/freeipmi/lib -Wl,-rpath,/hpc/opt/freeipmi/lib -lfreeipmi

int main(int argc, char *argv)
{
	ulong energy;
	ehandler_t handler;
	energy_init(&handler);
	energy_dc_read(&handler, &energy);
	energy_dispose(&handler);
	fprintf(stderr, "ENERGY DC %lu MJ\n", energy);

#if 0
	lenovo_act_node_energy_init((ipmi_ctx_t *) &handler.context);
	lenovo_act_read_dc_energy((ipmi_ctx_t) handler.context, &energy);
	fprintf(stderr, "LENOVO ACT %lu MJ\n", energy);
	
	energy_dc_read(&handler, &energy);
	fprintf(stderr, "ENERGY DC %lu MJ\n", energy);
#endif

	return 0;
}
