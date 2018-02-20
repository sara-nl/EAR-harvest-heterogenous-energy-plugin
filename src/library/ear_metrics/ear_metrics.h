#ifndef EAR_EAR_METRICS_H
#define EAR_EAR_METRICS_H

#include <types/application.h>

//
long long metrics_time();

//
int metrics_init(int my_id);

//
void metrics_dispose(application_t *metrics);

//
void metrics_compute_signature_begin();

//
int metrics_compute_signature_finish(application_t *metrics, uint iterations, ulong min_time_us);

/*
 *
 *
 *
 * LEGACY CODE
 *
 *
 *
 *
 */
long long metrics_usecs_diff(long long end, long long init);
void metrics_get_hw_info(int *sockets, int *cores_socket, ulong *max_f, ulong *min_f, ulong *nom_f,
						 char *CPU_model, char *CPU_name);
void metrics_get_app_name(char *app_name);
#endif //EAR_EAR_METRICS_H
