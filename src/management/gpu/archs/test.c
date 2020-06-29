#include "nvml.h"

#define ok(fun) \
	if (xtate_fail(s, fun)) { \
		printf(#fun " returned error %d (%s)\n", s, state_msg); \
		return 0; \
	} 

#define printa(d, n) \
	for (i = 0; i < n; ++i) { \
		printf("%d %d\n", i, d [i]); \
	} 

int main(int argc, char *argv[])
{
	uint watts[2];
	uint khz[2];
	state_t s;
	ctx_t c;
	int i;

	ok(management_nvml_init(&c));

	ok(nvml_clock_limit_get_current(&c, khz));
	printa(khz, 2);

	ok(nvml_clock_limit_get_default(&c, khz));
	printa(khz, 2);

	ok(nvml_clock_limit_get_max(&c, khz));
	printa(khz, 2);

	ok(nvml_power_limit_get_current(&c, watts));
	printa(watts, 2);

	ok(nvml_power_limit_get_default(&c, watts));
	printa(watts, 2);

	ok(nvml_power_limit_get_max(&c, watts));
	printa(watts, 2);

#if 1
	watts[0] = 120;
	watts[1] = 120;
	ok(nvml_power_limit_set(&c, watts));
	
	ok(nvml_power_limit_get_current(&c, watts));
	printa(watts, 2);
	
	ok(nvml_power_limit_reset(&c));

	ok(nvml_power_limit_get_current(&c, watts));
	printa(watts, 2);
#endif

	khz[0] = 1111*1000;
	khz[1] = 1011*1000;
	ok(nvml_clock_limit_set(&c, khz));

	ok(nvml_clock_limit_get_current(&c, khz));
	printa(khz, 2);
	
	ok(nvml_clock_limit_reset(&c));

	ok(nvml_clock_limit_get_current(&c, khz));
	printa(khz, 2);
	
	return 0;
}
