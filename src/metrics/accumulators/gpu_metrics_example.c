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
	gpu_power_t *pr;
	gpu_power_t *pd;
	pcontext_t c;
	int n = 100;
	int m; 
	int s;
	int i;

	s = acc_gpu_metrics_init(&c, &pr, &pd);
	printf("power_gpu_init: %d\n", s);

	if (state_fail(s)) {
		return 0;
	}

	s = acc_gpu_metrics_count(&c, &m);
	printf("power_gpu_init: %d (%u)\n", s, m);
	
	if (state_fail(s)) {
		return 0;
	}

	while (n > 0)
	{ 
		s = acc_gpu_metrics_read(&c, pr, pd);
		printf("power_gpu_read: %d\n", s);

#if 1
		#define gprint(d, i) \
		printf("correct: %u, ", d[i].correct); \
		printf("c_g: %u, ", d[i].freq_gpu_mhz); \
		printf("c_m: %u, ", d[i].freq_mem_mhz); \
   		printf("u_g: %u, ", d[i].util_gpu); \
    	printf("u_m: %u, ", d[i].util_mem); \
    	printf("temp_g: %u, ", d[i].temp_gpu); \
    	printf("temp_m: %u, ", d[i].temp_mem); \
		printf("power is: %f, ", d[i].power_w); \
		printf("energy is: %f\n", d[i].energy_j);
		
		i = 0;
		while (i < m) {
			gprint(pr, i);
			gprint(pd, i);
			i += 1;
		}
#endif
		
		sleep(2);
		n--;
	}

	acc_gpu_metrics_dispose(&c, &pr, &pd);
	
	return 0;
}
