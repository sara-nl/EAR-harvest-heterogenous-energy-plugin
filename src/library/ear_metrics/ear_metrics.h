#ifndef EAR_EAR_METRICS_H
#define EAR_EAR_METRICS_H

#include <common/types/application.h>

//
long long metrics_time();

//
long long metrics_usecs_diff(long long end, long long init);

//
int metrics_init();

//
void metrics_dispose(application_t *metrics);

//
void metrics_compute_signature_begin();

//
int metrics_compute_signature_finish(application_t *metrics, uint iterations, ulong min_time_us);
#endif //EAR_EAR_METRICS_H
