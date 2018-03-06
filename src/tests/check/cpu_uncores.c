#include <stdio.h>
#include <assert.h>
#include <metrics/custom/bandwidth.h>
#include <metrics/custom/hardware_info.h>
#include <common/states.h>

int EAR_VERBOSE_LEVEL = 4;

int main(void)
{
	int result;

	init_uncores(get_model());
	result = check_uncores();
	dispose_uncores();
	assert(result == EAR_SUCCESS || result == EAR_WARNING);

	return 0;
}

