#include <papi.h>
#include <assert.h>
#include <stdio.h>

// VERSION 5.4.3.0

int main(void)
{
	assert(PAPI_VERSION >= PAPI_VERSION_NUMBER(5,4,3,0));
	return 0;
}
