/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, transparent and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING
*/

#include <error.h>
#include <string.h>
#include <pthread.h>
#include <common/monitor.h>

#define N_QUEUE 128

static pthread_mutex_t	lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t 		thread;
static uint				enabled;

typedef struct wait_s {
	int relax;
	int burst;
} wait_t;

typedef struct register_s
{
	suscription_t	suscription;
	wait_t 			wait_units;
	wait_t			wait_saves;
	int				suscribed;
	int				delivered;
	int				bursted;
	int				aligned;
	int				ok_init;
	int			 	ok_main;
} register_t;

static register_t	queue[N_QUEUE];
static uint			queue_last;

static void monitor_sleep(int wait_units, int *pass_units, int *alignment)
{
	timestamp_t time;
	llong units;
	int s;

	//
	units = ((llong) wait_units) * 100LL;
	timestamp_revert(&time, &units, TIME_MSECS);

	// Sleeping
	timestamp_t t2;
	timestamp_t t1;

	debug("MONITOR: waiting %d units", units);

	timestamp_getfast(&t1);
	nanosleep(&time, NULL);
	timestamp_getfast(&t2);

	// If s is 0 then all want ok and the time
	// processing continues normally.
	*pass_units = ((int) timestamp_diff(&t2, &t1, TIME_MSECS)) + 1;
	*pass_units = *pass_units / 100;

	*alignment += *pass_units;
	*alignment %= 10;

	debug("MONITOR: waited %d units (alignment %d)",
		*pass_units, *alignment);
}

static void monitor_time_calc(register_t *reg, int *wait_units, int pass_units, int alignment)
{
	int wait_required;

	// Alignment processing
	if (!reg->aligned)
	{
		reg->aligned = (alignment == 0);
		reg->ok_main = reg->aligned;

		if (!reg->ok_main) {
			wait_required = 10 - alignment;

			if (wait_required < *wait_units) {
				*wait_units = wait_required;
			}
		
			return;
		}
		debug("MONITOR: aligned %d", reg->suscription.id);
	}

	// Normal processing
	reg->wait_units.burst -= pass_units;
	reg->wait_units.relax -= pass_units;

	if (reg->wait_units.burst <= 0 &&  reg->bursted) reg->ok_main = 1;
	if (reg->wait_units.relax <= 0 && !reg->bursted) reg->ok_main = 1;

	if (reg->wait_units.burst <= 0) reg->wait_units.burst = reg->wait_saves.burst;
	if (reg->wait_units.relax <= 0) reg->wait_units.relax = reg->wait_saves.relax;

	if (reg->bursted && reg->wait_units.burst < *wait_units) {
		*wait_units = reg->wait_units.burst;
	} else 	if (!reg->bursted && reg->wait_units.relax < *wait_units) {
		*wait_units = reg->wait_units.relax;
	}
}

static void *monitor(void *p)
{
	register_t *reg;
	suscription_t *sus;
	int wait_units;
	int pass_units;
	int alignment;
	int i;

	while (enabled)
	{
		while (pthread_mutex_trylock(&lock));

		for (i = 0, wait_units = 10; i < queue_last; ++i)
		{
			reg = &queue[i];
			sus = &reg->suscription;

			if (!reg->suscribed) {
				continue;
			}

			monitor_time_calc(reg, &wait_units, pass_units, alignment);

			if (reg->ok_main) {
				sus->call_main(sus->memm_main);
				reg->ok_main = 0;
			}
		}

		pthread_mutex_unlock(&lock);

		monitor_sleep(wait_units, &pass_units, &alignment);
	}
	return EAR_SUCCESS;
}

state_t monitor_init()
{
	int _errno;

	if (enabled != 0) {
		return_msg(EAR_ERROR, "monitor already enabled");
	}

	//
	_errno = pthread_create(&thread, NULL, monitor, NULL);
	enabled = (errno == 0);

	if (!enabled) {
		return_msg(EAR_ERROR, strerror(_errno));
	}

	return EAR_SUCCESS;
}

state_t monitor_dispose()
{
	if (!enabled) {
		return_msg(EAR_BAD_ARGUMENT, "not initialized");
	}

	while (pthread_mutex_trylock(&lock));
	memset(queue, 0, sizeof(suscription_t) * N_QUEUE);
	enabled = 0;
	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t monitor_register(suscription_t *s)
{
	if (s             == NULL) return_msg(EAR_BAD_ARGUMENT, "the suscription can't be NULL");
	if (s->time_relax <= 100 ) return_msg(EAR_BAD_ARGUMENT, "time can't be zero");
	if (s->call_main  == NULL) return_msg(EAR_BAD_ARGUMENT, "reading call is NULL");
	if (s->id          < 0   ) return_msg(EAR_BAD_ARGUMENT, "incorrect suscription index");

	while (pthread_mutex_trylock(&lock));

	queue[s->id].ok_init		= (s->call_init == NULL);
	queue[s->id].suscribed      = 1;
	queue[s->id].aligned		= 0;
	queue[s->id].bursted        = 0;

	queue[s->id].wait_saves.relax = (s->time_relax / 100);
	queue[s->id].wait_saves.burst = (s->time_burst / 100);

	if (s->id >= queue_last) {
		queue_last = s->id + 1;
	}

	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t monitor_unregister(suscription_t *s)
{
	if (s     == NULL      ) return_msg(EAR_BAD_ARGUMENT, "the suscription can't be NULL");
	if (s->id  > queue_last) return_msg(EAR_BAD_ARGUMENT, "incorrect suscription index");
	if (s->id  < 0         ) return_msg(EAR_BAD_ARGUMENT, "incorrect suscription index");

	while (pthread_mutex_trylock(&lock));
	memset(&queue[s->id], 0, sizeof(register_t));
	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t monitor_burst(suscription_t *s)
{
	if (s == NULL) {
		return_msg(EAR_BAD_ARGUMENT, "the suscription can't be NULL");
	}
	if (s->time_burst == 0) {
		return_msg(EAR_BAD_ARGUMENT, "time cant be zero");
	}
	if (s->time_burst  < 0) {
		return_msg(EAR_BAD_ARGUMENT, "burst time cant be less than relax time");
	}

	while (pthread_mutex_trylock(&lock));
	queue[s->id].bursted = 1;
	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

state_t monitor_relax(suscription_t *s)
{
	if (s == NULL) {
		return_msg(EAR_BAD_ARGUMENT, "the suscription can't be NULL");
	}

	while (pthread_mutex_trylock(&lock));
	queue[s->id].bursted = 0;
	pthread_mutex_unlock(&lock);

	return EAR_SUCCESS;
}

suscription_t *suscription()
{
	int i = 0;

	while (pthread_mutex_trylock(&lock));

	for (; i < N_QUEUE; ++i)
	{
		if (queue[i].delivered == 0) {
			break;
		}
	}

	if (queue_last == N_QUEUE) {
		pthread_mutex_unlock(&lock);
		return NULL;
	}

	printf("returning suscription %d\n", i);
	queue[i].suscription.id = i;
	queue[i].delivered      = 1;
	pthread_mutex_unlock(&lock);

	return &queue[i].suscription;
}
