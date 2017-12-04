#include <ear_arch_type.h>
#include <ear_uncores.h>
#include <config.h>
#include <assert.h>
#include <stdio.h>

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

