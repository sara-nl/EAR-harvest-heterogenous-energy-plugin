#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <common/monitor.h>
#include <metrics/gpu/gpu.h>

int main(int argc, char *argv[])
{
	uint dev_count;
	state_t s;
	ctx_t c;
	int d;

	if (xtate_fail(s, gpu_load())) {
		printf("gpu_load returned %d (%s)\n", s, state_msg);
		return 0;
	}

	if (xtate_fail(s, gpu_init(&c))) {
		printf("gpu_init returned %d (%s)\n", s, state_msg);
		return 0;
	}
	
	if (xtate_fail(s, gpu_count(&c, &dev_count))) {
		printf("gpu_count returned %d (%s)\n", s, state_msg);
		return 0;
	}
	
	gpu_t *data3;
	gpu_t *data2;
	gpu_t *data1;
	
	if (xtate_fail(s, gpu_data_alloc(&data1))) {
		printf("gpu_data_alloc returned %d (%s)\n", s, state_msg);
		return 0;
	}
	if (xtate_fail(s, gpu_data_alloc(&data2))) {
		printf("gpu_data_alloc returned %d (%s)\n", s, state_msg);
		return 0;
	}
	if (xtate_fail(s, gpu_data_alloc(&data3))) {
		printf("gpu_data_alloc returned %d (%s)\n", s, state_msg);
		return 0;
	}
	
	if (xtate_fail(s, monitor_init())) {
		printf("monitor_init returned %d (%s)\n", s, state_msg);
		return 0;
	}
	
	if (xtate_fail(s, gpu_read(&c, data1))) {
		printf("gpu_read returned %d (%s)\n", s, state_msg);
		return 0;
	}
	
	while(1)
	{
		sleep(1);
		if (xtate_fail(s, gpu_read_copy(&c, data2, data1, data3))) {
			printf("gpu_read_copy returned %d (%s)\n", s, state_msg);
			return 0;
		}
	}

	return 0;
}
