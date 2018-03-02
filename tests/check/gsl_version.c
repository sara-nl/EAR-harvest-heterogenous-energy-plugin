#include <gsl/gsl_version.h>
#include <assert.h>

// VERSION 1.4
#define MIN_GSL_MAJOR_VERSION 1
#define MIN_GSL_MINOR_VERSION 4

int main(void)
{
	assert(GSL_MAJOR_VERSION  > MIN_GSL_MAJOR_VERSION || 
	      (GSL_MAJOR_VERSION == MIN_GSL_MAJOR_VERSION &&
	       GSL_MINOR_VERSION >= MIN_GSL_MINOR_VERSION));

	return 0;
}

