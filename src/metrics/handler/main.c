#include <metrics/handler/energy.h>
//#include <metrics/ipmi/energy_node/lenovo_nm.h>

//gcc -I ../../ -I/hpc/opt/freeipmi/include -o main main.c energy.o ../custom/hardware_info.o ../common/cpuid.o ../../common/libcommon.a ../finder/energy.o -ldl -L/hpc/opt/freeipmi/lib -Wl,-rpath,/hpc/opt/freeipmi/lib -lfreeipmi -lpthread

char extra_buffer[4096];

int main(int argc, char *argv)
{
	cluster_conf_t conf_clus;
	ehandler_t handler;
	ulong emj1, emj2;
	ulong tms1, tms2;
	state_t s;

	// Configuration
	if (get_ear_conf_path(extra_buffer) == EAR_ERROR) {
		fprintf(stderr, "while getting ear.conf path");
		return 0;
	}

	fprintf(stderr, "reading '%s' configuration file\n", extra_buffer);
	read_cluster_conf(extra_buffer, &conf_clus);
	
	// Reading
	s = energy_init(&conf_clus, &handler);
	fprintf(stderr, "init (%d)\n", s);
	
	s = energy_dc_time_read(&handler, &emj1, &tms1);
	fprintf(stderr, "e1 %lu _ %lu (%d)\n", emj1, tms1, s);
	
	sleep(2);
	
	s = energy_dc_time_read(&handler, &emj2, &tms2);
	fprintf(stderr, "e2 %lu _ %lu (%d)\n", emj2, tms2, s);

	tms2 = (tms2 - tms1) / 1000;	
	emj2 = ((emj2 - emj1) / 1000) / tms2;
	fprintf(stderr, "time %lu S, power %lu W\n", tms2, emj2);

#if 0
	energy_dc_time_read(&handler, &emj1, &tms2);
	sleep(2);
	energy_dc_time_read(&handler, &emj1, &tms2);
	emj2 = emj2 - emj1;
	tms2 = tms2 - tms1;
	fprintf(stderr, "energy_dc_time_read %lu emj %lu tms\n", emj2, tms2);
#endif

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
