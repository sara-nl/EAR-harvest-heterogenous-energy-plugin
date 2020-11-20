/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <common/sizes.h>
#include <common/output/debug.h>
#include <metrics/common/hwmon.h>
#include <metrics/temperature/archs/amd17.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint n_sockets;

typedef struct hwfds_s {
	uint  n_ids;
	uint *n_fds;
	int **fds;
} hwfds_t;

state_t temp_amd17_status(topology_t *topo)
{
	state_t s;
	debug("asking for status");
	if (topo->vendor != VENDOR_AMD || topo->family < FAMILY_ZEN) {
		return EAR_ERROR;
	}
	while (pthread_mutex_trylock(&lock));
	if (n_sockets == 0) {
		if (xtate_ok(s, hwmon_find_drivers("k10temp", NULL, NULL))) {
			n_sockets = topo->socket_count;
		}
	}
	pthread_mutex_unlock(&lock);
	return s;
}

state_t temp_amd17_init(ctx_t *c)
{
	uint n_ids;
	uint *ids;
	state_t s;
	int i;

	debug("asking for init");
	if (c == NULL) {
		return_msg(EAR_BAD_ARGUMENT, Generr.input_null);
	}
	// Looking for ids
	if (xtate_fail(s, hwmon_find_drivers("k10temp", &ids, &n_ids))) {
		return s;
	}
	// Allocating context
	if ((c->context = calloc(1, sizeof(hwfds_t))) == NULL) {
		return_msg(s, strerror(errno));
	}
	// Allocating file descriptors)
	hwfds_t *h = (hwfds_t *) c->context;
	
	if ((h->fds = calloc(n_ids, sizeof(int *))) == NULL) {
		return_msg(s, strerror(errno));
	}
	if ((h->n_fds = calloc(n_ids, sizeof(uint))) == NULL) {
		return_msg(s, strerror(errno));
	}
	h->n_ids = n_ids;
	//
	for (i = 0; i < h->n_ids; ++i) {
		if (xtate_fail(s, hwmon_open_files(ids[i], Hwmon.temp_input, &h->fds[i], &h->n_fds[i]))) {
			return s;
		}
	}
	// Freeing ids space
	free(ids);
	debug("init succeded");
	return EAR_SUCCESS;
}

state_t temp_amd17_dispose(ctx_t *c)
{
	hwfds_t *h;
	int i, j, r;

	if (c == NULL || c->context == NULL) {
		return EAR_SUCCESS;
	}

	h = (hwfds_t *) c->context;
	for (i = 0; i < h->n_ids; ++i) {
		hwmon_close_files(h->fds[i], h->n_fds[i]);
	}
	free(h->n_fds);
	free(h->fds);

	c->context = NULL;
	return EAR_SUCCESS;
}

static llong parse(char *buffer, int n)
{
	char aux[SZ_NAME_SHORT];
	int i = 0;
	if (n < 0) {
		n = -n;
		i = sizeof(buffer) - n;
	}
	while(n > 0)
	{
		aux[i] = buffer[i];
		++i;
		--n;
	}
	aux[i] = '\0';
	debug("parsed word '%s'", aux);
	return atoll(aux);
}

state_t temp_amd17_read(ctx_t *c, llong *temp, llong *average)
{
	char data[SZ_PATH];	
	int i, j, k;
	hwfds_t *h;
	state_t s;
	llong aux;
	
	if (c == NULL || c->context == NULL || temp == NULL) {
		return_msg(EAR_BAD_ARGUMENT, Generr.input_null);
	}

	h = (hwfds_t *) c->context;

	for (k = 0, aux = 0; k < n_sockets; ++k) {
		temp[k] = 0;
	}

	for (i = 0, k = 0; i < h->n_ids && k < n_sockets; ++i) {
		for (j = 0; j < h->n_fds[i] && k < n_sockets; ++j, ++k) {
			if (xtate_fail(s, hwmon_read(h->fds[i][j], data, SZ_PATH))) {
				return s;
			}
			debug("read %s", data);
			temp[k] = parse(data, 2);
			aux    += temp[k];
		}
	}

	if (average != NULL) {
		*average = aux / (llong) n_sockets;
	}

	return EAR_SUCCESS;
}
