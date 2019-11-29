#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <metrics/energy/energy_gpu.h>
#include <metrics/accumulators/gpu_metrics.h>

// Compile:
// icc -I ../../ -o ex gpu_metrics_example.c ../libmetrics.a ../../common/libcommon.a

int main(int argc, char *argv[])
{
	gpu_energy_t *dr1, *dr2, *dra;
	pcontext_t c;
	int n = 100;
	int m; 
	int s;
	int i;

	s = acc_gpu_metrics_init(&c);
	printf("power_gpu_init: %d\n", s);

	if (state_fail(s)) {
		return 0;
	}

	s = acc_gpu_metrics_count(&c, &m);
	printf("power_gpu_init: %d (%u)\n", s, m);
	
	if (state_fail(s)) {
		return 0;
	}

	s = acc_gpu_metrics_data_alloc(&c, &dr1);
	printf("power_gpu_data_alloc: %d\n", s);
	s = acc_gpu_metrics_data_alloc(&c, &dr2);
	printf("power_gpu_data_alloc: %d\n", s);
	s = acc_gpu_metrics_data_alloc(&c, &dra);
	printf("power_gpu_data_alloc: %d\n", s);

	while (n > 0)
	{ 
		s = acc_gpu_metrics_read(&c, dr1);
		printf("power_gpu_read: %d\n", s);
		
		sleep(2);

		s = acc_gpu_metrics_read(&c, dr2);
		printf("power_gpu_read: %d\n", s);

		s = acc_gpu_metrics_data_diff(&c, dr1, dr2, dra);
		printf("power_gpu_data_diff: %d\n", s);
#if 1
		#define gprint(d, i) \
		printf("correct: %u, ", d[i].correct); \
		printf("c_g: %lu, ", d[i].freq_gpu_mhz); \
		printf("c_m: %lu, ", d[i].freq_mem_mhz); \
   		printf("u_g: %lu, ", d[i].util_gpu); \
    	printf("u_m: %lu, ", d[i].util_mem); \
    	printf("temp_g: %lu, ", d[i].temp_gpu); \
    	printf("temp_m: %lu, ", d[i].temp_mem); \
		printf("power is: %0.2lf W, ", d[i].power_w); \
		printf("energy is: %0.2lf J\n", d[i].energy_j);
		
		i = 0;
		while (i < m) {
			gprint(dra, i);
			i += 1;
		}
#endif
		
		n--;
	}

	acc_gpu_metrics_dispose(&c);
	
	return 0;
}
