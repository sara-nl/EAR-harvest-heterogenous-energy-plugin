#ifndef EAR_EAR_METRICS_H
#define EAR_EAR_METRICS_H

#include <common/types/application.h>

/** Returns the current time in usecs */
long long metrics_time();

/** Returns the difference between times in usecs */
long long metrics_usecs_diff(long long end, long long init);

/** Initializes local metrics as well as daemon's metrics */
int metrics_init();

/** Stops metrics collection and computes the accumulated data*/
void metrics_dispose(application_t *metrics);

/** Restarts the current metrics and recomputes the signature */
void metrics_compute_signature_begin();

int metrics_compute_signature_finish(application_t *metrics, uint iterations, ulong min_time_us);
#endif //EAR_EAR_METRICS_H
