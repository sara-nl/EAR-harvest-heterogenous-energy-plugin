#include <metrics/custom/energy.h>
#include <metrics/ipmi/energy_node/lenovo_nm.h>

//gcc -I ../../ -I/hpc/opt/freeipmi/include -o main main.c energy.o hardware_info.o ../common/cpuid.o ../../common/libcommon.a energy/finder.o -ldl -L/hpc/opt/freeipmi/lib -Wl,-rpath,/hpc/opt/freeipmi/lib -lfreeipmi

int main(int argc, char *argv)
{
	ulong emj1, emj2;
	ulong tms1, tms2;
	ulong ej;
	ulong ts;	

	ehandler_t handler;
	
	energy_init(&handler);
	
	energy_dc_read(&handler, &emj1);
	sleep(2);
	energy_dc_read(&handler, &emj2);
	emj2 = emj2 - emj1;
	fprintf(stderr, "energy_dc_read %lu emj\n", emj2);

	energy_dc_time_read(&handler, &emj1, &tms2);
	sleep(2);
	energy_dc_time_read(&handler, &emj1, &tms2);
	emj2 = emj2 - emj1;
	tms2 = tms2 - tms1;
	fprintf(stderr, "energy_dc_time_read %lu emj %lu tms\n", emj2, tms2);

	energy_dc_time_debug(&handler, &ej, &emj1, &ts, &tms1);
	sleep(2);
	energy_dc_time_debug(&handler, &ej, &emj2, &ts, &tms2);
	emj2 = emj2 - emj1;
	tms2 = tms2 - tms1;
	fprintf(stderr, "energy_dc_time_read %lu emj %lu tms\n", emj2, tms2);
		
	energy_dispose(&handler);

#if 0
	lenovo_act_node_energy_init((ipmi_ctx_t *) &handler.context);
	lenovo_act_read_dc_energy((ipmi_ctx_t) handler.context, &energy);
	fprintf(stderr, "LENOVO ACT %lu MJ\n", energy);
	
	energy_dc_read(&handler, &energy);
	fprintf(stderr, "ENERGY DC %lu MJ\n", energy);
#endif

	return 0;
}
