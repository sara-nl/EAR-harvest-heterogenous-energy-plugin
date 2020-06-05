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

#define SHOW_DEBUGS 1
#include <error.h>
#include <string.h>
#include <pthread.h>
#include <common/monitor.h>
#include <common/output/debug.h>

#define N_QUEUE 128

static pthread_mutex_t	lock_gen = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t	lock[N_QUEUE];
static pthread_t		thread;
static uint				enabled;

typedef struct wait_s {
	int relax;
	int burst;
} wait_t;

typedef struct register_s
{
	suscription_t suscription;
	wait_t wait_units;
	wait_t wait_saves;
	int	suscribed;
	int	delivered;
	int	bursted;
	int	aligned;
	int	ok_init;
	int	ok_main;
} register_t;

static register_t		queue[N_QUEUE];
static uint				queue_last;

static void monitor_sleep(int wait_units, int *pass_units, int *alignment)
{
	timestamp_t time;
	ullong units;

	//
	units = ((ullong) wait_units) * 100LL;
	timestamp_revert(&time, &units, TIME_MSECS);

	// Sleeping
	timestamp_t t2;
	timestamp_t t1;

	debug("waiting %lld units", units);

	timestamp_getfast(&t1);
	nanosleep(&time, NULL);
	timestamp_getfast(&t2);

	// If s is 0 then all want ok and the time
	// processing continues normally.
	*pass_units = ((int) timestamp_diff(&t2, &t1, TIME_MSECS)) + 1;
	*pass_units = *pass_units / 100;

	*alignment += *pass_units;
	*alignment %= 10;

	debug("waited %d units (alignment %d)",
		*pass_units, *alignment);
}

static void monitor_time_calc(register_t *reg, int *wait_units, int pass_units, int alignment)
{
	int wait_required;

	// Alignment processing
	if (!reg->aligned)
	{
		reg->aligned = (alignment == 0);
		reg->ok_init = reg->aligned && (reg->suscription.call_init != NULL);
		reg->ok_main = reg->aligned;

		if (!reg->ok_main) {
			wait_required = 10 - alignment;

			if (wait_required < *wait_units) {
				*wait_units = wait_required;
			}
		
			return;
		}

		
		debug("sus %d, aligned suscription", reg->suscription.id);
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
	int wait_units = 0;
	int pass_units = 0;
	int alignment = 0;
	int i;

	while (enabled)
	{
		for (i = 0, wait_units = 10; i < queue_last; ++i)
		{
			reg = &queue[i];
			sus = &reg->suscription;
		
			while (pthread_mutex_trylock(&reg->lock));

			if (!reg->suscribed) {
				continue;
			}

			monitor_time_calc(reg, &wait_units, pass_units, alignment);

			if (reg->ok_init) {
				debug("sus %d, called init", i);
				sus->call_init(sus->memm_init);
				reg->ok_init = 0;
			}
			if (reg->ok_main) {
				debug("sus %d, called main", i);
				sus->call_main(sus->memm_main);
				reg->ok_main = 0;
			}
		}

		pthread_mutex_unlock(&reg->lock);

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

static state_t monitor_register_void(void *p)
{
	suscription_t *s = (suscription_t *) p;
	return monitor_register(s);
}

state_t monitor_register(suscription_t *s)
{
	if (s             == NULL) return_msg(EAR_BAD_ARGUMENT, "the suscription can't be NULL");
	if (s->time_relax  < 100 ) return_msg(EAR_BAD_ARGUMENT, "time can't be zero");
	if (s->call_main  == NULL) return_msg(EAR_BAD_ARGUMENT, "reading call is NULL");
	if (s->id          < 0   ) return_msg(EAR_BAD_ARGUMENT, "incorrect suscription index");

	while (pthread_mutex_trylock(&lock));

	queue[s->id].suscribed      = 1;
	queue[s->id].aligned		= 0;
	queue[s->id].bursted        = 0;
	queue[s->id].wait_saves.relax = (s->time_relax / 100);
	queue[s->id].wait_saves.burst = (s->time_burst / 100);

	debug("registered suscription %d", s->id);
	debug("times relax/burst: %d/%d", queue[s->id].wait_saves.relax, queue[s->id].wait_saves.burst);
	debug("calls init/main: %p/%p (%d)", s->call_init, s->call_main, queue[s->id].ok_init);

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

int monitor_is_bursting(suscription_t *s)
{
	return sus[s->id].bursted;
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

	queue[i].suscription.id			= i;
	queue[i].delivered				= 1;
	queue[i].suscription.suscribe	= monitor_register_void;
	queue[i].lock = 
	
	if (queue[i].suscription.id >= queue_last) {
		queue_last = queue[i].suscription.id + 1;
	}

	pthread_mutex_unlock(&lock);

	return &queue[i].suscription;
}
