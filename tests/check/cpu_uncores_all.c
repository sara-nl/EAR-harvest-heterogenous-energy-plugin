#include <stdio.h>
#include <assert.h>
#include <ear_uncores.h>
#include <hardware.h>
#include <config.h>

int EAR_VERBOSE_LEVEL = 4;

int main(void)
{
	int result;

	init_uncores(get_model());
	result = check_uncores();
	dispose_uncores();
	assert(result == EAR_SUCCESS);

	return 0;
}

