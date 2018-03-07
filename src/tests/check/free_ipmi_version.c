#include <ipmi_monitoring.h>
#include <assert.h>

// VERSION 1.0.7

int main(void)
{
	assert(LIBIPMIMONITORING_VERSION_MAJOR  > 1  || 
	      (LIBIPMIMONITORING_VERSION_MAJOR == 1  &&
	       LIBIPMIMONITORING_VERSION_MINOR  > 0) ||
	      (LIBIPMIMONITORING_VERSION_MAJOR == 1  &&
	       LIBIPMIMONITORING_VERSION_MINOR == 0  &&
	       LIBIPMIMONITORING_VERSION_PATCH >= 7) );

	return 0;
}

