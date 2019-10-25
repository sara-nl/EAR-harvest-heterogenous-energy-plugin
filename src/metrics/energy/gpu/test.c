#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "time.h"
#include "api-nvml-smi.h"

int main(int argc, char *argv[])
{
	gpu_power_t *pr;
	gpu_power_t *pd;
	pcontext_t c;
	int n = 100; 
	int s;
	int i;

	s = power_gpu_init(&c, &pr, &pd, 2);
	printf("power_gpu_init: %d\n", s);

	while (n > 0)
	{ 
		s = power_gpu_read(&c, pr, pd, 2);
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
		while (i < 2) {
			gprint(pr, i);
			gprint(pd, i);
			i += 1;
		}
#endif
		
		sleep(2);
		n--;
	}

	power_gpu_dispose(&c, &pr, &pd);
	
	return 0;
}
